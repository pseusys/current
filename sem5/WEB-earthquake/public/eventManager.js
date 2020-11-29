const eventsManager = {
    immediates: ['ArrowUp', 'ArrowDown', 'w', 's'],
    callback: undefined,
    escaped: false,

    setup: function () {
        this.callback = undefined;
        this.escaped = false;

        document.body.addEventListener("keydown", eventsManager.onKeyDown);
    },

    onKeyDown: function (event) {
        if (event.key === 'Escape') eventsManager.escaped = true;
        else if (eventsManager.immediates.includes(event.key) && (eventsManager.callback)) eventsManager.callback(event.key);
    },



    onEnd: function (event) {
        if (event.key === 'Escape') {
            soundManager.stop();
            document.getElementById("canvas_id").style.display = "none";
            document.getElementById("menu").style.display = "block";
            document.body.removeEventListener("keydown", eventsManager.onEnd);
        }
    },

    finish: function () {
        document.body.removeEventListener("keydown", eventsManager.onKeyDown);
        document.body.addEventListener("keydown", eventsManager.onEnd);
    }
}
