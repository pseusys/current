function save_book_data(after) {
    const params = collect_values_if_possible("book_name_input", "book_author_input", "book_date_input");

    if (params) assemble_book(
        params["book_name_input"],
        params["book_author_input"],
        params["book_date_input"],
        $("#book_description_input").val(),
        $("#book_cover_input").prop("files")[0],
        after
    );
    else console.log("Params not collected!");
}

function on_modal_book_save(code = undefined) {
    const fill = !isNaN(code);
    save_book_data(function (book) {
        if (book) {
            if (fill) book.code = code;
            const callback = () => {
                window.location = window.location.href;
            };
            if (fill) edit_book(book, callback);
            else add_book(book, callback);
        }
    });
}
