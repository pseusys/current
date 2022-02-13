import { Interface, createInterface } from "readline"; // Подключение модуля ввода из командной строки

import { parseMsg } from "./msg"; // Подключение модуля разбора сообщений от сервера
import { Socket } from "dgram";
import { create } from "./socket";
import { biangulate, distBetween, Flag, Player, toPoint } from "./utils";

type Command = "move" | "hear" | "init" | "see" | "turn";
type Position = "l" | "r";

export class Agent {
    private readonly teamName: string;
    private position: Position;
    private run: boolean;
    private act: any;
    private isMoved: boolean;
    private turnSpeed: number;

    private rl: Interface;
    private socket: Socket;
    private id: number | undefined;

    constructor(team: string) {
        this.teamName = team;
        this.position = "l"; // По умолчанию - левая половина поля
        this.run = false; // Игра начата
        this.act = null; // Действия
        this.isMoved = false; // Заданы начальные координаты
        this.turnSpeed = 0; // Скорость вращения  (поворот на каждый такт)

        this.socket = create(this.teamName, msg => {
            this.msgGot(msg); //Обработка полученного сообщения
        });

        // Чтение консоли
        this.rl = createInterface({ input: process.stdin, output: process.stdout });

        this.rl.on("line", input => {
            // Обработка строки из консоли
            if (!this.run && !this.isMoved) {
                // Задание начальных параметров, полученных из консоли
                const args = input.split(" ");
                if (
                    args[0] ||
                    args[1] ||
                    args[2] ||
                    isNaN(+args[0]) ||
                    isNaN(+args[1]) ||
                    isNaN(+args[2]) ||
                    parseInt(args[0]) < -54 ||
                    parseInt(args[0]) > 54 ||
                    parseInt(args[1]) < -32 ||
                    parseInt(args[1]) > 32 ||
                    parseInt(args[2]) < -180 ||
                    parseInt(args[2]) > 180
                ) {
                    console.log("Неверные аргументы!");
                    return;
                }

                const x = parseInt(args[0]);
                const y = parseInt(args[1]);
                const turnSpeed = parseInt(args[2]);

                this.isMoved = true;
                this.turnSpeed = turnSpeed;
                this.socketSend("move", `${x} ${y}`); // поворот
                this.act = { n: "turn", v: this.turnSpeed };
                console.log(`Стартовые параметры x: ${x} y: ${y} turnSpeed: ${turnSpeed}`);
            }

            if (this.run) {
                // Если игра начата
                // Движения вперед, вправо, влево, удар по мячу
                if ("w" == input) this.act = { n: "dash", v: 100 };
                if ("d" == input) this.act = { n: "turn", v: 20 };
                if ("a" == input) this.act = { n: "turn", v: -20 };
                if ("s" == input) this.act = { n: "kick", v: 100 };
            }
        });
    }

    msgGot(msg: Buffer) {
        // Получение сообщения
        let data = msg.toString("utf8"); // Приведение к строке
        this.processMsg(data); // Разбор сообщения
        this.sendCmd(); // Отправка команды
    }

    socketSend(cmd: Command, value: any) {
        // Отправка команды
        this.socket?.sendMsg(`(${cmd} ${value})`);
    }

    processMsg(msg: string) {
        // Обработка сообщения
        let data = parseMsg(msg); // Разбор сообщения
        if (!data) throw new Error("Parse error \n" + msg);
        console.log(data.cmd);
        if (data.cmd == "hear") this.run = true; // Первое (hear) - начало игры
        if (data.cmd == "init") this.initAgent(data.p); // Инициализация
        this.analyzeEnv(data.cmd, data.p); // Обработка
    }

    initAgent(p: [string, string, string]) {
        // Игрок должен быть зарегистрирован на сервере, здесь обрабатывается ответ сервера
        if (p[0] == "r") this.position = "r"; // Правая половина поля
        if (p[1]) this.id = Number(p[1]); // id игрока
    }

    analyzeEnv(cmd: Command, p: any) {
        // Обработка сообщений от сервера
        // Анализ сообщения
        if (cmd == "see"/* && this.run && this.isMoved*/) {
            const seenFlags: Flag[] = [];
            const seenPlayers: Player[] = [];

            // Пройдемся по res.p из app.ts
            for (let i = 1; i < p.length; ++i) {
                // Если есть "cmd": { "р": [.............] } и есть угол и расстояние
                if (p[i].cmd.p.length >= 0 && p[i].p.length >= 2) {
                    // Если игрок видит флаг
                    // Запоминаем флаги в формате {название, расстояние, угол}
                    if (p[i].cmd.p[0] == "f") seenFlags.push({ name: p[i].cmd.p.join(""), distance: p[i].p[0], angle: p[i].p[1] });
                    // Если игрок видит игрока и есть угол и расстояние
                    else if (p[i].cmd.p[0] == "p" && p[i].cmd.p[1] != undefined && p[i].cmd.p[0] != undefined) seenPlayers.push({ distance: p[i].p[0], angle: p[i].p[1] });
                }
            }

            const flags = [toPoint(seenFlags[0]), toPoint(seenFlags[1]), toPoint(seenFlags[2])];
            const res = biangulate(flags[0], flags[1], flags[2]);
            console.log(`(Player ${this.id} of team ${this.teamName}): I am at coords (x: ${res.x}, y: ${res.y})`);
            if (seenPlayers.length > 0) {
                const end1 = distBetween(flags[0], seenFlags[0].angle, res, seenPlayers[0].angle);
                const end2 = distBetween(flags[1], seenFlags[1].angle, res, seenPlayers[0].angle);
                const enemyRes = biangulate(
                    { x: res.x, y: res.y, d: seenPlayers[0].distance },
                    { x: flags[0].x, y: flags[0].y, d: end1 },
                    { x: flags[1].x, y: flags[1].y, d: end2 }
                );
                console.log(`(Player ${this.id} of team ${this.teamName}): Enemy at coords (x: ${enemyRes.x}, y: ${enemyRes.y})`);
            }
        }
    }

    degrees_to_radians(degrees: number) {
        return degrees * (Math.PI / 180);
    }

    enemyCoord(seenPlayers: Player[], x: number, y: number) {
        let resX = null;
        let resY = null;
        if (seenPlayers.length > 0) {
            resX = Math.sin(this.degrees_to_radians(seenPlayers[0].angle)) * seenPlayers[0].distance - y;
            resY = Math.cos(this.degrees_to_radians(seenPlayers[0].angle)) * seenPlayers[0].distance + x;
        }
        return { x: resX, y: resY };
    }

    sendCmd() {
        //отправляет следующую команду на сервер
        if (this.run) {
            //Игра начата
            //this.socketSend("turn", this.turnSpeed);
            if (this.act) this.socketSend(this.act.n, this.act.v); //Есть команда от игрока
            this.act = null;
        }
    }
}
