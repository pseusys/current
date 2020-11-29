const soundManager = {
    clips: {}, // звуковые эффекты
    audio: undefined,

    init: function () {
        this.clips = {};
        this.audio = undefined;
    },

    load: function (path) {
        this.clips[path] = new Audio('/files?name=' + path);
    },

    loadArray: function (array) {
        // загрузка массива звуков
        for (let i = 0; i < array.length; i++) {
            soundManager.load(array[i]);
        }
    },



    play: function (sound, repet, volume) {
        if (this.audio) this.audio.pause();
        this.audio = this.clips[sound];
        this.audio.play();
        this.audio.onended = function () { if (repet) this.play(sound, repet, volume); };
    },

    stop: function () {
        if (this.audio) this.audio.pause();
    }
}
