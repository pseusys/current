$("input[type=checkbox]").change(function () {
    part_user($(this).prop("checked"), $(this).attr("id"));
});

function on_back () {
    window.location.href = "/";
}
