const physicsManager = {
    gravity: 1,
    dpc_y: 0,

    init: function () {
        this.gravity = 1;
        this.dpc_y = 0;
        this.dpc_y = document.getElementById('dpi').offsetHeight;
    },

    update_player: function (player) {
        if (eventsManager.actionChanged) {
            if (eventsManager.action === 'up') this.gravity = -1;
            if (eventsManager.action === 'down') this.gravity = 1;
            if (player.fly !== 0) {
                player.y_speed = -player.y_speed;
                player.y_speed_prev = player.y_speed;
            }
            player.fly = 0;
            player.flying = true;
        }

        if (player.flying) { // 100 times slower than real
            player.fly += 0.015;
            const new_speed = player.y_speed_prev + player.y_speedup * player.fly;
            const new_coord = player.pos_y + this.gravity * new_speed * 0.015 * this.dpc_y;

            if (new_coord > mapManager.floor.l - player.size_y) {
                player.pos_y = mapManager.floor.l - player.size_y;
                if (this.gravity === 1) player.flying = false;
                player.y_speed = player.y_speed_prev = player.fly = 0;
            } else if (new_coord < mapManager.floor.h) {
                player.pos_y = mapManager.floor.h;
                if (this.gravity === -1) player.flying = false;
                player.y_speed = player.y_speed_prev = player.fly = 0;
            } else {
                player.y_speed = new_speed;
                player.pos_y = new_coord;
            }
        }

        player.y_speedup += 0.0001;
    },

    update_npc: function (npc) {
        npc.screen_offset -= mapManager.world_speed * 0.015;
        npc.pos_x = npc.screen_offset + mapManager.view.x;
    },

    update_world: function () {
        mapManager.world_run += 0.015;
        mapManager.world_speed += mapManager.world_speedup * mapManager.world_run;
        mapManager.spread(mapManager.world_speed * 0.015, ctx);
    },

    trip: function (player) {
        const rout = mapManager.world_speed * 0.015;
        const rand = player.pos_y + Math.random() * rout * 2 - rout;
        if (rand > mapManager.floor.l - player.size_y) {
            player.pos_y = mapManager.floor.l - player.size_y;
        } else if (rand < mapManager.floor.h) {
            player.pos_y = mapManager.floor.h;
        } else {
            player.pos_y = rand;
        }
    },

    count_offset: function () {
        return mapManager.world_offset / this.dpc_y / 100;
    },

    get_text_by_size(size) {
        switch (size) {
            case "title":
                return mapManager.view.h / 5;
            case "big":
                return mapManager.view.h / 10;
            case "average":
                return mapManager.view.h / 25;
            case "small":
                return mapManager.view.h / 50;
        }
    },

    // Проверить коллизии с объектом obj
    entityAtXY: function (obj, x, y, any) {
        for (let i = 0; i < gameManager.entities.length; i++) {
            const e = gameManager.entities[i]
            if ((e.name !== obj.name) || any) {
                if (
                    x + obj.size_x < e.pos_x ||
                    y + obj.size_y < e.pos_y ||
                    x > e.pos_x + e.size_x ||
                    y > e.pos_y + e.size_y
                ) {
                    continue
                }
                return e
            }
        }
        return null
    },

    inColumn: function (obj) {
        for (let i = 0; i < gameManager.entities.length; i++) {
            const e = gameManager.entities[i]
            if (
                obj.pos_x + obj.size_x < e.pos_x ||
                obj.pos_x > e.pos_x + e.size_x
            ) {
                continue
            }
            return e
        }
        return null
    }
}

/*
update: function (player) {
        if (eventsManager.action !== '') {
            if (eventsManager.action === 'up') this.gravity = -1;
            if (eventsManager.action === 'down') this.gravity = 1;
            player.falling = true;
        }

        if (player.falling) {
            player.y_speed += this.gravity * 0.030;
            const new_y = player.pos_y + player.y_speed;

            if (new_y > mapManager.floor.l) {
                player.pos_y = mapManager.floor.l;
                if (this.gravity === 1) player.falling = false;
                else player.y_speed = 0;
            } else if (new_y < mapManager.floor.h) {
                player.pos_y = mapManager.floor.h;
                if (this.gravity === -1) player.falling = false;
                else player.y_speed = 0;
            } else player.pos_y = new_y;

            if (!player.falling) player.y_speed = 0;
            eventsManager.action = '';
        }
    },
 */
