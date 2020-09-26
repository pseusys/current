function save_book_data() {
    const book_name_input = document.getElementById("book_name_input");
    const book_author_input = document.getElementById("book_author_input");
    const book_date_input = document.getElementById("book_date_input");
    book_name_input.className = "form-control";
    book_author_input.className = "form-control";
    book_date_input.className = "form-control";

    if (!book_name_input.value) {
        book_name_input.className += " is-invalid";
        return;
    }
    if (!book_author_input.value) {
        book_author_input.className += " is-invalid";
        console.log(book_author_input.className);
        return;
    }
    if (!book_date_input.value) {
        book_date_input.className += " is-invalid";
        return;
    }

    return assemble_book(book_name_input.value, book_author_input.value, book_date_input.value,
        document.getElementById("book_description_input").value);
}

function fill_book_modal_block(book) {
    document.getElementById("book_name_input").value = book.name;
    document.getElementById("book_author_input").value = book["author"];
    document.getElementById("book_date_input").value = book.publication;
    document.getElementById("book_description_input").value = book.description;
}

function configure_book_modal_block(fill, code = undefined) {
    if (fill) get_book(code, (book) => {
        fill_book_modal_block(book);
    });

    document.getElementById("modal_book_save").onclick = function () {
        const book = save_book_data();
        if (fill) book.code = code;
        const callback = () => {
            window.location = window.location.href;
        };
        if (fill) edit_book(book, callback);
        else add_book(book, callback);
    }
}