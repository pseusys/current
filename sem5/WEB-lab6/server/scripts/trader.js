const sock = require('./socketer');
const substractor = require('./substractor');

let settings;
let trade_start, trade_end;

function runAtDate(date, func) {
    const diff = date - new Date();
    if (diff > 0x7FFFFFFF) // setTimeout limit is MAX_INT32=(2^31-1)
        setTimeout(() => { runAtDate(date, func); }, 0x7FFFFFFF);
    else
        setTimeout(func, diff);
}

module.exports.trade = function (set) {
    settings = set;
    trade_start = Date.parse(set.tradeStart);
    trade_end = Date.parse(set.tradeEnd);

    runAtDate(trade_start, () => { sock.trade(true); });
    runAtDate(trade_end, () => { sock.trade(false); });
}

module.exports.setup = function () {
    this.trade(substractor.read_settings());
}

module.exports.its_time = function () {
    const now = new Date();
    return ((now >= trade_start) && (now <= trade_end));
}

module.exports.buy = function (user, action, quantity) {
    const now = new Date();
    if ((now < trade_start) || (now > trade_end)) {
        sock.message("Время торгов ещё не пришло!", true);
        return;
    }

    sock.message(user + " хочет купить " + quantity + " акций " + action);
    const client = substractor.get_broker(user);
    const index = client.actions.findIndex((e) => { return e.name === action; });
    const object = substractor.get_action(action);

    if ((client.money >= object.cost * quantity) && (object.stocked >= quantity)) {
        client.money -= object.cost * quantity;
        object.stocked -= quantity;
        if (index !== -1) client.actions[index].quantity += quantity;
        else client.actions.push({ name: action, quantity: quantity });
    } else if (client.money < object.cost * quantity) sock.message("У " + user + " не хватает денег", true);
    else if (object.stocked < quantity) sock.message("В продаже недостаточно акций " + action, true);

    substractor.edit_broker(client);
    substractor.edit_action(object);
    sock.resolve();
}

module.exports.sell = function (user, action, quantity) {
    const now = new Date();
    if ((now < trade_start) || (now > trade_end)) {
        sock.message("Время торгов ещё не пришло!", true);
        return;
    }

    sock.message(user + " хочет продать " + quantity + " акций " + action);
    const client = substractor.get_broker(user);
    const index = client.actions.findIndex((e) => { return e.name === action; });
    const object = substractor.get_action(action);


    if ((index !== -1) && (client.actions[index].quantity >= quantity)) {
        client.money += object.cost * quantity;
        object.stocked += quantity;
        client.actions[index].quantity -= quantity;
    } else sock.message("У " + user + " не хватает акций " + action, true);

    substractor.edit_broker(client);
    substractor.edit_action(object);
    sock.resolve();
}
