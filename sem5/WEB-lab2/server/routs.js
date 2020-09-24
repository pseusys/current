module.exports.configure = function (server) {
    const express = require('express');
    const path = require('path');
    const library = require('./library');

    server.use(express.static(path.join(__dirname, "../public")));
    server.set("view engine", "pug");
    server.set("views", "./views");



    server.use((req, res, next) => {
        console.log("Called", req.url, "with method", req.method, "at",
            (new Date()).toLocaleDateString("ru"), (new Date()).toLocaleTimeString("ru"), "processing...");
        return next();
    });

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
        const code = req.query["code"];
        if (!code) return res.render("404", {cause: "Wrong query."});

        const book = library.get_book(code);
        if (!book) return res.render("404", {cause: "Book with code " + code + " does not exist."});

        return res.render("book", {
            book_name: book["name"],
            book_author: book["author"],
            book_desc: book["description"],
            cover_img: book["cover"]
        });
    });

    server.get("/", (req, res, next) => {
        return res.render("lib", {user: req.user.name});
    });

    server.get("*", (req, res) => {
        return res.render("404", {cause: "Page does not exist!"});
    });
}
