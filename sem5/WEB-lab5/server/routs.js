module.exports.configure = function (server) {
    server.get("/login", (req, res) => {
        return res.render("login", {});
    });

    server.get("*", (req, res) => {
        return res.render("404", { cause: "Страница не найдена!" });
    });
}
