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
    ratings.sort((a, b) => (Number.parseInt(a.score) > Number.parseInt(b.score)) ? 1 : -1);
    scores.innerHTML = "";
    for (let i = 0; i < ratings.length; i++) {
        scores.innerHTML = scores.innerHTML.concat('<li>' + ratings[i].time.toString() + ': ' + ratings[i].score.toString() + '</li>' );
    }
}
