function fill_user_modal_block(books) {
    const container = document.getElementById("modal_book_container");
    for (const code of books) {
        get_book(code, (book) => {
            const row = document.createElement('div');
            row.className = "row justify-content-between";

            const link = document.createElement('a');
            link.href = "/book?code=" + book.code;
            link.innerText = book.name;
            const date = document.createElement('p');
            date.innerText = (new Date(book["taken"]["return"])).toLocaleDateString("ru")

            row.appendChild(link);
            row.appendChild(date);
            container.appendChild(row);
        });
    }
}

function configure_user_modal_block(fill) {
    get_user((user) => {
        document.getElementById("modal_username").innerText = user.name;
        if (fill) fill_user_modal_block(user.books);
    });

    document.getElementById("modal_delete_user").onclick = function () {
        signout(() => {
            window.location = "/login";
        });
    }
}
