function get_books(filter, onsuccess = undefined, onerror = undefined) {
    const request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        if (request.readyState === 4) {
            if (request.status === 200) {
                if (onsuccess) onsuccess(JSON.parse(this.responseText));
            } else if (onerror) onerror(request.responseText);
        }
    }
    request.open("GET", "/books?method=books&filter=" + filter);
    request.send();
}

function push_books(onsuccess = undefined, onerror = undefined) {
    manage_lib("POST", onsuccess, onerror);
}

function pull_books(onsuccess = undefined, onerror = undefined) {
    manage_lib("GET", onsuccess, onerror);
}

function manage_lib(method, onsuccess = undefined, onerror = undefined) {
    const request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        if (request.readyState === 4) {
            if (request.status === 200) {
                if (onsuccess) onsuccess();
            } else if (onerror) onerror(request.responseText);
        }
    }
    request.open(method, "/lib");
    request.send();
}



function get_book(code, onsuccess = undefined, onerror = undefined) {
    const request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        if (request.readyState === 4) {
            if (request.status === 200) {
                if (onsuccess) onsuccess(JSON.parse(this.responseText));
            } else if (onerror) onerror(request.responseText);
        }
    }
    request.open("GET", "/books?method=book&code=" + code);
    request.send();
}

function commit_book(book, method, onsuccess = undefined, onerror = undefined) {
    const request = new XMLHttpRequest();
    request.open(method, "/books");
    request.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    request.onreadystatechange = function () {
        if (request.readyState === 4) {
            if (request.status === 200) {
                if (onsuccess) onsuccess();
            } else if (onerror) onerror(request.responseText);
        }
    }
    request.send("book=" + JSON.stringify(book));
}

function add_book(book, onsuccess = undefined, onerror = undefined) {
    commit_book(book, "PUT", onsuccess, onerror);
}

function edit_book(book, onsuccess = undefined, onerror = undefined) {
    commit_book(book, "POST", onsuccess, onerror);
}

function remove_book(code, onsuccess = undefined, onerror = undefined) {
    const request = new XMLHttpRequest();
    request.open("DELETE", "/books");
    request.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    request.onreadystatechange = function () {
        if (request.readyState === 4) {
            if (request.status === 200) {
                if (onsuccess) onsuccess();
            } else if (onerror) onerror(request.responseText);
        }
    }
    request.send("code=" + code);
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
