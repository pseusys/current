// @flow

const library = require('./library');
const trader = require('./trade');
const population = require('./population');
const log = require('./logger');

let settings;
let books;
let time;

let bets_allowed = false;
let trades = {};

let current_winner;
let current_bet;

function sec(s) {
    return new Promise(resolve => setTimeout(resolve, Number.parseInt(s) * 1000));
}

function runAtDate(date, func) {
    const diff = date - new Date();
    if (diff > 0x7FFFFFFF) // setTimeout limit is MAX_INT32=(2^31-1)
        setTimeout(() => { runAtDate(date, func); }, 0x7FFFFFFF);
    else
        setTimeout(func, diff);
}

module.exports.time = function (): string {
    return time;
}

module.exports.auct = function (set: { date_time: string, sell_timeout: string, interval: string, pause: string }): void {
    time = set.date_time;
    const date = Date.parse(time);
    let date_diff = date - new Date();
    if (date_diff < 0) return;

    runAtDate(date, () => {
        settings = set;
        books = library.get_books_sorted("on-sale");
        trade().catch((e) => { log.logger.error(e); })
            .then(() => { log.logger.info("Trading ended successfully!"); });
    });
    trader.time(time);
}

module.exports.bet = function (user: string, money: number): void {
    trader.message(user + " bets " + money);
    const better = population.get_user(user);
    if (typeof better === "boolean") {
        trader.message("User not found!");
        return;
    }
    if ((better.money >= money) &&
        (money > current_bet) &&
        bets_allowed &&
        (money - current_bet >= trades.min_step) &&
        (money - current_bet <= trades.max_step) &&
        (money > trades.price)) {

        current_bet = money;
        current_winner = better;
        trader.new_price(user, money);

    } else if (better.money < money) trader.message("Funds insufficient!");
    else if (!bets_allowed) trader.message("Bets not allowed!");
    else if (money - current_bet < trades.min_step) trader.message("Minimal step underrun!");
    else if (money - current_bet > trades.max_step) trader.message("Maximal step overrun!");
    else if (money <= trades.price) trader.message("Bet less than minimal cost!");
    else trader.message("Bet rejected!");
}

function win (book) {
    if (!!current_winner) {
        current_winner.money -= current_bet;
        current_winner.books.push(book.code);
        population.edit_user(current_winner);

        book.owner = { user: current_winner.name, price: current_bet };
        book.part = false;
        library.edit_book(book);

        trader.message("Book " + book.name + " bought by " + current_winner.name + " for " + current_bet + " moneys!");
    }
}

async function trade () {
    trader.start();
    for (const book of books) {
        if (typeof book.part !== "boolean") {
            current_winner = undefined;
            current_bet = book.part.start_cost;

            trader.new_book(book);
            trades = {
                price: (typeof book.part !== "boolean") ? book.part.start_cost : -1,
                min_step: (typeof book.part !== "boolean") ? book.part.min_step : -1,
                max_step: (typeof book.part !== "boolean") ? book.part.max_step : -1
            };

            bets_allowed = true;
            trader.trading(trades);
            await sec(settings.sell_timeout);

            trader.countdown(Number.parseInt(settings.interval));
            await sec(settings.interval);
            bets_allowed = false;

            trader.message("Trading ends!");

            win(book);

            trader.message("Pause!");
            await sec(settings.pause);
        }
    }
    trader.over();
}
