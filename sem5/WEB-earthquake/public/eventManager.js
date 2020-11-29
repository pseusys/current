const eventsManager = {
    bind: [],
    action: '',
    actionChanged: true,

    immediates: [],
    escaped: false,

    setup: function () {
        this.bind = [];
        this.action = [];
        this.actionChanged = true;

        this.bind[27] = 'esc'
        this.bind[87] = 'up'
        this.bind[83] = 'down'
        document.body.addEventListener("keydown", eventsManager.onKeyDown);
    },

    onKeyDown: function (event) {
        const action = eventsManager.bind[event.keyCode]
        if (action) {
            if (action !== eventsManager.action) eventsManager.actionChanged = true;
            eventsManager.action = action
        }
    },

    consume: function () {
        eventsManager.actionChanged = false
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
