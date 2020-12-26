import { kill, level } from './game_manager.js';
import { drawSprite, drawSpriteRotated } from './sprite_manager.js';
import { view } from './map_manager.js';
import { entityAtXY, gravity, trip, update_npc, update_player } from './physics_manager.js';
import { escape } from './event_manager.js';


// declarations
export class Entity {
    constructor (name, size_x, size_y) {
        this.name = name;

        this.pos_x = 0;
        this.pos_y = 0;

        this.size_x = size_x;
        this.size_y = size_y;

        this.y_speed = 0;
        this.y_speed_prev = 0;
        this.y_speedup = 9.81;

        this.fly = 0;
        this.flying = false;
    }
}

export class Player extends Entity {
    constructor (name, pos_x, pos_y, size_x, size_y) {
        super(name, size_x, size_y);
        this.pos_x = pos_x;
        this.pos_y = pos_y;
        this.cur_frame = -1;
    }

    draw (ctx) {
        let sprite;
        if (level === 1) {
            if (gravity === 1) sprite = this.flying ? 'dwarf_flying' : 'dwarf_running';
            else sprite = this.flying ? 'dwarf_flying_r' : 'dwarf_running_r';
            this.cur_frame = (this.cur_frame + 1) % (this.flying ? 35 : 40);
        } else {
            sprite = 'gargoyle';
            this.cur_frame = (this.cur_frame + 1) % 15;
        }

        drawSprite(ctx, sprite, this.pos_x + view.x, this.pos_y,
            this.size_x, this.size_y, Math.round(this.cur_frame / 5));
    }

    update () {
        if (level === 2) trip(this);
        else update_player(this);
        if (entityAtXY(this, this.pos_x + view.x, this.pos_y, false)) escape();
    }
}

export class Wall extends Entity {
    constructor (name, size_x, size_y) {
        super(name, size_x, size_y);
        this.screen_offset = 0;
        this.turned = 0;
    }

    draw (ctx) {
        drawSpriteRotated(ctx, this.name, this.pos_x, this.pos_y, this.size_x, this.size_y, 0, this.turned);
    }

    update () {
        update_npc(this);
        if (level === 2) update_player(this);
        if (this.pos_x + this.size_x < view.x) { kill(this); }
    }
}
