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



function assemble_book(name, author, publication, description, cover, after) {
    let book = {
        name: name,
        cover: "./covers/default.jpg",
        code: undefined,
        author: author,
        publication: publication,
        description: description ? description : "[Нет описания]",
        taken: false
    };

    if (cover) {
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
    } else after (book);
}
