const gameManager = {
    factory: {},
    entities: [],
    player: null,
    walls_rem: 8,
    level: 0,
    proc: undefined,

    primes: [1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109],

    loadAll: function () {
        this.factory = {};
        this.entities = [];
        this.player = null;
        this.walls_rem = 8;
        this.proc = undefined;

        soundManager.init();
        mapManager.init();
        spriteManager.init();

        let track;
        if (this.level === 1) {
            track = 'cave_track.mp3';
            mapManager.loadMap('cave.json')
            spriteManager.loadAtlas('cave_sprites.json', 'cave_sprites.png')
        } else {
            track = 'pass_track.mp3';
            mapManager.loadMap('pass.json')
            spriteManager.loadAtlas('pass_sprites.json', 'pass_sprites.png')
        }
        soundManager.loadArray([/*'hit.mp3', */'loss.mp3', track]);

        gameManager.factory.Player = Player // инициализация фабрики
        gameManager.factory.Wall1 = Object.create(Wall)
        gameManager.factory.Wall2 = Object.create(Wall)
        if (this.level === 1) gameManager.factory.Wall3 = Object.create(Wall)

        mapManager.parseEntities()
        mapManager.draw(ctx)
        eventsManager.setup()
        physicsManager.init();
    },

    initPlayer: function (obj) {
        this.player = obj;
        gameManager.entities.push(obj)
    },

    play: function (level) {
        this.level = level;
        gameManager.loadAll();
        if (this.level === 1) soundManager.play('cave_track.mp3', true, 0.75);
        else soundManager.play('pass_track.mp3', true, 0.75);
        this.proc = setInterval(updateWorld, 15);
    },

    triple: { l: 0, h: 0 },
    duple: { l: 0, h: 0 },

    fix_offsets: function (diff) {
        this.triple.l -= diff;
        this.triple.h -= diff;
        this.duple.l -= diff;
        this.duple.h -= diff;
    },

    emit_wall: function () {
        const index = this.primes.length - 1 - Math.round(mapManager.world_run / 5)

        const add_wall = (cre, x, lower) => {
            let wall;
            if (cre < 10 && (this.level !== 2)) {
                if (((mapManager.world_offset- this.triple.l < mapManager.tileSize * 10) ||
                    (mapManager.world_offset - this.duple.l < mapManager.tileSize * 7)) && !lower) return false;
                if (((mapManager.world_offset - this.triple.h < mapManager.tileSize * 10) ||
                    (mapManager.world_offset - this.duple.h < mapManager.tileSize * 7)) && lower) return false;
                wall = Object.create(gameManager.factory.Wall3);
                if (lower) this.triple.l = mapManager.world_offset;
                else this.triple.h = mapManager.world_offset;
            } else if (cre < 30) {
                if (((mapManager.world_offset - this.duple.l < mapManager.tileSize * 5) ||
                    (mapManager.world_offset - this.triple.l < mapManager.tileSize * 7)) && !lower) return false;
                if (((mapManager.world_offset - this.duple.h < mapManager.tileSize * 5) ||
                    (mapManager.world_offset - this.triple.h < mapManager.tileSize * 7)) && lower) return false;
                wall = Object.create(gameManager.factory.Wall2);
                if (lower) this.duple.l = mapManager.world_offset;
                else this.duple.h = mapManager.world_offset;
            } else {
                wall = Object.create(gameManager.factory.Wall1);
            }
            if (!lower) wall.turned = Math.PI;
            wall.screen_offset = x;
            wall.pos_y = lower ? (mapManager.floor.l - wall.size_y) : (mapManager.floor.h);
            wall.update();

            if (!physicsManager.entityAtXY(wall, wall.pos_x, wall.pos_y, true) && (!(this.level === 2) || !physicsManager.inColumn(wall))) {
                this.entities.push(wall);
                return true;
            } else return false;
        }

        const random = Math.round(Math.random() * mapManager.view.w);
        if ((random % this.primes[index >= 0 ? index : 0] === 0) && this.walls_rem > 0) {
            const pos_x = mapManager.view.w + mapManager.view.x + mapManager.tileSize;
            const creator = Math.round(Math.random() * 100);
            if (add_wall(creator, pos_x, creator % 2 === 0)) this.walls_rem--;
        }
    },

    kill: function (obj) {
        const idx = this.entities.indexOf(obj)
        if (idx > -1) {
            this.entities.splice(idx, 1);
            this.walls_rem++;
        }
    },

    update: function () {
        if (this.player === null) return
        if (eventsManager.action === 'esc') {
            this.end_game(ctx);
            return;
        }

        physicsManager.update_world();

        // обновление информации по всем объектам на карте
        this.entities.forEach(function (e) {
            try {
                e.update()
            } catch (ex) {
                console.log(e.name + ' ' + JSON.stringify(ex))
            }
        });
        eventsManager.consume();

        this.emit_wall();

        mapManager.draw(ctx) // Перерисовываем карту
        this.draw(ctx) // Перерисовываем объекты

        ctx.fillStyle = "white";
        const text = "Score " + physicsManager.count_offset().toFixed(3);
        ctx.font = physicsManager.get_text_by_size("small") + "px serif";
        ctx.fillText(text, mapManager.view.x + mapManager.view.w / 2, mapManager.tileSize / 2);
        const prompt = "Use 'W', 'S' to change gravity, 'Esc' to quit, 'Space' to toggle sound";
        ctx.fillText(prompt, mapManager.view.x + mapManager.view.w / 2, mapManager.floor.l + mapManager.tileSize / 2);
    },

    draw: function (ctx) {
        for (const entity of this.entities) {
            entity.draw(ctx);
        }
    },

    end_game: function () {
        eventsManager.finish();
        clearInterval(this.proc);
        this.proc = undefined;

        const score = Math.round(physicsManager.count_offset() * 1000) / 1000;
        const text = "Alas! You reached only " + score.toFixed(3) + " meters!";

        ctx.fillStyle = "rgba(175, 175, 175, 0.5)";
        ctx.fillRect(mapManager.view.x, mapManager.view.y, mapManager.view.w, mapManager.view.h);
        ctx.font = physicsManager.get_text_by_size("average") + "px serif";
        ctx.fillStyle = "black";
        ctx.fillText(text, mapManager.view.x + mapManager.view.w / 2, mapManager.view.h / 2);

        push_score(this.level === 1 ? 'Dwarf' : 'Gargoyle', score);
        soundManager.play('loss.mp3', false, 0.75);
    }
}

function updateWorld () {
    gameManager.update()
}
