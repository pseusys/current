function format_books(books) {
    let shelves = [];
    for (let i = 0; i < books.length; i++) {
        let index = Math.floor(i / 12);
        if (!shelves[index]) shelves[index] = [];
        shelves[index].push(books[i]);
    }
    return shelves;
}

function create_book_row(shelf, librarian) {
    const new_row = document.createElement('div');
    new_row.className = "row";
    for (const book of shelf) create_book(book, new_row);
    librarian.appendChild(new_row);
}

function create_book(book, row) {
    const holder = document.createElement('div');
    holder.className = "col-md-2";

    const cover = document.createElement('img');
    cover.className = "img-thumbnail rounded mb-2";
    cover.src = book["cover"];
    const open = document.createElement('button');
    open.type = "button";
    open.className = "btn btn-sm btn-block texteous " + (book["taken"] ? "btn-danger" : "btn-success");
    open.innerText = book["name"];
    open.onclick = function() {window.location = "/book?code=" + book["code"];};

    holder.appendChild(cover);
    holder.appendChild(open);
    row.appendChild(holder);
}



window.onload = function () {
    const request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        if ((this.readyState === 4) && (this.status === 200)) {
            const shelves = format_books(JSON.parse(this.responseText));
            const librarian = document.getElementById("librarian");
            for (const shelf of shelves) create_book_row(shelf, librarian);
        }
    }
    request.open("GET", "/api?method=sort&filter=return-date");
    request.send();

    document.getElementById("logout").onclick = function () {
        logout(() => {window.location = "/login";})
    }
}