function on_modal_delete_user() {
    signout(() => {
        window.location = "/login";
    });
}

function on_money() {
    part_user($("#money_input").val());
}
