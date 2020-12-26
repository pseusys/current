let playing = false;
let audio = undefined;
let iterator = -1;



function Score(score) {
    this.score = score;
    this.time = (new Date()).toLocaleDateString("en-US");
}



export function set_record(score) {
    let scores = localStorage.getItem("score.count");
    scores = scores ? Number.parseInt(scores) : 0;
    localStorage.setItem("score.count", (scores + 1).toString());
    localStorage.setItem(scores.toString(), JSON.stringify(new Score(score)));
}

function get_records() {
    const scores = Number.parseInt(localStorage.getItem("score.count"));
    const score = [];
    for (let i = 0; i < scores; i++) {
        score.push(JSON.parse(localStorage.getItem(i.toString())));
    }
    return score;
}



export function fill_records() {
    const scores = document.getElementById("scores");
    const ratings = get_records();
    ratings.sort((a, b) => (Number.parseInt(a.score) < Number.parseInt(b.score)) ? 1 : -1);
    scores.innerHTML = "";
    if (ratings.length > 15) ratings.length = 15;
    for (let i = 0; i < ratings.length; i++) {
        scores.innerHTML = scores.innerHTML.concat("<li>" + localStorage.getItem("tetris.username") + " " + ratings[i].time.toString() + ": " + ratings[i].score.toString() + "</li>");
    }
}



export function notify_check() {
    if (!("Notification" in window)) {
        console.log("Skipping musical part as we can not check permission.")
    } else if (Notification.permission === "granted") {
        return true;
    } else if (Notification.permission !== "denied") {
        Notification.requestPermission().then(r => {
            return true;
        });
    }
    return false;
}

export function music(play) {
    const files = [
        "Chad Crouch - Algorithms",
        "Chuzausen - Raro Bueno"
    ];
    playing = play;
    play_on(files);
}

function play_on(files) {
    if (playing && audio) audio.play();
    else if (playing) {
        iterator++;
        if (iterator >= files.length) iterator = 0;
        new Notification("Playing: ".concat(files[iterator]), {icon: "./favicon.ico"});
        const file_name = "./music/".concat(files[iterator], ".mp3");
        audio = new Audio(file_name);
        audio.play();
        audio.onended = function () {
            audio = undefined;
            play_on(files, iterator);
        }
    } else if (audio) audio.pause();
}
