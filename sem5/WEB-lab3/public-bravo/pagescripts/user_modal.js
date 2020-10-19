function on_modal_delete_user() {
    signout(() => {
        window.location = "/login";
    });
}

function on_money() {
    money_user($("#money_input").val());
}
