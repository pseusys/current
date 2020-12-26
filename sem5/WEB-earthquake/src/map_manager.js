import { factory, fix_offsets, initPlayer, level } from './game_manager.js';
import { load_img } from './sprite_manager.js';


// declarations
let map_data = null;
let tiles_layer = null;
let tile_sets = [];

let x_count = 0;
let y_count = 0;

export let view = { x: 0, y: 0, w: window.innerWidth, h: window.innerHeight };

export let t_size = { x: 32, y: 32 };
let m_size = { x: 8, y: 16 };

export let tile_size = 135;
let map_size = { x: 1080, y: 2160 };
export let floor = { l: 945, h: 135 };

export let world_speed = 32;
export let world_speedup = 0.001;
export let world_run = 0;
export let world_offset = 0;



// initiator
export async function load_map (path) {
    tiles_layer = null;
    tile_sets = [];
    view = { x: 0, y: 0, w: window.innerWidth, h: window.innerHeight };
    world_speed = 32;
    world_speedup = 0.001;
    world_run = 0;
    world_offset = 0;

    map_data = JSON.parse(await load_file(path));
    x_count = map_data['width'];
    y_count = map_data['height'];
    t_size.x = map_data['tilewidth'];
    t_size.y = map_data['tileheight'];
    tile_size = Math.ceil(window.innerHeight / y_count);
    m_size.x = x_count * t_size.x;
    m_size.y = y_count * t_size.y;

    map_size.x = x_count * tile_size;
    map_size.y = y_count * tile_size;

    floor.l = map_size.y - tile_size;
    floor.h = tile_size;

    for (const tile_set of map_data['tilesets']) {
        const img = await load_img(tile_set.image);
        const t = tile_set;
        const ts = {
            firstgid: t.firstgid,
            image: img,
            name: t.name,
            xCount: Math.floor(t['imagewidth'] / map_data['tilewidth']),
            yCount: Math.floor(t['imageheight'] / map_data['tileheight'])
        };
        tile_sets.push(ts);
    }
}



// setters
export function step (addition) { world_run += addition; }
export function speedup (addition) { world_speed += addition; }


// functions
export async function load_file (path) {
    return await (await fetch("files/" + path)).text();
}

export function parse_entities () {
    for (const layer of map_data['layers']) if (layer.type === 'objectgroup')
        for (const object of layer['objects']) if (object.name === 'player') {
            factory[object.type] = class extends factory[object.type] {
                constructor() {
                    super(object.name,
                        tile_size, level === 2 ? (floor.h + floor.l - tile_size) / 2 : floor.l - tile_size,
                        tile_size, tile_size);
                }
            };
            initPlayer(factory[object.type]);
        } else {
            factory[object.type] = class extends factory[object.type] {
                constructor() {
                    super(object.name, object.width / t_size.x * tile_size, object.height / t_size.y * tile_size);
                }
            }
        }
}


function get_tile (tileIndex) {
    const tile = { img: null, px: 0, py: 0 };
    const tile_set = tile_sets.find(e => { return e.firstgid <= tileIndex });
    tile.img = tile_set.image;
    const id = tileIndex - tile_set.firstgid;
    const x = id % tile_set.xCount;
    const y = Math.floor(id / tile_set.xCount);
    tile.px = x * t_size.x;
    tile.py = y * t_size.y;
    return tile;
}

export function isVisible (x, y, width, height) {
    return !((x + width < view.x) ||
        (y + height < view.y) ||
        (x > view.x + view.w) ||
        (y > view.y + view.h));
}

export function draw_map (ctx) {
    if (tiles_layer === null) tiles_layer = map_data['layers'].find(e => { return e.type === 'tilelayer' });
    for (let i = 0; i < tiles_layer.data.length; i++) if (tiles_layer.data[i] !== 0) {
        const tile = get_tile(tiles_layer.data[i]);
        let pX = (i % x_count) * tile_size;
        let pY = Math.floor(i / x_count) * tile_size;
        if (!isVisible(pX, pY, tile_size, tile_size)) continue;
        ctx.drawImage(tile.img, tile.px, tile.py, t_size.x, t_size.y, pX, pY, tile_size, tile_size);
    }
}


export function spread (x, ctx) {
    view.x += x;
    world_offset += x;
    ctx.translate(-x, 0);
    if (view.x > map_size.x / 2) {
        const limit = view.x - map_size.x / 2;
        ctx.translate(view.x, 0);
        fix_offsets(view.x);
        view.x = 0;
        spread(limit, ctx);
    }
}
