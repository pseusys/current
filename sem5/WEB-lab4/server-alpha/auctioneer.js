const library = require('./library');
const trader = require('./trade');
const population = require('./population');

let settings;
let books;
let time;

let bets_allowed = false;
let trades = {};

let current_winner;
let current_bet;

function sec(s) {
    return new Promise(resolve => setTimeout(resolve, s * 1000));
}

module.exports.time = function () {
    return time;
}

module.exports.auct = function (set) {
    time = set.date_time;
    let date_diff = Date.parse(time) - new Date();
    if (date_diff < 0) return;

    setTimeout(() => {
        settings = set;
        books = library.get_books_sorted("on-sale");
        trade().then(() => {
            console.log("Trading ended.");
        });
    }, date_diff);
    trader.time(time);
}

module.exports.bet = function (user, money) {
    console.log(user + " bets " + money);
    const better = population.get_user(user);
    if ((better.money >= money) &&
        (money > current_bet) &&
        bets_allowed &&
        (money - current_bet >= trades.min_step) &&
        (money - current_bet <= trades.max_step) &&
        (money > trades.price)) {

        current_bet = money;
        current_winner = better;
        trader.new_price(user, money);

    } else if (better.money < money) trader.message("Your funds insufficient!");
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
        current_winner = undefined;
        current_bet = book.part.start_cost;

        trader.new_book(book);
        trades = {
            price: book.part.start_cost,
            min_step: book.part.min_step,
            max_step: book.part.max_step
        };

        bets_allowed = true;
        trader.trading(trades);
        await sec(settings.sell_timeout);

        trader.countdown(Number.parseInt(settings.interval));
        await sec(settings.interval);
        bets_allowed = false;

        trader.message("Trading ends!");

        win(book, current_winner, current_bet);

        trader.message("Pause!");
        await sec(settings.pause);
    }
    trader.over();
}
