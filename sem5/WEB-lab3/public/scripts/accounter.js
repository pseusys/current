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



function return_book(old_book, onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/user", type: "PUT", data: "give-book=" + old_book})
        .done(function(result) { if (onsuccess) onsuccess(result ? JSON.parse(result) : "") })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

function take_book(new_book, onsuccess = undefined, onerror = undefined) {
    $.ajax({url: "/user", type: "PUT", data: "take-book=" + new_book})
        .done(function(result) { if (onsuccess) onsuccess(result ? JSON.parse(result) : "") })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}
