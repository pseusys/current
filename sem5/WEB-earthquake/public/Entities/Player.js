const Player = Entity.extend({
    curFrame: -1,

    draw: function (ctx) {
        let sprite;
        if (gameManager.level === 1) {
            if (physicsManager.gravity === 1) sprite = this.flying ? 'dwarf_flying' : 'dwarf_running';
            else sprite = this.flying ? 'dwarf_flying_r' : 'dwarf_running_r';
            this.curFrame = (this.curFrame + 1) % (this.flying ? 35 : 40);
        } else {
            sprite = 'gargoyle';
            this.curFrame = (this.curFrame + 1) % 15;
        }

        spriteManager.drawSprite(ctx, sprite, this.pos_x + mapManager.view.x, this.pos_y,
            this.size_x, this.size_y, Math.round(this.curFrame / 5));
    },

    update: function () {
        if (gameManager.level === 2) physicsManager.trip(this);
        else physicsManager.update_player(this);
        if (physicsManager.entityAtXY(this, this.pos_x + mapManager.view.x, this.pos_y, false)) {
            eventsManager.escaped = true;
        }
    }
})
