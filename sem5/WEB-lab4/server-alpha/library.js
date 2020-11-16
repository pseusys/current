// @flow

const fs = require('fs');
const path = require('path');

const coverage = require('./coverage');



function get_all_books () {
    const library = fs.readFileSync(path.join(__dirname, "../storage-alpha/lib.json"));
    const current_state = JSON.parse(library.toString());
    current_state.sort((a, b) => {return a.code - b.code});
    return current_state;
}

function set_all_books (state) {
    const lib = JSON.stringify(state);
    fs.writeFileSync(path.join(__dirname, "../storage-alpha/lib.json"), lib);
}



module.exports.get_book = function (book_code: number): boolean {
    for (const book of get_all_books()) if (book.code === book_code) return book;
    return false;
}

module.exports.set_book = function (book: { code: number }): boolean {
    const storage = get_all_books();

    let previous = 0, new_code = -1;
    for (let i = 0; i < storage.length; i++) {
        if (storage[i].code - previous > 1) {
            new_code = previous + 1;
            break;
        } else previous = storage[i].code;
    }
    if (new_code === -1) new_code = storage.length;
    book.code = new_code;

    storage.push(book);
    set_all_books(storage);
    return true;
}

module.exports.edit_book = function (book: { cover: string, code: number }): boolean {
    const storage = get_all_books();
    for (let i = 0; i < storage.length; i++) if (storage[i].code === book.code) {
        if (storage[i].cover !== book.cover) coverage.remove_cover(storage[i].cover);
        storage[i] = book;
        set_all_books(storage);
        return true;
    }
    return false;
}

module.exports.delete_book = function (book_code: number): boolean {
    let storage = get_all_books();
    for (let i = 0; i < storage.length; i++) if (storage[i].code === book_code) {
        coverage.remove_cover(storage[i].cover);
        storage.splice(i, 1);
        set_all_books(storage);
        return true;
    }
    return false;
}



/** Accepts:
 * @filter none = return all books
 * @filter return-date = sort by return date, earlier first
 * @filter available = available books only
 */
module.exports.get_books_sorted = function (condition: string = "all"):
    Array<{ code: number, author: string, cover: string, name: string, owner: {} | boolean, part: { start_cost: number, min_step: number, max_step: number } | boolean }> {

    const all_books = get_all_books();
    const sorted_books = [];
    switch (condition) {
        case "all":
            for (const book of all_books) sorted_books.push(book);
            break;
        case "on-sale":
            for (const book of all_books) if (book["part"]) sorted_books.push(book);
            break;
        case "sold":
            for (const book of all_books) if (book["owner"]) sorted_books.push(book);
            break;
    }
    return sorted_books;
}



module.exports.give_book = function (code: number, part: {}): boolean {
    let storage = get_all_books();
    for (const book of storage) if (book.code === code) {
        if (!book.owner) book.part = part;
        set_all_books(storage);
        return true;
    }
    return false;
}

module.exports.return_book = function (code: number): boolean {
    let storage = get_all_books();
    for (const book of storage) if (book.code === code) {
        book.part = false;
        set_all_books(storage);
        return true;
    }
    return false;
}

module.exports.sell_book = function (code: number): boolean {
    let storage = get_all_books();
    for (const book of storage) if (book.code === code) {
        book.owner = false;
        set_all_books(storage);
        return true;
    }
    return false;
}
