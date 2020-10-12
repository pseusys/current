function on_login () {
    login($("#username").val(), $("#password").val(),
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}

function on_signin () {
    signin($("#username").val(), $("#password").val(),
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}
