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



function assemble_book(name, author, publication, description) {
    const actual_description = description ? description : "[Нет описания]";
    let actual_file = "./covers/sample_cover.jpg";

    return {
        name: name,
        cover: actual_file,
        code: undefined,
        author: author,
        publication: publication,
        description: actual_description,
        taken: false
    };
}
