import { start_game } from './game.js';
import { display_results } from './results.js';


// functions
function play(mode) {
    document.getElementById("menu").style.display = "none";
    document.getElementById("game").style.display = "block";
    start_game(mode);
}



// listeners
document.getElementById("game_1_button").onclick = function () { play(1); };
document.getElementById("game_2_button").onclick = function () { play(2); };
document.getElementById("results_button").onclick = function () {
    document.getElementById("menu").style.display = "none";
    document.getElementById("results").style.display = "block";
    display_results();
};
