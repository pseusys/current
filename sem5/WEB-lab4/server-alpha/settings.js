// @flow

const fs = require('fs');
const path = require('path');
const auct = require('./auctioneer');



function write_settings (settings) {
    const lib = JSON.stringify(settings);
    fs.writeFileSync(path.join(__dirname, "../storage-alpha/settings.json"), lib !== undefined ? lib : "");
}

module.exports.read_settings = function (): Array<{ name: string, value: string }> {
    const library = fs.readFileSync(path.join(__dirname, "../storage-alpha/settings.json"));
    return JSON.parse(library.toString());
}

module.exports.alter_settings = function (set: {[key: string]: string}): void {
    const settings = module.exports.read_settings();
    for (const setting of settings) if (set[setting.name]) setting.value = set[setting.name];
    write_settings(settings);
    auct.auct(set);
}
