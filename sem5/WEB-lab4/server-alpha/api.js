// @flow

const https = require('https');
const formidable = require('formidable');

const coverage = require('./coverage');
const library = require('./library');
const settings = require('./settings');

module.exports.configure = function (server: https.Server): null {
    /** Accepts:
     * @param method = one of the following:
     *      book (expects 'code' parameter) = get book with following code
     *      books (expects 'filter' parameter) = get filtered books
     * @param code = code of the book
     *    OR
     * @param filter = filtering condition, one of the following:
     *      none = all books
     *      return-date = by date of return
     *      available = only available ones
     */
    server.get("/books", (req, res) => {
        const method = req.query["method"];
        if (!method) return res.status(404).end("Неверный запрос к API.");

        switch (method) {
            case "book":
                const code = req.query["code"] ? Number.parseInt(req.query["code"]) : undefined;
                if (code === undefined) return res.status(404).end("Неверный запрос к API.");
                return res.json(library.get_book(code)).end();
            case "books":
                const filter = req.query["filter"];
                if (!filter) return res.status(404).end("Неверный запрос к API.");
                return res.json(library.get_books_sorted(filter)).end();
            default:
                return res.status(404).end("Неверный запрос к API.");
        }
    });

    /** Accepts:
     * @param book = book to add
     */
    server.put("/books", (req, res) => {
        const book = req.body.book ? JSON.parse(req.body.book) : undefined;
        if (book) library.set_book(book);
        return res.end();
    });

    /** Accepts:
     * @param book = book to update
     */
    server.post("/books", (req, res) => {
        const book = req.body.book ? JSON.parse(req.body.book) : undefined;
        if (book) {
            book.code = Number.parseInt(book.code.toString());
            library.edit_book(book);
        }
        return res.end();
    });

    /** Accepts:
     * @param code = code of the book to delete
     */
    server.delete("/books", (req, res) => {
        const code = req.body.code ? Number.parseInt(req.body.code) : undefined;
        if (code !== undefined) library.delete_book(code);
        return res.end();
    });



    /** Accepts:
     * @param image = new book cover image
     */
    server.post("/cover", (req, res) => {
        let form = new formidable.IncomingForm();
        form.parse(req, function (err, fields, files) {
            coverage.add_cover(files, function () {
                res.end("./covers/" + files.image.name);
            });
        });
    });



    server.put("/part", (req, res) => {
        const take_book = req.body["take-book"] ? Number.parseInt(req.body["take-book"]) : undefined;
        const give_book = req.body["give-book"] ? Number.parseInt(req.body["give-book"]) : undefined;
        const sell_book = req.body["sell-book"] ? Number.parseInt(req.body["sell-book"]) : undefined;
        if (take_book !== undefined) {
            library.give_book(take_book, JSON.parse(req.body["params"]));
            return res.end();
        } else if (give_book !== undefined) {
            library.return_book(give_book);
            return res.end();
        } else if (sell_book !== undefined) {
            library.sell_book(sell_book);
            return res.end();
        }
    });

    server.post("/settings", (req, res) => {
        if (req.body.settings) settings.alter_settings(req.body.settings);
        return res.end();
    });
}
