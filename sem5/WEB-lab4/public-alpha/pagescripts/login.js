function on_login () {
    const creditionals = collect_values_if_possible("username", "password");
    if (creditionals) login(creditionals.username, creditionals.password,
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}

function on_signin () {
    const creditionals = collect_values_if_possible("username", "password");
    if (creditionals) signin(creditionals.username, creditionals.password,
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}
