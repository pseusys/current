# COMP 4634 - Project 2, Part 2

> by Aleksandr Sergeev, student id `21158862`.

Read this file as markdown for the best expirience :)

**DISCLAIMER:**
There are _A LOT_ of things can be changed in the code, including request processing in `code/router.js`, cookie storage and values (it is not clear why `profile` or `bitbars` are stored in cookies), error processing, library versions, etc.
As far as I understood, most of these things are out of the scope of this assignment, so I decided to introduce _as few changes to the original codebase as possible_, keeping most of the defences in form of drop-in replacements, stored in `metarouter.js` file.

## XSS Attacks

> Protects against exploits Alpha and Foxtrot

The XSS-only defences I have used include CSP and HTML special symbols escaping.
First of all, since at least some HTML tags should be supported in user profile (and maybe even in username, why not), all the potentially dangerous tags should be disabled with CSP headers.
All the sources have been limited to `'self'` scope, only styles from `fonts.googleapis.com` and fonts from `fonts.gstatic.com` were allowed.

Since there is one inline script in `views/pages/projile/view.ejs` that can not be removed.
In order to allow it, a `'nonce'` is automatically generated on every request and included both into CSP header and HTML being rendered.
The script itself was also changed, two `eval` calls were removed from it: one explicit was replaced with `parseInt` and another implicit inside `setTimeout` was replaced with passing the function and the argument separatly.

It is also worth mentioning that even without this protection [CSRF protection](#csrf-attacks) and [Cookie protection](#cookie-tampering) makes malicious script execution _a lot_ harder.

Another potential vector of attack would be malicious HTML insertion into error message views.
In the same function, all the text meant to appear as an error message is being escaped.
You can see the replacement table below (the characters are being replaced with their HTML-safe representation):

| Original character | Replacement character |
| :---: | :---: |
| `&` | `&amp;` |
| `<` | `&lt;` |
| `>` | `&gt;` |
| `"` | `&quot;` |
| `'` | `&#039;` |

### Online resources used

- CSP [guide](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Security-Policy)
- CSP nonce [guide](https://content-security-policy.com/nonce/)
- HTML excaping [code snippet](https://stackoverflow.com/a/6234804/9124072)

## CSRF Attacks

> Protects against exploit Beta (also affecting Alpha, Foxtrot and Gamma)

For CSRF attack protection, a so-called "double submit" defence is used, meaning the CSRF token is not stored on server, instead it is included into user session cookie.
Then again, just as it happens for inline script `nonce`, it is passed to rendering function and is included into all the forms as a hidden input with a default value.
This imput typically looks like this:

```html
<input type=hidden name=_csrf value=<%= CSRF %> >
```

After every form submission (with either GET or POST request) the value of this value is compared to the CSRF token stored in session cookie.
In case validation is not successful (or the CSRF input is not find among request parameters), an error is thrown.
If the validation is successful, the session CSRF token _is regenerated_, meaning one token is valid for one form submission only.

This algorithm, combined with [Cookie protection](#cookie-tampering), makes it tricky to perform any form submissions on users behalf.
The only way to do that I can imagine is requesting any form HTML, having user session cookie stolen, parsing HTML searching for that hidden CSRF inputs and retrieving their value.
Unfortunately, this can only be fixed with a stricter cookie policy (and we are not allowed to change that).

### Online resources used

- CSRF vulnerabilities [overview](https://portswigger.net/web-security/csrf/bypassing-token-validation)
- CSRF token implementation [example](https://cheatsheetseries.owasp.org/cheatsheets/Cross-Site_Request_Forgery_Prevention_Cheat_Sheet.html#transmissing-csrf-tokens-in-synchronized-patterns)

### Cookie Tampering

> Protects against exploits Charlie and Delta (also affecting Alpha, Beta, Foxtrot and Gamma)

According to the assignment specification, we were not allowed to change any cookie policies.
I have also decided not to change session cookie fields, because it would involve rewriting too much of `router.js` code.
However, I could not help noticing, that no parts of the session cookie were used _on the client side_ (only in server code and during HTML generation).
That is why I decided to just encrypt session cookie.

Decryption is performed just before all the server code (as a middleware), encryption is performed just before `res.render` call, that returns HTTP response to the user.
A static `AES` cipher (from `sjcl` library) in `GCM` mode is used for that.
After all the session cookie available to the user only contains one field, called `secret` and containing the encrypted cookie data.

The key for the static cipher should be 32 bytes long, and it can be automatically generated upon server startup (meaning all the user sessions will become invalid after server restart).
In order to preserve user sessions, the key can be taken from an environment variable (see updated `Dockerfile`).

Session initialization was also removed from `app.js` (the only change that was made there, I promise) and placed into encryption middleware.
That means even anonymous sessions (with `loggedIn` field set to `false`) are encrypted leading to the login form using CSRF token protection as well.

### Online resources used

- `cookie-session` library [sources](https://github.com/expressjs/cookie-session/blob/master/index.js)
- An `AES+GCM` with `sjcl` library [example](https://runkit.com/harshbhanot24/5e42710ba806fa001ad38920)

## SQL Injection

> Protects against exploit Echo

That one is relatively easy, however also including lots of changes to the `router.js`.
The SQL injection protection I have used was just replacing sll the templates in SQL queries with `?` and passing arguments to the javascript `sqlite` library calls.
The library automatically treats all the arguments as unsafe, sanitizing them if necessary.

The utility function for SQL calls I have come up to (`runSQL` in `metarouter.js`) acts just like `get` function from `sqlite` library, returning the first result for all the `SELECT` queries or `undefined` otherwise.

### Online resources used

- `sqlite` library [documentation](https://nodejs.org/api/sqlite.html)

## Side-Channel Timing Attack

> Protects against exploit Gamma

Just like in the part 1, it was not really clear, what should I protect against in this case.
Since all the hidden script execution and image source GET calss were already covered by [XSS protection](#xss-attacks) and hidden form submission were (in a way) covered by [CSRF protection](#csrf-attacks), the exploit gamma already does not work and the only thing left to think about here are side channel attacks.
For simplicity, let us consider time-related side channel attacks, in particular the situation when _correct form submissions take much more time then errorous ones_, helping an attacker to gain some knowledge about the correct form values.

In case we _do know_ what part of code causes the delay, we can use constant time functions that return the result (no matter correct or wrong) in the same time.
For example, the password checking function can be replaced with such a function: vanilla password string comparison is unsafe, since it exits early on the first character that does not match, so my constant time function will iterate all the stored password hash string unconditionally and only return after the whole loop is executed.
However, honestly, I doubt it can be _really_ useful in case of WEB technologies, because string vomparison time is so short and can easily be less then for example HTTP respons delivery delay.

The other case I would like to consider is the case when we _do not know how_ (or do not want) to perform all the operations in constant time.
In that case we can use a constant time middleware, (for now it is active for `/get_login` route only) that stores the execution start time in the session cookie.
Then, right before page rendering, the function execution time is calculated and compared to the successful execution time average (in the beginning it is equal to some big number and updated on every successful call).
If the function is about to return an error (meaning that it might be exiting early) and the execution time is less then average, the middleware just sleeps for the time delta.
As a result, both successful and unsuccessful function executions take almost the same time (on average).

### Online resources used

- The fact that [this method](https://nodejs.org/dist/v6.17.1/docs/api/crypto.html#crypto_crypto_timingsafeequal_a_b) is present in modern NodeJS
