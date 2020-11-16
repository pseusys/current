import {collect_values_if_possible} from "../scripts/general"
import {add_book, edit_book, assemble_participation, assemble_book} from "../scripts/book_manager"



window.toggle_part = function () {
    $("#part_container :input").prop("disabled", !$("#part_input").prop("checked"));
}

function save_book_data(after) {
    const params = collect_values_if_possible("book_name", "book_author", "book_date");
    let part_params = false;
    if ($("#part_input").prop("checked")) {
        const part = collect_values_if_possible("min_step", "max_step", "start_cost");

        part_params = assemble_participation(part.min_step, part.max_step, part.start_cost);
        if (!part_params) {
            alert("Min step should be less than max!");
            return;
        }
    }

    const files = $("#book_cover");
    if (params) assemble_book(
        params["book_name"],
        params["book_author"],
        params["book_date"],
        $("#book_description").val(),
        (files.prop("files") && files.prop("files").length > 0) ? files.prop("files")[0] : files.attr("val"),
        part_params,
        after
    );
    else console.log("Params not collected!");
}

window.on_modal_book_save = function (code = undefined) {
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
