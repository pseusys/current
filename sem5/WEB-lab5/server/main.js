const express = require('express');
const path = require('path');
const bodyParser = require('body-parser');
const cors = require('cors');

const cors_options = {
  credentials: true,
  origin: 'http://localhost:4200',
  methods: 'GET, POST, PUT, DELETE'
};

const app = express();

app.use(cors(cors_options));

app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());

const server = express.Router();

server.use((req, res, next) => {
    console.log("Called " + req.url + " with method " + req.method + " at " +
        (new Date()).toLocaleDateString("ru") + " " + (new Date()).toLocaleTimeString("ru") +
        " processing...");
    return next();
});

const auth = require('./auth');
auth.configure(server);
const actionize = require('./actionize');
actionize.configure(server);
const set = require('./set');
set.configure(server);
const routes = require('./routs');
routes.configure(server);

app.use("/", server);

app.listen(8081, () => {
    console.log("Server started at: https://localhost:8081/");
});
