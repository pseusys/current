let game_mode = 1;

function game (mode) {
    game_mode = mode;
    document.getElementById("menu").style.display = "none";
    document.getElementById("canvas_id").style.display = "block";
    start_game();
}

function results () {
    document.getElementById("menu").style.display = "none";
    document.getElementById("results").style.display = "block";
    display_results();
}
