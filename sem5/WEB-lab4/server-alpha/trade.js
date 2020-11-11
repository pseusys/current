// @flow

const auct = require('./auctioneer');
const log = require('./logger');

let io;

module.exports.setup_io = function (server) {
    io = require('socket.io')(server);

    io.on("connection", (socket) => {
        socket.on("join", () => {
            socket.emit("time", auct.time());
        });

        socket.on("bet", (data) => {
            const d = JSON.parse(data);
            auct.bet(d.user, d.money);
        });

        socket.on("message", (message) => {
            const p = JSON.parse(message);
            log.logger.info(p.sender + " said: " + p.msg);
        });
    });
}



function broadcast (event, data = {}) {
    const j = JSON.stringify(data);
    io.emit(event, j);
    log.logger.info(event + ": " + j);
}

module.exports.time = function (time) {
    broadcast("time", time);
}

module.exports.start = function () {
    broadcast("start");
}

module.exports.message = function (message) {
    broadcast("message", { sender: "server", msg: message });
}

module.exports.new_book = function (book) {
    broadcast("new_book", { name: book.name, author: book.author });
}

module.exports.trading = function (trades) {
    broadcast("trading", trades);
}

module.exports.countdown = function (interval) {
    broadcast("countdown", interval);
}

module.exports.new_price = function (user, money) {
    broadcast("new_price", { user: user, money: money });
}

module.exports.over = function () {
    broadcast("over")
}
