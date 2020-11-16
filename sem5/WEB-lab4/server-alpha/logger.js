// @flow

const winston = require('winston');
const Transport = require('winston-transport');

class RollbarTransport extends Transport {
    constructor(opts) {
        super(opts);
        const Rollbar = require("rollbar");
        this.rollbar = new Rollbar({
            accessToken: '064f75178d3e4896bf8b9d13fbe819c7',
            captureUncaught: true,
            captureUnhandledRejections: true
        });
    }

    log(info, callback) {
        setImmediate(() => {
            this.emit('logged', info);
        });
        this.rollbar.log(info);
        callback();
    }
}

const logger: typeof winston.Logger = winston.createLogger({
    exitOnError: false,
    transports: [
        new winston.transports.Console({ level: "silly", format: winston.format.combine(
            winston.format.colorize(),
                winston.format.simple()
            )} ),
        new winston.transports.File({ filename: 'combined.log', format: winston.format.json() }),
        new RollbarTransport( { format: winston.format.json() } )
    ]
});

module.exports.logger = logger;
