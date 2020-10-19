const fs = require('fs');
const path = require('path');

module.exports.add_cover = function (files, after) {
    const cover = path.join(__dirname, "../public-alpha/covers/") + files.image.name;
    fs.rename(files.image.path, cover, function (err) {
        if (err) throw err;
        after();
    });
}

module.exports.remove_cover = function (cover) {
    if (cover !== "./covers/default.jpg")
        fs.unlink(path.join(__dirname, "../public-alpha/covers/") + cover.split("/").pop(), function () {});
}