function on_modal_delete_user() {
    signout(() => {
        window.location = "/login";
    });
}
