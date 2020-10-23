function add_book(book, onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/books", type: "PUT", data: "book=" + JSON.stringify(book)})
        .done(function(result) { if (onsuccess) onsuccess(result ? JSON.parse(result) : "") })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function edit_book(book, onsuccess = undefined, onerror = undefined) {
    $.post("/books", {book: JSON.stringify(book)})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function remove_book(code, onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/books", type: "DELETE", data: "code=" + code})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function return_book(old_book, onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/part", type: "PUT", data: "give-book=" + old_book})
        .done(function(result) { if (onsuccess) onsuccess(result ? JSON.parse(result) : "") })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function take_book(new_book, params, onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/part", type: "PUT", data: "take-book=" + new_book + "&params=" + JSON.stringify(params)})
        .done(function(result) { if (onsuccess) onsuccess(result ? JSON.parse(result) : "") })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}



function assemble_participation(min_step, max_step, start_cost) {
    min_step = Number.parseInt(min_step);
    max_step = Number.parseInt(max_step);
    if (min_step < max_step) {
        return {
            min_step: min_step,
            max_step: max_step,
            start_cost: start_cost
        };
    } else return false;
}

function assemble_book(name, author, publication, description, cover, part, after) {
    let book = {
        name: name,
        cover: "./covers/default.jpg",
        code: undefined,
        author: author,
        publication: publication,
        description: description ? description : "[Нет описания]",
        part: part
    };

    if ((cover) && (typeof cover !== "string")) {
        const formData = new FormData();
        formData.append("image", cover);

        $.ajax({
            url: "/cover",
            type: "POST",
            data: formData,
            processData: false,
            contentType: false
        }).done(function (result) {
            book.cover = result;
            after(book);
        }).fail(function () {
            console.log("Cover can not be uploaded!");
            after(book);
        });
    } else if ((cover) && (typeof cover === "string")) {
        book.cover = cover;
        after(book);
    } else {
        after (book);
    }
}
