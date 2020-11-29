const mapManager = {
    mapData: null, // Информация о карте
    tLayer: null, // Текущий тайл
    xCount: 0,
    yCount: 0,
    imgLoadCount: 0, // количество загруженных изображений
    imgLoaded: false, // изображения не загружены
    jsonLoaded: false, // json не загружен
    tilesets: [], // Наборы тайлов
    view: { x: 0, y: 0, w: window.innerWidth, h: window.innerHeight }, // Видимая часть

    tSize: { x: 32, y: 32 }, // Размер тайла
    mSize: { x: 8, y: 16 }, // Размер карты

    tileSize: 135,
    mapSize: { x: 1080, y: 2160 },
    floor: { l: 945, h: 135 },

    world_speed: 32,
    world_speedup: 0.001,
    world_run: 0,
    world_offset: 0,

    init: function () {
        this.mapData = null;
        this.tLayer = null;
        this.xCount = 0;
        this.yCount = 0;
        this.imgLoadCount = 0;
        this.imgLoaded = false;
        this.jsonLoaded = false;
        this.tilesets = [];
        this.view = { x: 0, y: 0, w: window.innerWidth, h: window.innerHeight };

        this.tSize = { x: 32, y: 32 };
        this.mSize = { x: 8, y: 16 };

        this.tileSize = 135;
        this.mapSize = { x: 1080, y: 2160 };
        this.floor = { l: 945, h: 135 };

        this.world_speed = 32;
        this.world_speedup = 0.001;
        this.world_run = 0;
        this.world_offset = 0;
    },

    loadMap: function (path) {
        const request = new XMLHttpRequest()
        request.onreadystatechange = function () {
            if (request.readyState === 4 && request.status === 200) {
                mapManager.parseMap(request.responseText)
            }
        }
        request.open('GET', "/files?name=" + path, true)
        request.send()
    },

    parseMap: function (tilesJSON) {
        this.mapData = JSON.parse(tilesJSON)
        this.xCount = this.mapData.width
        this.yCount = this.mapData.height
        this.tSize.x = this.mapData.tilewidth
        this.tSize.y = this.mapData.tileheight
        this.tileSize = Math.ceil(window.innerHeight / this.yCount)
        this.mSize.x = this.xCount * this.tSize.x
        this.mSize.y = this.yCount * this.tSize.y

        this.mapSize.x = this.xCount * this.tileSize
        this.mapSize.y = this.yCount * this.tileSize

        this.floor.l = this.mapSize.y - this.tileSize;
        this.floor.h = this.tileSize;

        for (let i = 0; i < this.mapData.tilesets.length; i++) {
            let img = new Image()
            img.onload = function () {
                mapManager.imgLoadCount++
                if (mapManager.imgLoadCount === mapManager.mapData.tilesets.length) {
                    mapManager.imgLoaded = true
                }
            }
            img.src = '/files?name=' + this.mapData.tilesets[i].image
            const t = this.mapData.tilesets[i]
            const ts = {
                firstgid: t.firstgid,
                image: img,
                name: t.name,
                xCount: Math.floor(t.imagewidth / this.mapData.tilewidth),
                yCount: Math.floor(t.imageheight / this.mapData.tileheight)
            }
            this.tilesets.push(ts)
        }
        this.jsonLoaded = true
    },

    draw: function (ctx) {
        // ctx.rect(this.view.x, this.view.y, this.view.w, this.view.h)
        // ctx.fillStyle = 'rgba(0,0,0,0.63)'
        // ctx.fill()
        // если карта не загружена, то повторить прорисовку через 100 мс
        if (!mapManager.imgLoaded || !mapManager.jsonLoaded) {
            setTimeout(function () {
                mapManager.draw(ctx)
            }, 100)
        } else {
            const layerCount = 0
            if (this.tLayer === null) {
                // проверка, что tLayer настроен
                for (let id = 0; id < this.mapData.layers.length; id++) {
                    // проходим по всем layer карты
                    const layer = this.mapData.layers[id]
                    if (layer.type === 'tilelayer') {
                        this.tLayer = layer
                        // break;
                    }
                }
            }
            for (let i = 0; i < this.tLayer.data.length; i++) {
                // проходим по всей карте
                if (this.tLayer.data[i] !== 0) {
                    // если данных нет, то пропускаем
                    const tile = this.getTile(this.tLayer.data[i]) // получение блока по индексу
                    let pX = (i % this.xCount) * this.tileSize // вычисляем x в пикселях
                    let pY = Math.floor(i / this.xCount) * this.tileSize // не рисуем за пределами видимой зоны
                    if (!this.isVisible(pX, pY, this.tileSize, this.tileSize)) continue;
                    ctx.drawImage(
                        tile.img,
                        tile.px,
                        tile.py,
                        this.tSize.x,
                        this.tSize.y,
                        pX,
                        pY,
                        this.tileSize,
                        this.tileSize
                    ) // отрисовка в контексте
                }
            }
        }
    },

    getTile (tileIndex) {
        const tile = {
            img: null,
            px: 0,
            py: 0
        }
        const tileset = this.getTileset(tileIndex)
        tile.img = tileset.image
        const id = tileIndex - tileset.firstgid
        const x = id % tileset.xCount
        const y = Math.floor(id / tileset.xCount)
        tile.px = x * this.tSize.x
        tile.py = y * this.tSize.y
        return tile
    },

    getTileset (tileIndex) {
        for (let i = this.tilesets.length - 1; i >= 0; i--) {
            if (this.tilesets[i].firstgid <= tileIndex) {
                return this.tilesets[i]
            }
        }
        return null
    },

    // Так же для того чтобы отрисовывать не всю карту, а только её видимую
    // часть (созданное поле view) создадим функцию isVisible для проверки
    // нахождения объекта в области видимости.
    isVisible: function (x, y, width, height) {
        // не рисуем за пределами видимой зоны
        return !(
            x + width < this.view.x ||
            y + height < this.view.y ||
            x > this.view.x + this.view.w ||
            y > this.view.y + this.view.h
        )
    },

    spread: function (x, ctx) {
        this.view.x += x;
        this.world_offset += x;
        ctx.translate(-x, 0);
        if (this.view.x > this.mapSize.x / 2) {
            const limit = this.view.x - this.mapSize.x / 2;
            ctx.translate(this.view.x, 0);
            gameManager.fix_offsets(this.view.x);
            this.view.x = 0;
            this.spread(limit, ctx);
        }
    },

    parseEntities: function () {
        if (!mapManager.imgLoaded || !mapManager.jsonLoaded) {
            setTimeout(function () {
                mapManager.parseEntities()
            }, 100)
        } else {
            for (let j = 0; j < this.mapData.layers.length; j++) {
                if (this.mapData.layers[j].type === 'objectgroup') {
                    const entities = this.mapData.layers[j]
                    for (let i = 0; i < entities.objects.length; i++) {
                        const e = entities.objects[i]
                        try {
                            if (e.name === 'player') {
                                const obj = Object.create(gameManager.factory[e.type])
                                obj.pos_x = this.tileSize;
                                obj.pos_y = gameManager.level === 2 ?
                                    (this.floor.h + this.floor.l - this.tileSize) / 2 :
                                    this.floor.l - this.tileSize;
                                obj.size_x = this.tileSize
                                obj.size_y = this.tileSize
                                gameManager.initPlayer(obj);
                            } else {
                                const fabric = gameManager.factory[e.type]
                                fabric.name = e.name
                                fabric.size_x = e.width / this.tSize.x * this.tileSize
                                fabric.size_y = e.height / this.tSize.y * this.tileSize
                            }
                        } catch (ex) {
                            console.log('Ошибка создания: [' + e.type + ']' + e.type + ',' + ex)
                        }
                    }
                }
            }
        }
    }
}

// module.exports = mapManager
