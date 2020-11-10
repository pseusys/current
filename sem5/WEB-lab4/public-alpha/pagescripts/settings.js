function on_back () {
    window.location.href = "/";
}

function on_commit() {
    let settings = Object();
    $(".collectable").map(function () {
        settings[$(this).attr("id")] = $(this).val();
        return this;
    });
    set_settings(settings, function () {
        window.location.href = "/settings";
    });
}
