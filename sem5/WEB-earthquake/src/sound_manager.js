// declarations
let clips = {};
let audio = null;



// initiators
export function load_audios (array) {
    clips = {};
    audio = null;
    for (const item of array) clips[item] = new Audio("files/" + item);
}



// functions
export function play (sound, repet, volume) {
    if (audio) audio.pause();
    audio = clips[sound];
    audio.play().catch(_ => {});
    audio.onended = function () { if (repet) play(sound, repet, volume); };
}

export function stop () {
    if (audio) audio.pause();
}
