const express = require('express');
const path = require('path');

const app = express();
app.use(express.static(path.join(__dirname, "./public")));

const router = express.Router();

router.get("/", (req, res) => {
    return res.sendFile(path.join(__dirname, './public/index.html'));
});
router.get("/game", (req, res) => {
    return res.sendFile(path.join(__dirname, './public/game.html'));
});
router.get("/results", (req, res) => {
    return res.sendFile(path.join(__dirname, './public/results.html'));
});
router.get("/files", (req, res) => {
    if (!req.query.name) res.status(404).end();
    return res.sendFile(path.join(__dirname, './files/' + req.query.name));
});
router.get("*", (req, res) => {
    return res.status(404).end();
});

app.use('/', router);
const listener = app.listen(8080, () => {
    console.log("Server started at: http://localhost:" + listener.address().port);
});
