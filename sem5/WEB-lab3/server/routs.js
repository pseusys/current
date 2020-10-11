module.exports.configure = function (server) {
    const library = require('./library');



    server.use((req, res, next) => {
        if (req.user) {
            if (req.url !== "/login") return next();
            else return res.redirect("/");
        } else {
            if (req.url === "/login") return next();
            else return res.redirect("/login");
        }
    });



    server.get("/login", (req, res) => {
        return res.render("login");
    });

    server.get("/book", (req, res) => {
        const code = req.query["code"] ? Number.parseInt(req.query["code"]) : undefined;
        if (code === undefined) return res.render("404", {cause: "Неверный запрос."});

        const book = library.get_book(code);
        if (!book) return res.render("404", {cause: "Книга с кодом " + code + " не существует."});

        return res.render("book", {
            return_date: book.taken ? (new Date(book.taken.return)).toLocaleDateString("ru") : undefined,
            taken_by_me: book.taken ? (book.taken.reader === req.user.name) : book.taken,
            holder: req.user.name,
            is_admin: req.user.is_admin,

            book_code: book.code,
            book_name: book.name,
            book_cover: book.cover,
            book_taken: book.taken,
            book_author: book.author,
            book_description: book.description,
            book_publication: book.publication
        });
    });

    server.get("/", (req, res) => {
        return res.render("lib", {
            user_name: req.user.name,
            is_admin: req.user.is_admin,
            all_books: library.get_books_sorted(req.query.filter),
            user_books: library.get_user_books(req.user)
        });
    });

    server.get("*", (req, res) => {
        return res.render("404", {cause: "Страница не найдена!"});
    });
}
