import { init } from './game_manager.js';


// declarations
const canvas = document.getElementById("game");
const ctx = canvas.getContext('2d');



// functions
export function start_game (game_mode) {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    ctx.imageSmoothingEnabled = false;
    ctx.textAlign = "center";

    init(game_mode, ctx).then();
}
