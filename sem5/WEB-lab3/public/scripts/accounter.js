function login(username, password, onsuccess = undefined, onerror = undefined) {
    $.post("/login", {username: username, password: password})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function signin(username, password, onsuccess = undefined, onerror = undefined) {
    $.post("/user", {username: username, password: password})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}



function logout(onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/login", type: "DELETE"})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function signout(onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/user", type: "DELETE"})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}



function part_user(part, user = undefined, onsuccess = undefined, onerror = undefined) {
    $.post("/users", {part: part, username: user})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function money_user(money, user = undefined, onsuccess = undefined, onerror = undefined) {
    $.post("/users", {money: money, username: user})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}
