const soundManager = {
    clips: {}, // звуковые эффекты
    audio: undefined,

    init: function () {
        soundManager.clips = {};
        soundManager.audio = undefined;
    },

    load: function (path) {
        soundManager.clips[path] = new Audio('/files?name=' + path);
    },

    loadArray: function (array) {
        for (let i = 0; i < array.length; i++) {
            soundManager.load(array[i]);
        }
    },



    play: function (sound, repet, volume) {
        if (soundManager.audio) soundManager.audio.pause();
        soundManager.audio = this.clips[sound];
        soundManager.audio.play().catch(_ => {});
        soundManager.audio.onended = function () { if (repet) soundManager.play(sound, repet, volume); };
    },

    stop: function () {
        if (soundManager.audio) soundManager.audio.pause();
    }
}
