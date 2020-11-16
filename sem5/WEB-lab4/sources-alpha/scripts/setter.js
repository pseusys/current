export function set_settings(settings, onsuccess = undefined, onerror = undefined) {
    $.post("/settings", {settings: settings})
        .done(function(result) { if (onsuccess) onsuccess(result) })
        .fail(function(jqXHR) { if (onerror) onerror(jqXHR.responseText) });
}