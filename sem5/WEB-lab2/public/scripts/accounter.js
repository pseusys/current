function login(username, password, onsuccess = undefined, onerror = undefined) {
    const http = new XMLHttpRequest();
    const url = "/login";
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
    const http = new XMLHttpRequest();
    const url = "/logout";
    http.open("GET", url);
    http.onreadystatechange = function () {
        if (http.readyState === 4) {
            if (http.status === 200) {
                if (onsuccess) onsuccess();
            } else if (onerror) onerror(http.responseText);
        }
    }
    http.send();
}
