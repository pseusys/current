function on_login () {
    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;
    login(username, password,
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}

function on_signin () {
    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;
    signin(username, password,
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}
