const assert = require('assert');

const library = require('../src/library');

describe("Book test", () => {
    const book_data = {
        name: "test_book",
        cover: "default.jpg",
        author: "book_author",
        publication: "11.23.1980",
        description: "This is a test book",
        part: false,
        owner: false
    };

    it("Should add test book to the lib", () => {
        const set_book = library.set_book(book_data);
        assert.strictEqual(set_book, true);
    });

    let book_code = -1;
    it("Book should exist in the lib", () => {
        const books = library.get_books_sorted();
        for (const book of books) {
            let all_match = true;
            for (const key in book) {
                all_match &= book_data[key] === book[key];
            }
            if (all_match) book_code = book.code;
        }
        assert.notStrictEqual(book_code, -1);
    });

    it("Should edit book in the lib", () => {
        const book = library.get_book(book_code);
        assert.notStrictEqual(book, false);

        book.author = "best_author";
        book.name = "best_book";
        const edit_book = library.edit_book(book);
        assert.strictEqual(edit_book, true);

        const new_book = library.get_book(book_code);
        assert.notStrictEqual(book, false);
        assert.strictEqual(new_book.author, "best_author");
        assert.strictEqual(new_book.name, "best_book");
    });

    it("Should set book to trades", () => {
        const trade_info = {
            start_cost: 100,
            min_step: 10,
            max_step: 1000
        };
        const set_book = library.give_book(book_code, trade_info);
        assert.strictEqual(set_book, true);

        const edited_book = library.get_book(book_code);
        assert.notStrictEqual(edited_book, false);
        assert.deepStrictEqual(edited_book.part, trade_info);
    });

    it("Should return book from trades", () => {
        const remove_book = library.return_book(book_code);
        assert.strictEqual(remove_book, true);

        const unedited_book = library.get_book(book_code);
        assert.notStrictEqual(unedited_book, false);
        assert.strictEqual(unedited_book.part, false);
    });

    it("Should delete book from the lib", () => {
        const delete_book = library.delete_book(book_code);
        assert.strictEqual(delete_book, true);

        const book = library.get_book(book_code);
        assert.strictEqual(book, false);
    });
});
