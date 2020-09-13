import {cup_height} from './tetris.js'
import {cup_width} from './tetris.js'



const colors = ["cyan", "yellow", "orange", "green", "blue", "red", "purple"];

export class Tetramino {
    constructor(pts, field) {
        const cX = (pts[0].x + pts[1].x + pts[2].x + pts[3].x) / 4;
        const cY = (pts[0].y + pts[1].y + pts[2].y + pts[3].y) / 4;
        const center = new Coords(cX, cY);

        this.col = colors[(center.x * 10 + center.y) % 7];

        this.prove(pts, center, field);
    }

    rotate(field) {
        const pts = new Array(4);
        for (let i = 0; i < 4; i++) {
            let vector = new Coords(this.pts[i].x - this.center.x, this.pts[i].y - this.center.y);
            pts[i] = new Coords(this.center.x - vector.y, this.center.y + vector.x);
        }
        const center = this.center;
        this.prove(pts, center, field);
    }

    shift(field) {
        const pts = new Array(4);
        for (let i = 0; i < 4; i++) {
            pts[i] = new CopyCoords(this.pts[i]);
            pts[i].y += 1;
        }
        const center = new CopyCoords(this.center);
        center.y += 1;
        this.prove(pts, center, field);
    }

    move(x, field) {
        const pts = new Array(4);
        for (let i = 0; i < 4; i++) {
            pts[i] = new CopyCoords(this.pts[i]);
            pts[i].x += x;
        }
        const center = new CopyCoords(this.center);
        center.x += x;
        this.prove(pts, center, field);
    }

    prove(pts, center, field) {
        let accepted = true;
        for (let i = 0; i < 4; i++)
            accepted &= (pts[i].x >= 0) && (pts[i].x < cup_width) && (pts[i].y >= 0) && (pts[i].y < cup_height);
        accepted &= (center.x >= 0) && (center.x < cup_width) && (center.y >= 0) && (center.y < cup_height);
        if (accepted) {
            for (let i = 0; i < 4; i++) accepted &= field[pts[i].x][pts[i].y] === false;
            if (accepted) {
                this.pts = pts;
                this.center = center;
                return;
            }
        }
        console.log("Tetramino position rejected.");
    }
    
    touching(field) {
        for (let i = 0; i < 4; i++) {
            if (this.pts[i].y === (cup_height - 1)) return true;
            if (field[this.pts[i].x][this.pts[i].y + 1] === true) return true;
        }
        return false;
    }

    get points() {
        return this.pts;
    }

    get color() {
        return this.col;
    }
}



export function Coords(x, y) {
    this.x = Math.floor(x);
    this.y = Math.floor(y);
}

function CopyCoords(other) {
    this.x = other.x;
    this.y = other.y;
}



function contains(arr, coord) {
    for (let i = 0; i < arr.length; i++) if ((arr[i].x === coord.x) && (arr[i].y === coord.y)) return true;
    return false;
}

export function generate() {
    const result = [];
    result.push(new Coords(7, 3));
    for (let i = 0; i < 3; i++) {
        const ways = [];
        for (let j = 0; j < result.length; j++) {
            ways.push(new Coords(result[j].x - 1, result[j].y));
            ways.push(new Coords(result[j].x + 1, result[j].y));
            ways.push(new Coords(result[j].x, result[j].y - 1));
            ways.push(new Coords(result[j].x, result[j].y + 1));
        }
        const directions = [];
        for (let j = 0; j < ways.length; j++)
            if ((!contains(result, ways[j])) && (ways[j].y < 4)) directions.push(ways[j]);
        const chance = Math.floor(Math.random() * directions.length);
        result.push(directions[chance]);
    }
    return result;
}

export function print_next(tetramino) {
    const canvas = document.getElementById("next");
    const context = canvas.getContext("2d");

    const width = canvas.width;
    const height = canvas.height;
    const block_width = width / 7;
    const block_height = height / 7;
    const w_margin = width / 100;
    const h_margin = height / 100;

    context.fillStyle = "black";
    context.fillRect(0, 0, width, height);

    const dots = tetramino.points;
    for (let i = 0; i < 4; i++) {
        const image = new Coords(3 + (dots[i].x - tetramino.center.x), 3 + (dots[i].y - tetramino.center.y));
        context.fillStyle = tetramino.color;
        context.fillRect(image.x * block_width + w_margin, image.y * block_height + h_margin,
            block_width - w_margin, block_height - h_margin);
    }
}
