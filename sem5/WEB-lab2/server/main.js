const express = require('express');
const app = express();
const server = express();

const api = require('./api');
api.configure(server);
const auth = require('./auth');
auth.configure(server);
const routes = require('./routs');
routes.configure(server);

app.use("/", server).listen(3000, () => {
    console.log("Server started at: http://localhost:3000/");
});
