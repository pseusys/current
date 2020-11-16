import {money_user} from "../scripts/accounter"
import {sell_book, return_book, remove_book} from "../scripts/book_manager"
import {get_params} from "../scripts/general"

import "./book_modal"
import "./take_modal"

require("../styles/custom.less")



window.on_back = function () {
    window.location = "/";
}

window.on_sell = function (price, money) {
    money_user(Number.parseInt(price) + Number.parseInt(money));
    sell_book(get_params("code"), () => {
        window.location = window.location.href;
    });
}

window.on_return = function () {
    return_book(get_params("code"), () => {
        window.location = window.location.href;
    });
}

window.on_delete = function () {
    remove_book(get_params("code"), () => {
        window.location = "/";
    });
}
