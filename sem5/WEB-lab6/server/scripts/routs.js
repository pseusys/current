module.exports.configure = function (server) {
    server.get("*", (req, res) => {
        return res.status(404).end("Страница не найдена!");
    });
}
