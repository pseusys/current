const express = require('express');
const path = require('path');
const bodyParser = require('body-parser');

const app = express();
app.use(express.static(path.join(__dirname, "../public")));
app.set("view engine", "pug");
app.set("views", "./views");

app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());

const server = express.Router();

server.use((req, res, next) => {
    console.log("Called", req.url, "with method", req.method, "at",
        (new Date()).toLocaleDateString("ru"), (new Date()).toLocaleTimeString("ru"), "processing...");
    return next();
});

const api = require('./api');
api.configure(server);
const auth = require('./auth');
auth.configure(server);
const routes = require('./routs');
routes.configure(server);

app.use("/", server).listen(3000, () => {
    console.log("Server started at: http://localhost:3000/");
});
