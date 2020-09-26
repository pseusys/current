function getParams(url) {
    const params = {};
    const parser = document.createElement('a');
    parser.href = url;
    const query = parser.search.substring(1);
    const vars = query.split('&');
    for (let i = 0; i < vars.length; i++) {
        const pair = vars[i].split('=');
        params[pair[0]] = decodeURIComponent(pair[1]);
    }
    return params;
}

window.onload = function () {
    const code = getParams(window.location.href).code;
    if (document.getElementById("edit")) configure_book_modal_block(true, code);

    document.getElementById("back").onclick = function () {
        window.location = "/";
    }

    get_user((user) => {
        const button_return = document.getElementById("return");
        if (button_return) button_return.onclick = function () {
            return_book(code, (date) => {
                alert("Книга возвращена!");
                window.location = window.location.href;
            });
        }
        const button_take = document.getElementById("take");
        if (button_take) button_take.onclick = function () {
            take_book(code, (date) => {
                alert("Книга взята! Дата возврата " + new Date(date).toLocaleDateString("ru"));
                window.location = window.location.href;
            });
        }
        const button_delete = document.getElementById("delete");
        if (button_delete) button_delete.onclick = function () {
            remove_book(code, () => {
                window.location = "/";
            });
        }
    });
}