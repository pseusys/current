$(document).ready(function () {
    const filter = get_params("filter");
    if (filter) $("#" + filter).prop("checked", true);
});

$("input[type=radio]").change(function () {
    window.location = "?filter=" + $(this).attr("id");
});



function on_open(code) {
    window.location = "/book?code=" + code;
}

function on_logout() {
    logout(() => {
        window.location = "/login";
    })
}

function on_users() {
    window.location = "/users";
}

function on_settings() {
    window.location = "/settings";
}
