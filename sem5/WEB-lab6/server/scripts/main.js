const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const http = require('http');

const origins = ['http://localhost:4200', 'http://localhost:3000'];
const cors_options = {
  credentials: true,
  origin: function (origin, callback) {
      if (origins.indexOf(origin) !== -1) {
          callback(null, true)
      } else {
          callback(new Error('Not allowed by CORS'))
      }
  },
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

const trader = require('./trader');
trader.setup();

const auth = require('./auth');
auth.configure(server);
const actionize = require('./actionize');
actionize.configure(server);
const set = require('./set');
set.configure(server);
const routes = require('./routs');
routes.configure(server);

app.use("/", server);

const httpServer = http.createServer(app);
const sock = require('./socketer');
sock.setup_io(httpServer);

httpServer.listen(8081, () => {
    console.log("Server started at: https://localhost:8081/");
});
