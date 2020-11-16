import {collect_values_if_possible} from "../scripts/general"
import {login, signin} from "../scripts/accounter"

require("../styles/custom.less")

window.on_login = function () {
    const creditionals = collect_values_if_possible("username", "password");
    if (creditionals) login(creditionals.username, creditionals.password,
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}

window.on_signin = function () {
    const creditionals = collect_values_if_possible("username", "password");
    if (creditionals) signin(creditionals.username, creditionals.password,
        () => {window.location = "/";},
        (error) => {alert(error);}
    );
}
