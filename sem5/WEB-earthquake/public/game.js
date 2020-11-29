const canvas = document.getElementById("canvas_id");
const ctx = canvas.getContext('2d');

function start_game () {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    ctx.imageSmoothingEnabled = false;
    ctx.textAlign = "center";

    gameManager.play(game_mode);
}
