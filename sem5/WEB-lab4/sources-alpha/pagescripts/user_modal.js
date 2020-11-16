import {signout, money_user} from "../scripts/accounter"

window.on_modal_delete_user = function () {
    signout(() => {
        window.location = "/login";
    });
}

window.on_money = function () {
    money_user($("#money_input").val());
}
