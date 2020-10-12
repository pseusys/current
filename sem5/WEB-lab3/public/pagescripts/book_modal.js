function save_book_data() {
    const necessary_fields = $("input[type=text], input[type=date]");
    let valid = true;
    necessary_fields.map(function () {
        $(this).toggleClass("is-invalid", this.value === "");
        valid &= (this.value !== "");
        return this;
    });

    if (valid) return assemble_book(
        $("#book_name_input").val(),
        $("#book_author_input").val(),
        $("#book_date_input").val(),
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
