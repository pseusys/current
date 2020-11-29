const Wall = Entity.extend({
    screen_offset: 0,
    turned: 0,

    draw: function (ctx) {
        spriteManager.drawSpriteRotated(ctx, this.name, this.pos_x, this.pos_y, this.size_x, this.size_y, 0, this.turned);
    },

    update: function () {
        physicsManager.update_npc(this);
        if (gameManager.level === 2) physicsManager.update_player(this);
        if (this.pos_x + this.size_x < mapManager.view.x) {
            gameManager.kill(this);
        }
    }
})
