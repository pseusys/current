import {get_params} from "../scripts/general"
import {logout} from "../scripts/accounter"

import "./book_modal"
import "./user_modal"
import "./module"

require("../styles/custom.less");



$(document).ready(function () {
    const filter = get_params("filter");
    if (filter) $("#" + filter).prop("checked", true);
});

$("input[type=radio]").change(function () {
    window.location = "?filter=" + $(this).attr("id");
});



window.on_open = function (code) {
    window.location = "/book?code=" + code;
}

window.on_logout = function () {
    logout(() => {
        window.location = "/login";
    })
}

window.on_users = function () {
    window.location = "/users";
}

window.on_settings = function () {
    window.location = "/settings";
}
