const spriteManager = {
    image: new Image(),
    sprites: [],
    imgLoaded: false,
    jsonLoaded: false,

    init: function () {
        this.image = new Image();
        this.sprites = [];
        this.imgLoaded = false;
        this.jsonLoaded = false;
    },

    loadImg: function (imgName) {
        this.image.onload = function () {
            spriteManager.imgLoaded = true
        }
        this.image.src = '/files?name=' + imgName
        this.image.id = imgName
    },

    loadAtlas: function (atlasJson, atlasImg) {
        const request = new XMLHttpRequest()
        request.onreadystatechange = function () {
            if (request.readyState === 4 && request.status === 200) {
                spriteManager.parseAtlas(request.responseText)
            }
        }
        request.open('GET', '/files?name=' + atlasJson, true)
        request.send()
        this.loadImg(atlasImg)
    },

    parseAtlas: function (atlasJSON) {
        const atlas = JSON.parse(atlasJSON)
        for (const i in atlas) {
            const frame = atlas[i];
            this.sprites.push({
                name: frame.name,
                x: frame.x,
                y: frame.y,
                w: frame.width,
                h: frame.height
            }) // сохранение характеристик frame в виде объекта
        }

        this.jsonLoaded = true // атлас разобран
    },

    drawSprite: function (ctx, name, x, y, w, h, curFrame) {
        // если изображение не загружено, то повторить запрос через 100 мс
        if (!this.imgLoaded || !this.jsonLoaded) {
            setTimeout(function () {
                spriteManager.drawSprite(ctx, name, x, y, w, h, curFrame)
            }, 100)
        } else {
            if (!mapManager.isVisible(x, y, w, h)) return // не рисуем за пределами видимой зоны
            const sprite = this.getSprite(name) // получить спрайт по имени
            ctx.drawImage(this.image,
                sprite.x + curFrame * mapManager.tSize.x, sprite.y,
                mapManager.tSize.x, mapManager.tSize.y,
                x, y, w, h)
        }
    },

    drawSpriteRotated: function (ctx, name, x, y, w, h, curFrame, angle) {
        // если изображение не загружено, то повторить запрос через 100 мс
        if (!this.imgLoaded || !this.jsonLoaded) {
            setTimeout(function () {
                spriteManager.drawSprite(ctx, name, x, y, w, h, curFrame)
            }, 100)
        } else {
            if (!mapManager.isVisible(x, y, w, h)) return // не рисуем за пределами видимой зоны
            const sprite = this.getSprite(name) // получить спрайт по имени
            const tx = x + w / 2;
            const ty = y + h / 2;
            ctx.translate(tx, ty);
            ctx.rotate(angle);
            ctx.drawImage(this.image,
                sprite.x + curFrame * mapManager.tSize.x, sprite.y,
                mapManager.tSize.x, mapManager.tSize.y * (h / mapManager.tileSize),
                -w / 2, -h / 2, w, h)
            ctx.rotate(-angle);
            ctx.translate(-tx, -ty);
        }
    },

    getSprite: function (name) {
        for (let i = 0; i < this.sprites.length; i++) {
            const s = this.sprites[i]
            if (s.name === name) {
                return s
            }
        }
        return null // не нашли спрайт
    }
}
