function on_back() {
    window.location = "/";
}

function on_return() {
    return_book(get_params("code"), () => {
        alert("Книга возвращена!");
        window.location = window.location.href;
    });
}

function on_take() {
    take_book(get_params("code"), (date) => {
        alert("Книга взята! Дата возврата " + new Date(date).toLocaleDateString("ru"));
        window.location = window.location.href;
    });
}

function on_delete() {
    remove_book(get_params("code"), () => {
        window.location = "/";
    });
}
