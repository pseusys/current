import {Tetramino} from './tetramino.js';
import {generate} from './tetramino.js';
import {print_next} from './tetramino.js';
import {fill_records} from './ratings.js';
import {set_record} from './ratings.js';


let field;

export const cup_width = 16;
export const cup_height = 28;
const cup_height_visual = cup_height - 4;

const static_color = "gray";
const fill_color = "black";
const highlight_color = "white";



let canvas;
let context;
let width;
let height;
let block_width;
let block_height;
let w_margin;
let h_margin;



let next;
let current;

let interval = undefined;
let paused = false;
let timeout = 1000;

let points = 0;



function generate_next() {
    current = next;
    next = new Tetramino(generate(), field);
    print_next(next);
}

function pause() {
    paused = !paused;
    if (paused) document.getElementById("status").innerText = "Paused";
    else document.getElementById("status").innerText = "";
}

function score(plus) {
    points += plus;
    timeout = Math.floor((1/(points/500 + 5/4) + 0.2) * 1000);
    document.getElementById("score").innerText = "Score: ".concat(points.toString());
    document.getElementById("level").innerText = "Speed: ".concat(timeout.toString());
}



window.onload = function() {
    let username = localStorage.getItem("tetris.username");
    if (username !== null) document.title = username + "'s Tetris";

    document.getElementById("back").onclick = function () {
        localStorage.removeItem("tetris.username");
        window.location = "index.html";
    }
    document.getElementById("start").onclick = function () {start_game()};

    document.getElementById("rating_holder").style.display = "none";
    document.getElementById("canvas_holder").style.display = "flex";

    const canvas = document.getElementById("canvas");
    const context = canvas.getContext("2d");
    context.fillStyle = fill_color;
    context.fillRect(0, 0, canvas.width, canvas.height);
}

function start_game() {
    canvas = document.getElementById("canvas");
    context = canvas.getContext("2d");

    width = canvas.width;
    height = canvas.height;
    block_width = width / cup_width;
    block_height = height / cup_height_visual;
    w_margin = width / 100;
    h_margin = height / 100;

    init_field();
    score(0);

    next = new Tetramino(generate(), field);
    generate_next();
    print_field();

    interval = setTimeout(() => {game_step();}, timeout);
    document.addEventListener("keydown", playing_event_handler);

    document.getElementById("start").style.display = "none";
    document.getElementById("rating_holder").style.display = "none";
    document.getElementById("canvas_holder").style.display = "flex";
}

function stop_game() {
    document.removeEventListener("keydown", playing_event_handler);

    set_record(points);
    fill_records();

    timeout = 1000;
    points = 0;

    document.getElementById("start").style.display = "flex";
    document.getElementById("rating_holder").style.display = "flex";
    document.getElementById("canvas_holder").style.display = "none";
}

function playing_event_handler(event) {
    if (event.code === "Enter") pause();
    else if (!paused) {
        if (event.code === "ArrowDown") while (!current.touching(field)) current.shift(field);
        else if (event.code === "Space") current.rotate(field);
        else if (event.code === "ArrowLeft") current.move(-1, field);
        else if (event.code === "ArrowRight") current.move(1, field);
        else return;
        print_field();
    }
}

function game_step() {
    if (!paused) {
        current.shift(field);
        if (current.touching(field)) {
            const dots = current.points;
            generate_next();
            score(1);
            for (let i = 0; i < 4; i++) {
                field[dots[i].x][dots[i].y] = true;
                if (dots[i].y < 4) {
                    current = false;
                    score(-1);
                    break;
                }
            }
        }

        print_field();
        conditional_check();
    }

    if (current) {
        interval = setTimeout(() => {
            game_step();
        }, timeout);
    } else stop_game();
}



function conditional_check() {
    let rows = [];
    for (let j = 4; j < cup_height; j++) {
        let filled = true;
        for (let i = 0; i < 16; i++) {
            filled &= field[i][j];
        }
        if (filled) rows.push(j);
    }

    if (rows.length > 4) {
        console.log("Impossible row deletion condition.");
        return;
    }

    if (rows.length > 0) {
        context.fillStyle = highlight_color;
        for (let i = 0; i < rows.length; i++) {
            for (let j = 0; j < cup_width; j++) {
                context.fillRect(j * block_width + w_margin, (rows[i] - 4) * block_height + h_margin,
                    block_width - w_margin, block_height - h_margin);
            }
        }

        let skip = 0;
        for (let i = 0; i < rows.length; i++) {
            for (let j = cup_height - 1; j > 4; j--) {
                if (rows[i] === j) skip = 1;
                for (let i = 0; i < 16; i++) {
                    field[i][j] = field[i][j - skip];
                }
            }
        }

        score(10 * rows.length);
    }
}



function init_field() {
    field = new Array(cup_width);
    for (let i = 0; i < cup_width; i++) {
        field[i] = new Array(cup_height); // 24 + 4 extra, invisible for forms construction.
        for (let j = 0; j < cup_height; j++) {
            field[i][j] = false;
        }
    }
}

function print_field () {
    context.fillStyle = fill_color;
    context.fillRect(0, 0, width, height);

    for (let i = 0; i < cup_width; i++) {
        for (let j = 0; j < cup_height_visual; j++) {
            context.fillStyle = field[i][j + 4] ? static_color : fill_color;
            context.fillRect(i * block_width + w_margin, j * block_height + h_margin,
                block_width - w_margin, block_height - h_margin);
        }
    }

    if (current) {
        const dots = current.points;
        for (let i = 0; i < 4; i++) {
            if (dots[i].y < 4) continue;
            context.fillStyle = current.color;
            context.fillRect(dots[i].x * block_width + w_margin, (dots[i].y - 4) * block_height + h_margin,
                block_width - w_margin, block_height - h_margin);
        }
    }
}
