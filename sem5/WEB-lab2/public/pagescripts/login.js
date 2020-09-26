window.onload = function () {
    document.getElementById("login").onclick = function () {
        const username = document.getElementById("username").value;
        const password = document.getElementById("password").value;
        login(username, password,
            () => {window.location = "/";},
            (error) => {alert(error);}
        );
    }

    document.getElementById("signin").onclick = function () {
        const username = document.getElementById("username").value;
        const password = document.getElementById("password").value;
        signin(username, password,
            () => {window.location = "/";},
            (error) => {alert(error);}
        );
    }
}