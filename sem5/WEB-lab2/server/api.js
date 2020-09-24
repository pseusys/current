module.exports.configure = function (server) {
    const library = require('./library');



    const methods = ["sort", "get-book"];
    server.get("/api", (req, res, next) => {
        const method = req.query["method"];
        if (!method) return res.status(404).end("Wrong query.");

        switch (method) {
            case methods[0]:
                const condition = req.query["filter"];
                if (!condition) return res.status(404).end("Wrong query.");
                return res.end(JSON.stringify(library.get_books_sorted(condition)));
        }
    });
}
