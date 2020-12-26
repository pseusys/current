import { isVisible, tile_size, t_size, load_file } from './map_manager.js';


// declarations
let image = new Image();
let sprites = [];


// initiator
export async function load_atlas (atlasJson, atlasImg) {
    sprites = [];
    image = await load_img(atlasImg);

    const atlas = JSON.parse(await load_file(atlasJson));
    for (const frame of atlas)
        sprites.push({
            name: frame.name,
            x: frame.x,
            y: frame.y,
            w: frame.width,
            h: frame.height
        });
}



// functions
export function load_img (path) {
    return new Promise(resolve => {
        const img = new Image();
        img.onload = function () { resolve(img); };
        img.src = "files/" + path;
    });
}


export function drawSprite (ctx, name, x, y, w, h, curFrame) {
    if (!isVisible(x, y, w, h)) return;
    const sprite = sprites.find(e => { return e.name === name; });
    ctx.drawImage(image, sprite.x + curFrame * t_size.x, sprite.y, t_size.x, t_size.y, x, y, w, h);
}

export function drawSpriteRotated (ctx, name, x, y, w, h, curFrame, angle) {
    if (!isVisible(x, y, w, h)) return;
    const sprite = sprites.find(e => { return e.name === name; });
    const tx = x + w / 2;
    const ty = y + h / 2;
    ctx.translate(tx, ty);
    ctx.rotate(angle);
    ctx.drawImage(image, sprite.x + curFrame * t_size.x, sprite.y, t_size.x, t_size.y * (h / tile_size),
        -w / 2, -h / 2, w, h);
    ctx.rotate(-angle);
    ctx.translate(-tx, -ty);
}
