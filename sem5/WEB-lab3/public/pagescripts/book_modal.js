function save_book_data() {
    const params = collect_values_if_possible("book_name_input", "book_author_input", "book_date_input");

    return assemble_book(
        params["book_name_input"],
        params["book_author_input"],
        params["book_date_input"],
        $("#book_description_input").val()
    ); //TODO: resolve path to pic.
}

function on_modal_book_save(code = undefined) {
    const fill = !isNaN(code);
    const book = save_book_data();
    if (book) {
        if (fill) book.code = code;
        const callback = () => {
            window.location = window.location.href;
        };
        if (fill) edit_book(book, callback);
        else add_book(book, callback);
    }
}
