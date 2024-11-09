import express from 'express';
import sqlite from 'sqlite';

import sleep from './utils/sleep';
import { generateRandomness, KDF } from './utils/crypto';

import { asyncMiddleware } from './utils/asyncMiddleware';
import { cookieMiddleware, render, runSQL, safeCheckPassword } from './metarouter';


const router = express.Router();
const dbPromise = sqlite.open('./db/database.sqlite')

router.get('/', cookieMiddleware(async (req, res, next) => {
  await render(req, res, next, 'index', 'Bitbar Home');
}));


router.post('/set_profile', cookieMiddleware(async (req, res, next) => {
  req.session.account.profile = req.body.new_profile;
  console.log(req.body.new_profile);
  await runSQL(dbPromise, "UPDATE Users SET profile = ? WHERE username = ?", req.body.new_profile, req.session.account.username);
  await render(req, res, next, 'index', 'Bitbar Home');
}));


router.get('/login', cookieMiddleware(async (req, res, next) => {
  await render(req, res, next, 'login/form', 'Login');
}));


router.get('/get_login', cookieMiddleware(async (req, res, next) => {
  const result = await runSQL(dbPromise, "SELECT * FROM Users WHERE username == ?", req.query.username);
  if(result) { // if this username actually exists
    if(safeCheckPassword(req.query.password, result)) { // if password is valid
      await sleep(2000);
      req.session.loggedIn = true;
      req.session.account = result;
      await render(req, res, next, 'login/success', 'Bitbar Home');
      return;
    }
  }
  await render(req, res, next, 'login/form', 'Login', 'This username and password combination does not exist!');
}, true));


router.get('/register', cookieMiddleware(async (req, res, next) => {
  await render(req, res, next, 'register/form', 'Register');
}));


router.post('/post_register', cookieMiddleware(async (req, res, next) => {
  let result = await runSQL(dbPromise, "SELECT * FROM Users WHERE username == ?", req.body.username);
  if(result) { // query returns results
    if(result.username === req.body.username) { // if username exists
      await render(req, res, next, 'register/form', 'Register', 'This username already exists!');
      return;
    }
  }
  const salt = generateRandomness();
  const hashedPassword = KDF(req.body.password, salt);
  console.log(hashedPassword);
  console.log(salt);
  await runSQL(dbPromise, "INSERT INTO Users(username, hashedPassword, salt, profile, bitbars) VALUES(?, ?, ?, ?, ?)", req.body.username, hashedPassword, salt, '', 100);
  req.session.loggedIn = true;
  req.session.account = {
    username: req.body.username,
    hashedPassword,
    salt,
    profile: '',
    bitbars: 100,
  };
  await render(req, res, next,'register/success', 'Bitbar Home');
}));


router.get('/close', cookieMiddleware(async (req, res, next) => {
  if(req.session.loggedIn == false) {
    await render(req, res, next, 'login/form', 'Login', 'You must be logged in to use this feature!');
    return;
  };
  await runSQL(dbPromise, "DELETE FROM Users WHERE username == ?", req.session.account.username);
  req.session.loggedIn = false;
  req.session.account = {};
  await render(req, res, next, 'index', 'Bitbar Home', 'Deleted account successfully!');
}));


router.get('/logout', cookieMiddleware(async (req, res, next) => {
  req.session.loggedIn = false;
  req.session.account = {};
  await render(req, res, next, 'index', 'Bitbar Home', 'Logged out successfully!');
}));


router.get('/profile', cookieMiddleware(async (req, res, next) => {
  if(req.session.loggedIn == false) {
    await render(req, res, next, 'login/form', 'Login', 'You must be logged in to use this feature!');
    return;
  };

  if(req.query.username != null) { // if visitor makes a search query
    let result;
    try {
      result = await runSQL(dbPromise, "SELECT * FROM Users WHERE username == ?", req.query.username);
    } catch(err) {
      result = false;
    }
    if(result) { // if user exists
      await render(req, res, next, 'profile/view', 'View Profile', false, result);
    }
    else { // user does not exist
      await render(req, res, next, 'profile/view', 'View Profile', `${req.query.username} does not exist!`, req.session.account);
    }
  } else { // visitor did not make query, show them their own profile
    await render(req, res, next, 'profile/view', 'View Profile', false, req.session.account);
  }
}));


router.get('/transfer', cookieMiddleware(async (req, res, next) => {
  if(req.session.loggedIn == false) {
    await render(req, res, next, 'login/form', 'Login', 'You must be logged in to use this feature!');
    return;
  };
  await render(req, res, next, 'transfer/form', 'Transfer Bitbars', false, {receiver:null, amount:null});
}));


router.post('/post_transfer', cookieMiddleware(async (req, res, next) => {
  if(req.session.loggedIn == false) {
    await render(req, res, next, 'login/form', 'Login', 'You must be logged in to use this feature!');
    return;
  };

  if(req.body.destination_username === req.session.account.username) {
    await render(req, res, next, 'transfer/form', 'Transfer Bitbars', 'You cannot send money to yourself!', {receiver:null, amount:null});
    return;
  }

  const receiver = await runSQL(dbPromise, "SELECT * FROM Users WHERE username == ?", req.body.destination_username);
  if(receiver) { // if user exists
    const amount = parseInt(req.body.quantity);
    if(Number.isNaN(amount) || amount > req.session.account.bitbars || amount < 1) {
      await render(req, res, next, 'transfer/form', 'Transfer Bitbars', 'Invalid transfer amount!', {receiver:null, amount:null});
      return;
    }

    req.session.account.bitbars -= amount;
    await runSQL(dbPromise, "UPDATE Users SET bitbars = ? WHERE username == ?", req.session.account.bitbars, req.session.account.username);
    const receiverNewBal = receiver.bitbars + amount;
    await runSQL(dbPromise, "UPDATE Users SET bitbars = ? WHERE username == ?", receiverNewBal, eceiver.username);
    await render(req, res, next, 'transfer/success', 'Transfer Complete', false, {receiver, amount});
  } else { // user does not exist
    let q = req.body.destination_username;
    if (q == null) q = '';

    let oldQ;
    while (q !== oldQ) {
      oldQ = q;
      q = q.replace(/script|SCRIPT|img|IMG/g, '');
    }
    await render(req, res, next, 'transfer/form', 'Transfer Bitbars', `User ${q} does not exist!`, {receiver:null, amount:null});
  }
}));


router.get('/steal_cookie', asyncMiddleware(async (req, res, next) => {
  let stolenCookie = req.query.cookie;
  console.log('\n\n' + stolenCookie + '\n\n');
  await render(req, res, next, 'theft/view_stolen_cookie', 'Cookie Stolen!', false, stolenCookie);
}));

router.get('/steal_password', asyncMiddleware(async (req, res, next) => {
  let password = req.query.password;
  let timeElapsed = req.query.timeElapsed;
  console.log(`\n\nPassword: ${req.query.password}, time elapsed: ${req.query.timeElapsed}\n\n`);
  res.end();
}));


module.exports = router;
