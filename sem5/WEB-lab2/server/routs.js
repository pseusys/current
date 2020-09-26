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
            book: book,
            publication: (new Date(book.publication)).toLocaleDateString("ru"),
            return_date: book["taken"] ? (new Date(book["taken"]["return"])).toLocaleDateString("ru") : undefined,
            taken_by_me: book["taken"] ? (book["taken"]["reader"] === req.user.name) : book["taken"],
            holder: req.user.name,
            is_admin: req.user.is_admin
        });
    });

    server.get("/", (req, res, next) => {
        return res.render("lib", {user: req.user});
    });

    server.get("*", (req, res) => {
        return res.render("404", {cause: "Страница не найдена!"});
    });
}
