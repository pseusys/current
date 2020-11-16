import {part_user} from "../scripts/accounter"

require("../styles/custom.less")

$("input[type=checkbox]").change(function () {
    part_user($(this).prop("checked"), $(this).attr("id"));
});

window.on_back = function () {
    window.location.href = "/";
}
