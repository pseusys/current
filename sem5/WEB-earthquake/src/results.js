// declarations
const key = "scores";

class Score {
    constructor(mode, score) {
        this.mode = mode;
        this.score = score;
        this.time = new Date();
    }
}



// functions
export function push_score (mode, score) {
    const scores = get_scores();
    scores.push(new Score(mode, score));
    window.localStorage.setItem(key, JSON.stringify(scores));
}

function get_scores () {
    const unpacked = JSON.parse(window.localStorage.getItem(key));
    return unpacked === null ? [] : unpacked;
}


function populate_score(score, i) {
    score.time = new Date(score.time);
    const elem = document.createElement("tr");

    const index = document.createElement("th");
    index.scope = "row";
    index.className = "ins";
    index.style.cssText = "text-align: center;";
    index.innerText = i;

    const time = document.createElement("td");
    time.className = "ins";
    time.style.cssText = "text-align: center;";
    time.innerText = score.time.toLocaleDateString("ru") + " " + score.time.toLocaleTimeString("ru");

    const mode = document.createElement("td");
    mode.className = "ins";
    mode.style.cssText = "text-align: center;";
    mode.innerText = score.mode;

    const value = document.createElement("td");
    value.className = "ins";
    value.style.cssText = "text-align: center;";
    value.innerText = score.score;

    elem.appendChild(index);
    elem.appendChild(time);
    elem.appendChild(mode);
    elem.appendChild(value);
    return elem;
}


export function display_results () {
    const scores = get_scores().sort((a, b) => {
        return b.score - a.score;
    }).slice(0, 10);
    if (scores.length === 0) {
        const table_holder = document.getElementById("table_holder");

        const ins = document.createElement("h2");
        ins.className = "ins middle";
        ins.innerText = "No scores yet!";

        table_holder.innerHTML = "";
        table_holder.appendChild(ins);
    } else {
        const table = document.getElementById("table");
        for (let i = 0; i < scores.length; i++) table.appendChild(populate_score(scores[i], i));
    }

    document.body.addEventListener("keydown", listener);
}



// listener
function listener (event) {
    if (event.key === 'Escape') {
        document.getElementById("results").style.display = "none";
        document.getElementById("menu").style.display = "block";
        document.body.removeEventListener("keydown", listener);
    }
}
