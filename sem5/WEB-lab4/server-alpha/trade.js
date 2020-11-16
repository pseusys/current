// @flow

const express = require('express');

const auct = require('./auctioneer');
const log = require('./logger');

let io;

module.exports.setup_io = function (server: typeof express.Router): void {
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

module.exports.time = function (time: string): void {
    broadcast("time", time);
}

module.exports.start = function (): void {
    broadcast("start");
}

module.exports.message = function (message: string): void {
    broadcast("message", { sender: "server", msg: message });
}

module.exports.new_book = function (book: { name: string, author: string }): void {
    broadcast("new_book", { name: book.name, author: book.author });
}

module.exports.trading = function (trades: {}): void {
    broadcast("trading", trades);
}

module.exports.countdown = function (interval: number): void {
    broadcast("countdown", interval);
}

module.exports.new_price = function (user: string, money: number): void {
    broadcast("new_price", { user: user, money: money });
}

module.exports.over = function (): void {
    broadcast("over")
}
