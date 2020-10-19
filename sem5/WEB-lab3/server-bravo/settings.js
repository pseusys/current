const fs = require('fs');
const path = require('path');



function write_settings (settings) {
    const lib = JSON.stringify(settings);
    fs.writeFileSync(path.join(__dirname, "../storage-bravo/settings.json"), lib);
}

module.exports.read_settings = function () {
    const library = fs.readFileSync(path.join(__dirname, "../storage-bravo/settings.json"));
    return JSON.parse(library.toString());
}

module.exports.alter_settings = function (set) {
    const settings = module.exports.read_settings();
    for (const setting of settings) if (set[setting.name]) setting.value = set[setting.name];
    write_settings(settings);
}
