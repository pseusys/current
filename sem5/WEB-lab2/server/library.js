const fs = require('fs');
const path = require('path');



get_all_books = function () {
    const library = fs.readFileSync(path.join(__dirname, "../storage/lib.json"));
    return JSON.parse(library.toString());
}

module.exports.get_book = function (book_code) {
    const library = fs.readFileSync(path.join(__dirname, "../storage/lib.json"));
    for (const book of JSON.parse(library.toString())) if (book.code === book_code) return book;
    return undefined;
}

const filters = ["all", "return-date", "available"];
module.exports.get_books_sorted = function (condition) {
    const all_books = get_all_books();
    const sorted_books = [];
    switch (condition) {
        case filters[0]:
            for (const book of all_books) sorted_books.push(book);
            break;
        case filters[1]:
            for (const book of all_books) if (book.code < 1000) sorted_books.push(book);
            break;
        case filters[2]:
            for (const book of all_books) sorted_books.push(book);
            sorted_books.sort((a, b) => {});
            break;
    }
    return sorted_books;
}



module.exports.back_up_lib = function (library) {

}

module.exports.restore_lib = function () {

}
