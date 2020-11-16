export function push_books(onsuccess = undefined, onerror = undefined) {
    $.post("/lib")
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}

export function pull_books(onsuccess = undefined, onerror = undefined) {
    $.get("/lib")
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}
