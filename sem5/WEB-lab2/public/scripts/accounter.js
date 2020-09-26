function login(username, password, onsuccess = undefined, onerror = undefined) {
    auth(username, password, "/login", onsuccess, onerror);
}

function signin(username, password, onsuccess = undefined, onerror = undefined) {
    auth(username, password, "/user", onsuccess, onerror);
}

function auth(username, password, url, onsuccess, onerror) {
    const http = new XMLHttpRequest();
    const params = "username=" + username + "&password=" + password;
    http.open("POST", url, true);
    http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    http.onreadystatechange = function () {
        if (http.readyState === 4) {
            if (http.status === 200) {
                if (onsuccess) onsuccess();
            } else if (onerror) onerror(http.responseText);
        }
    }
    http.send(params);
}



function logout(onsuccess = undefined, onerror = undefined) {
    deauth("/login", onsuccess, onerror);
}

function signout(onsuccess = undefined, onerror = undefined) {
    deauth("/user", onsuccess, onerror);
}

function deauth(url, onsuccess, onerror) {
    const http = new XMLHttpRequest();
    http.open("DELETE", url);
    http.onreadystatechange = function () {
        if (http.readyState === 4) {
            if (http.status === 200) {
                if (onsuccess) onsuccess();
            } else if (onerror) onerror(http.responseText);
        }
    }
    http.send();
}



function get_user(onsuccess, onerror) {
    const http = new XMLHttpRequest();
    http.onreadystatechange = function () {
        if (http.readyState === 4) {
            if (http.status === 200) {
                if (onsuccess) onsuccess(JSON.parse(this.responseText));
            } else if (onerror) onerror(http.responseText);
        }
    }
    http.open("GET", "/user");
    http.send();
}



function return_book(old_book, onsuccess = undefined, onerror = undefined) {
    update_book(old_book, "give-book", onsuccess, onerror);
}

function take_book(new_book, onsuccess = undefined, onerror = undefined) {
    update_book(new_book, "take-book", onsuccess, onerror);
}

function update_book(new_book_code, parameter, onsuccess, onerror) {
    const request = new XMLHttpRequest();
    const params = parameter + "=" + new_book_code;
    request.open("PUT", "/user");
    request.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    request.onreadystatechange = function () {
        if (request.readyState === 4) {
            if (request.status === 200) {
                if (onsuccess) onsuccess(this.responseText ? JSON.parse(this.responseText) : "");
            } else if (onerror) onerror(http.responseText);
        }
    }
    request.send(params);
}
