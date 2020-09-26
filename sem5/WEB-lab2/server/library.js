const fs = require('fs');
const path = require('path');
let current_lib_state = undefined;



function get_all_books() {
    if (current_lib_state !== undefined) return current_lib_state;
    else {
        const library = fs.readFileSync(path.join(__dirname, "../storage/lib.json"));
        current_lib_state = JSON.parse(library.toString());
        current_lib_state.sort((a, b) => {return a.code - b.code});
        return current_lib_state;
    }
}



module.exports.get_book = function (book_code) {
    for (const book of get_all_books()) if (book.code === book_code) return book;
    return false;
}

module.exports.set_book = function (book) {
    const library = get_all_books();

    let previous = 0, new_code;
    for (let i = 0; i < library.length; i++) {
        if (library[i].code - previous > 1) {
            new_code = previous + 1;
            break;
        } else previous = library[i].code;
    }
    if (previous === library.length - 1) new_code = library.length;
    book.code = new_code;

    get_all_books().push(book);
}

module.exports.edit_book = function (book) {
    const library = get_all_books();
    for (let i = 0; i < library.length; i++) if (library[i].code === book.code) {
        library[i] = book;
        return true;
    }
    return false;
}

module.exports.delete_book = function (book_code) {
    let library = get_all_books();
    for (let i = 0; i < library.length; i++) if (library[i].code === book_code) {
        library = library.splice(i, 1);
        return true;
    }
    return false;
}



/** Accepts:
 * @filter none = return all books
 * @filter return-date = sort by return date, earlier first
 * @filter available = available books only
 */
module.exports.get_books_sorted = function (condition) {
    const all_books = get_all_books();
    const sorted_books = [];
    switch (condition) {
        case "none":
            for (const book of all_books) sorted_books.push(book);
            break;
        case "return-date":
            for (const book of all_books) if (book["taken"]) sorted_books.push(book);
            sorted_books.sort((a, b) => {
                return (new Date(a["taken"]["return"])) - (new Date(b["taken"]["return"]));
            });
            break;
        case "available":
            for (const book of all_books) if (!book["taken"]) sorted_books.push(book);
            break;
    }
    return sorted_books;
}



module.exports.back_up_lib = function () {
    get_all_books();
    const lib = JSON.stringify(current_lib_state);
    fs.writeFileSync(path.join(__dirname, "../storage/lib.json"), lib);
}

module.exports.restore_lib = function () {
    const library = fs.readFileSync(path.join(__dirname, "../storage/lib.json"));
    current_lib_state = JSON.parse(library.toString());
}



module.exports.give_book = function (code, reader) {
    const return_date = new Date();
    return_date.setMonth(return_date.getMonth() + 1);
    for (const book of get_all_books()) if (book.code === code) {
        book.taken = {
            reader: reader,
            taken: new Date,
            return: return_date
        };
        return return_date;
    }
    return false;
}

module.exports.return_book = function (code) {
    for (const book of get_all_books()) if (book.code === code) {
        book.taken = false;
        return true;
    }
    return false;
}
