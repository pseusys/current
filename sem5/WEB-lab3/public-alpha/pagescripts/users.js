$("input[type=checkbox]").change(function () {
    part_user($(this).attr("id"), $(this).prop("checked"));
});

function on_back () {
    window.location.href = "/";
}
