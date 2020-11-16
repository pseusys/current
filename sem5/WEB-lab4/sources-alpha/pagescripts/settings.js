import {set_settings} from "../scripts/setter"

require("../styles/custom.less")

window.on_back = function () {
    window.location.href = "/";
}

window.on_commit = function () {
    let settings = Object();
    $(".collectable").map(function () {
        settings[$(this).attr("id")] = $(this).val();
        return this;
    });
    set_settings(settings, function () {
        window.location.href = "/settings";
    });
}
