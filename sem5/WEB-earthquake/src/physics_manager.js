import { set_callback } from './event_manager.js';
import { entities, level } from './game_manager.js';
import {
    floor,
    speedup,
    spread,
    step,
    view,
    world_offset,
    world_run,
    world_speed,
    world_speedup
} from './map_manager.js';


// declarations
export let gravity = 1;
let dpc_y = 0;



// initiator
export function init_physics () {
    gravity = 1;
    dpc_y = 0;
    dpc_y = document.getElementById('dpi').offsetHeight;

    set_callback(queryEntities);
}



// functions
export function update_player (player) {
    if (player.flying) { // 100 times slower than real
        player.fly += 0.015;
        const new_speed = player.y_speed_prev + player.y_speedup * player.fly;
        const new_coord = player.pos_y + gravity * new_speed * 0.015 * dpc_y;

        if (new_coord > floor.l - player.size_y) {
            player.pos_y = floor.l - player.size_y;
            if (gravity === 1) player.flying = false;
            player.y_speed = player.y_speed_prev = player.fly = 0;
        } else if (new_coord < floor.h) {
            player.pos_y = floor.h;
            if (gravity === -1) player.flying = false;
            player.y_speed = player.y_speed_prev = player.fly = 0;
        } else {
            player.y_speed = new_speed;
            player.pos_y = new_coord;
        }
    }

    player.y_speedup += 0.0001;
}

export function update_npc (npc) {
    npc.screen_offset -= world_speed * 0.015;
    npc.pos_x = npc.screen_offset + view.x;
}

export function update_world (ctx) {
    step(0.015);
    speedup(world_speedup * world_run);
    spread(world_speed * 0.015, ctx);
}

export function trip (player) {
    const rout = world_speed * 0.015;
    const rand = player.pos_y + Math.random() * rout * 2 - rout;
    if (rand > floor.l - player.size_y) player.pos_y = floor.l - player.size_y;
    else if (rand < floor.h) player.pos_y = floor.h;
    else player.pos_y = rand;
}


export function count_offset () {
    return world_offset / dpc_y / 100;
}

export function get_text_by_size(size) {
    switch (size) {
        case "title":
            return view.h / 5;
        case "big":
            return view.h / 10;
        case "average":
            return view.h / 25;
        case "small":
            return view.h / 50;
    }
}


export function entityAtXY (obj, x, y, any) {
    for (const entity of entities)
        if (((entity.name !== obj.name) || any) && (!(x + obj.size_x < entity.pos_x ||
            y + obj.size_y < entity.pos_y || x > entity.pos_x + entity.size_x || y > entity.pos_y + entity.size_y)))
            return entity;
    return null;
}

export function inColumn (obj) {
    for (const entity of entities)
        if (!(obj.pos_x + obj.size_x < entity.pos_x || obj.pos_x > entity.pos_x + entity.size_x))
            return entity;
    return null;
}



// listeners
function queryEntities (key) {
    const new_gravity = ((key === 'w') || (key === 'ArrowUp')) ? -1 : 1;
    if (new_gravity !== gravity) {
        gravity = new_gravity;
        for (const entity of entities)
            if (((level === 1) && (entity.name === 'player')) || ((level === 2) && (entity.name !== 'player'))) {
                if (entity.fly !== 0) {
                    entity.y_speed = -entity.y_speed;
                    entity.y_speed_prev = entity.y_speed;
                }
                entity.fly = 0;
                entity.flying = true;
            }
    }
}
