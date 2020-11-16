// @flow

const express = require('express');

const library = require('./library');
const population = require('./population');
const settings = require('./settings');

module.exports.configure = function (server: typeof express.Router) {
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
        const user = population.get_user(req.user.name);
        if (!book) return res.render("404", {cause: "Книга с кодом " + code + " не существует."});

        return res.render("book", {
            holder: req.user.name,
            holder_money: (typeof user !== "boolean") ? user.money : 0,
            is_admin: req.user.is_admin,

            book_owner: (typeof book !== "boolean") ? book.owner : "",
            book_code: (typeof book !== "boolean") ? book.code : -1,
            book_name: (typeof book !== "boolean") ? book.name : "",
            book_cover: (typeof book !== "boolean") ? book.cover : "",
            book_part: (typeof book !== "boolean") ? book.part : false,
            book_author: (typeof book !== "boolean") ? book.author : "",
            book_description: (typeof book !== "boolean") ? book.description : "",
            book_publication: (typeof book !== "boolean") ? book.publication : ""
        });
    });

    server.get("/users", (req, res) => {
        return res.render("users", {
            is_admin: req.user.is_admin,
            users: population.get_users()
        });
    });

    server.get("/settings", (req, res) => {
        return res.render("settings", {
            settings: settings.read_settings()
        });
    });

    server.get("/", (req, res) => {
        const books = [];
        const user = population.get_user(req.user.name);
        if (typeof user === "boolean") return res.end();
        for (const code of user.books) books.push(library.get_book(code));
        return res.render("lib", {
            user_name: req.user.name,
            is_admin: req.user.is_admin,
            user_part: user.part,
            user_money: user.money,
            user_books: books,
            all_books: library.get_books_sorted(req.query.filter)
        });
    });

    server.get("*", (req, res) => {
        return res.render("404", {cause: "Страница не найдена!"});
    });
}
