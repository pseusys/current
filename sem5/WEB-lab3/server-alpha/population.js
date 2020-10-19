const fs = require('fs');
const path = require('path');



function set_users(users) {
    const user_string = JSON.stringify(users);
    fs.writeFileSync(path.join(__dirname, "../storage/users.json"), user_string);
}

module.exports.get_users = function () {
    const user_book = fs.readFileSync(path.join(__dirname, "../storage/users.json"));
    return JSON.parse(user_book.toString());
}



module.exports.get_user = function(username, password = undefined) {
    const users = module.exports.get_users();
    for (const user of users) if (user.name === username) if (!password || check_pass(username, password)) return user;
    return false;
}

module.exports.set_user = function(username, password) {
    const user = {
        name: username,
        is_admin: false,
        registration: new Date(),
        books: []
    }

    const users = module.exports.get_users();
    for (const user of users) if (user.name === username) return false;
    users.push(user);
    set_users(users);
    set_pass(user.name, password);

    return user;
}

module.exports.edit_user = function(user) {
    const users = module.exports.get_users();
    for (let i = 0; i < users.length; i++) if (users[i].name === user.name) {
        users[i] = user;
        set_users(users);
        return true;
    }
    return false;
}

module.exports.delete_user = function(user) {
    const users = module.exports.get_users();
    for (let i = 0; i < users.length; i++) if (users[i].name === user.name) {
        if (delete_pass(user.name)) {
            users.splice(i, 1);
            set_users(users);
            return true;
        } else return false;
    }
    return false;
}



function get_passes() {
    const passwords = fs.readFileSync(path.join(__dirname, "../storage/passwords.json"));
    return JSON.parse(passwords.toString());
}

function set_passes(passes) {
    const passwords = JSON.stringify(passes);
    fs.writeFileSync(path.join(__dirname, "../storage/passwords.json"), passwords);
}

function check_pass(username, password) {
    const passes = get_passes();
    for (const pass of passes) if (pass.user === username) return pass.password === password;
    return false;
}

function set_pass(username, pass) {
    const passes = get_passes();
    passes.push({user: username, password: pass});
    set_passes(passes);
}

function delete_pass(username) {
    const passes = get_passes();
    for (let i = 0; i < passes.length; i++) if (passes[i].user === username) {
        passes.splice(i, 1);
        set_passes(passes);
        return true;
    }
    return false;
}