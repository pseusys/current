function on_back() {
    window.location = "/";
}

function on_return() {
    return_book(get_params("code"), () => {
        window.location = window.location.href;
    });
}

function on_delete() {
    remove_book(get_params("code"), () => {
        window.location = "/";
    });
}
