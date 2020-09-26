function populate_lib(filter) {
    get_books(filter, (books) => {
        const shelves = format_books(books);
        const librarian = document.getElementById("librarian");
        librarian.innerHTML = "";

        if (shelves.length > 0) for (const shelf of shelves) create_book_row(shelf, librarian);
        else {
            const empty = document.createElement('h1');
            empty.className = "text-center align-self-center";
            librarian.appendChild(empty);
        }
    });
}

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
    document.getElementById("logout").onclick = function () {
        logout(() => {
            window.location = "/login";
        })
    }

    if (document.getElementById("add")) {
        document.getElementById("push").onclick = function () {
            push_books(() => {
                alert("Состояние библиотеки успешно сохранено!")
            });
        }
        document.getElementById("pull").onclick = function () {
            pull_books(() => {
                alert("Состояние библиотеки успешно загружено!")
            });
        }

        configure_book_modal_block(false);
    }

    const filter_none = document.getElementById("filter-none");
    filter_none.onchange = function () {
        if (filter_none.checked) populate_lib("none");
    }
    const filter_available = document.getElementById("filter-available");
    filter_available.onchange = function () {
        if (filter_available.checked) populate_lib("available");
    }
    const filter_return_date = document.getElementById("filter-return-date");
    filter_return_date.onchange = function () {
        if (filter_return_date.checked) populate_lib("return-date");
    }

    // It is saved in some cases, not sure.
    if (filter_none.checked) populate_lib("none");
    if (filter_available.checked) populate_lib("available");
    if (filter_return_date.checked) populate_lib("return-date");



    configure_user_modal_block(true);
}