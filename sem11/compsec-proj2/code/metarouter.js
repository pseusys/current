import sjcl from "./utils/sjcl";
import { asyncMiddleware } from "./utils/asyncMiddleware";
import { generateRandomness, KDF } from "./utils/crypto";
import sleep from "./utils/sleep";

/** XSS Attack Protection **/

function escapeHtml(unsafe) {
	return unsafe.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&#039;");
}

function sanitizeOutput(output, res) {
	const oneScriptNonce = generateRandomness();
	const defaultPolicy = "default-src 'self'";
	const scriptPolicy = `script-src 'self' 'nonce-${oneScriptNonce}'`;
	const stylePolicy = "style-src 'self' fonts.googleapis.com";
	const fontPolicy = "font-src 'self' fonts.gstatic.com";
	res.header("Content-Security-Policy", `${defaultPolicy}; ${scriptPolicy}; ${stylePolicy}; ${fontPolicy}`);

	if (output.errorMsg) output.errorMsg = escapeHtml(output.errorMsg);
	return Object.assign(output, { oneScriptNonce });
}

/** CSRF Attack Protection **/

const CSRF_INPUT_NAME = "_csrf";

function initializeSession() {
	return {
		loggedIn: false,
		account: {},
		CSRF: generateRandomness()
	};
}

function checkAndRegenerateCSRF(session, formdata, request) {
	if (formdata[CSRF_INPUT_NAME] == undefined) throw new Error(`CSRF token field is missing in ${request} request: ${CSRF_INPUT_NAME}`);
	else if (formdata[CSRF_INPUT_NAME] !== session.CSRF) throw new Error(`Invalid CSRF token in ${request} request: ${CSRF_INPUT_NAME}`);
	else session.CSRF = generateRandomness();
	return session;
}

const validateCSRF = (fn) => async (req, res, next) => {
	if (Object.keys(req.query) != 0) req.session = checkAndRegenerateCSRF(req.session, req.query, "GET");
	else if (Object.keys(req.body) != 0) req.session = checkAndRegenerateCSRF(req.session, req.body, "POST");

	await fn(req, res, next);
};

/** Cookie Tampering Attack Protection **/

const AES_KEY_LENGTH = 32;
const IV_LENGTH = 8;

let SERVER_CIPHER = ((secret) => {
	let key;
	if (secret) key = sjcl.codec.hex.toBits(secret).slice(0, AES_KEY_LENGTH);
	else key = sjcl.random.randomWords(Math.trunc(AES_KEY_LENGTH / 4));
	return new sjcl.cipher.aes(key);
})(process.env.BITBAR_SECRET);

const decryptCookies = (fn) => async (req, res, next) => {
	if (req.session.secret !== undefined) {
		const session = sjcl.codec.base64.toBits(req.session.secret);
		const iv = session.slice(0, IV_LENGTH);
		const data = session.slice(IV_LENGTH);
		const decrypted = sjcl.mode.gcm.decrypt(SERVER_CIPHER, data, iv);
		req.session = JSON.parse(sjcl.codec.utf8String.fromBits(decrypted));
	} else req.session = initializeSession();

	await fn(req, res, next);
};

function encryptCookies(session) {
	const iv = sjcl.random.randomWords(IV_LENGTH);
	const data = sjcl.codec.utf8String.toBits(JSON.stringify(session));
	const encrypted = sjcl.mode.gcm.encrypt(SERVER_CIPHER, data, iv);
	return { secret: sjcl.codec.base64.fromBits(sjcl.bitArray.concat(iv, encrypted)) };
}

/** SQL Injection Attack Protection **/

export async function runSQL(dbPromise, template, ...params) {
	const db = await dbPromise;
	return await db.get(template, params);
}

/** Side-Channel Timing Attack Protection **/

const DEFAULT_DELTA = 2500;
let TIMING_DELTA = {};

const calculateLoginTime = (fn, sideChannel) => async (req, res, next) => {
	if (sideChannel) req.session.login_started = new Date();
	else req.session.login_started = undefined;

	await fn(req, res, next);
};

async function pauseOnLogin(req, errorMsg) {
	if (req.session.login_started === undefined) return;
	if (TIMING_DELTA[req.path] === undefined) TIMING_DELTA[req.path] = DEFAULT_DELTA;

	const elapsed = new Date() - req.session.login_started;
	if (errorMsg === false) TIMING_DELTA[req.path] = elapsed * 0.2 + TIMING_DELTA[req.path] * 0.8;
	else if (elapsed < TIMING_DELTA[req.path]) await sleep(TIMING_DELTA[req.path] - elapsed);
}

export function safeCheckPassword(password, dbResult) {
	const inputKDFResult = KDF(password, dbResult.salt);
	let passwordsMatch = true;
	for (let i = 0; i < dbResult.hashedPassword.length; i++) {
		if (i >= inputKDFResult.length) passwordsMatch = passwordsMatch && true;
		else passwordsMatch = passwordsMatch && inputKDFResult[i] === dbResult.hashedPassword[i];
	}
	return passwordsMatch;
}

/** Putting it all together **/

export const cookieMiddleware =
	(fn, sideChannel = false) =>
	(req, res, next) => {
		asyncMiddleware(decryptCookies(validateCSRF(calculateLoginTime(fn, sideChannel))))(req, res, next);
	};

export async function render(req, res, next, page, title, errorMsg = false, result = null) {
	await pauseOnLogin(req, errorMsg);
	const renderSession = Object.assign({}, req.session);
	req.session = encryptCookies(req.session);
	const output = sanitizeOutput({ page, title, errorMsg, result }, res);
	const renderParams = Object.assign(output, renderSession);
	res.render("layout/template", renderParams);
}
