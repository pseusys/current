const trade = require('./trader');

let io;

module.exports.setup_io = function (server) {
    io = require('socket.io')(server, { cors: { origin: '*' } });

    io.on("connection", (socket) => {
        console.log("Someone connected!");
        socket.on("join", () => {
            this.trade(trade.its_time());
        });

        socket.on("buy", (data) => {
            const d = JSON.parse(data);
            trade.buy(d.user, d.action, d.quantity);
        });

        socket.on("sell", (data) => {
            const d = JSON.parse(data);
            trade.sell(d.user, d.action, d.quantity);
        });

        socket.on("message", (message) => {
            const p = JSON.parse(message);
            console.log(p.sender, "said:", p.msg);
        });
    });
}



function broadcast (event, data = {}) {
    const j = JSON.stringify(data);
    io.emit(event, j);
    console.log(event + ":", j);
}

module.exports.message = function (message, display = false) {
    broadcast("message", { sender: "server", msg: message, display: display });
}

module.exports.resolve = function () {
    broadcast("resolve");
}

module.exports.trade = function (on) {
    broadcast("update", { ongoing: on });
}
