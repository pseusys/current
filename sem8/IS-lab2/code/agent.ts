import { Interface, createInterface } from "readline"; // Подключение модуля ввода из командной строки

import { parseMsg } from "./msg"; // Подключение модуля разбора сообщений от сервера
import { Socket } from "dgram";
import { create } from "./socket";
import { Flag, Player, Point } from "./locator";
import { FIELD_X, FIELD_Y, Position, Signal } from "./constants";
import { Command } from "./command";
import { ActionFlow } from "./action";

export class Agent {
    private readonly teamName: string;
    private position: Position;
    private run: boolean;
    private act: Command | null;
    private isMoved: boolean;

    private coord: Point;
    private isLock: boolean;
    private angle: number;
    private speed: number;
    private turnSpeed: number;
    private flow: ActionFlow;

    private rl: Interface;
    private readonly socket: Socket;
    private id: number | undefined;

    constructor(team: string) {
        this.teamName = team;
        this.position = "l"; // По умолчанию - левая половина поля
        this.run = false; // Игра начата
        this.act = null; // Действия
        this.isMoved = false; // Заданы начальные координаты

        this.coord = { x: 0, y: 0, d: 0 };
        this.isLock = false;
        this.angle = 0;
        this.speed = 0;
        this.turnSpeed = 0;
        this.flow = new ActionFlow([{ act: "kick", fl: "gr" }]);

        this.socket = create(this.teamName, msg => {
            // Получение сообщения
            let data = msg.toString("utf8"); // Приведение к строке
            this.processMsg(data); // Разбор сообщения
            this.sendCmd(); // Отправка команды
        });

        // Чтение консоли
        this.rl = createInterface({ input: process.stdin, output: process.stdout });

        this.rl.on("line", input => {
            // Обработка строки из консоли
            if (!this.run && !this.isMoved) {
                // Задание начальных параметров, полученных из консоли
                const args = input.split(" ");
                if (
                    isNaN(+args[0]) ||
                    isNaN(+args[1]) ||
                    parseInt(args[0]) < -FIELD_X ||
                    parseInt(args[0]) > FIELD_X ||
                    parseInt(args[1]) < -FIELD_Y ||
                    parseInt(args[1]) > FIELD_Y
                ) {
                    this.log("Неверные аргументы!");
                    return;
                }

                const x = parseInt(args[0]);
                const y = parseInt(args[1]);
                (new Command("move", `${x} ${y}`)).send(this.socket);

                this.isMoved = true;
                this.log(`Стартовые параметры x: ${x} y: ${y}`);
            }

            if (this.run) {
                // Если игра начата
                // Движения вперед, вправо, влево, удар по мячу
                if ("w" == input) this.act = new Command("dash", 100);
                if ("d" == input) this.act = new Command("turn", 20);
                if ("a" == input) this.act = new Command("turn", -20);
                if ("s" == input) this.act = new Command("kick", 100);
            }
        });
    }

    private log(msg: string) {
        console.log(`(Player ${this.id} of team ${this.teamName}): ${msg}`);
    }

    private processMsg(msg: string) {
        // Обработка сообщения
        let data = parseMsg(msg); // Разбор сообщения
        if (!data) throw new Error("Parse error \n" + msg);
        if (data.cmd == "hear" && data.p[1] == "referee" && data.p[2] == "play_on") this.run = true; // Первое (hear) - начало игры
        if (data.cmd === "hear" && data.p[1] == "referee" && data.p[2].includes("goal")) this.flow.current = 0;
        if (data.cmd == "init") this.initAgent(data.p); // Инициализация
        this.analyzeEnv(data.cmd, data.p); // Обработка
    }

    private initAgent(p: [string, string, string]) {
        // Игрок должен быть зарегистрирован на сервере, здесь обрабатывается ответ сервера
        if (p[0] == "r") this.position = "r"; // Правая половина поля
        if (p[1]) this.id = Number(p[1]); // id игрока
    }

    private analyzeEnv(cmd: Signal, p: any) {
        // Обработка сообщений от сервера
        // Анализ сообщения
        if (cmd == "see" && this.run && this.isMoved) {
            const { seenFlags, seenPlayers, seenBall } = this.getVisible(p);

            const step = this.flow.now();
            const flagNumber = seenFlags.findIndex(flag => { return flag.name == step.fl });

            if (step.act == "flag") {
                if (flagNumber == -1 || (Math.abs(seenFlags[flagNumber].angle) >= 1)) {
                    if (flagNumber != -1) this.act = new Command("turn", -(seenFlags[flagNumber].angle));
                    else this.act = new Command("turn", 10);
                } else {
                    if (seenFlags[flagNumber].distance <= 3.0) {
                        this.flow.current++;
                        this.act = new Command("dash", 50);
                    } else this.act = new Command("dash", 100);
                }
            } else if (step.act == "kick") {
                if (seenBall == null) this.act = new Command("turn", 10);
                else if (Math.abs(seenBall.angle) >= 1) this.act = new Command("turn", seenBall.angle);
                else if (Math.abs(seenBall.distance) >= 0.5) this.act = new Command("dash", 100);
                else if (flagNumber != -1) this.act = new Command("kick", `100 ${seenFlags[flagNumber].angle}`);
                else this.act = new Command("kick", "10 -45");
            }
        }
    }

    private getVisible(p: any): { seenFlags: Flag[], seenPlayers: Player[], seenBall: Player | null } {
        const seenFlags: Flag[] = [];
        const seenPlayers: Player[] = [];
        let seenBall: Player | null = null;

        // Пройдемся по res.p из app.ts
        for (let i = 1; i < p.length; ++i) {
            // Если есть "cmd": { "р": [.............] } и есть угол и расстояние
            if (p[i].cmd.p.length >= 0 && p[i].p.length >= 2) {
                // Если игрок видит флаг
                // Запоминаем флаги в формате {название, расстояние, угол}
                if (p[i].cmd.p[0] == "f" || p[i].cmd.p[0] == "g") seenFlags.push({ name: p[i].cmd.p.join(""), distance: p[i].p[0], angle: p[i].p[1] });
                // Если игрок видит игрока и есть угол и расстояние
                else if (p[i].cmd.p[0] == "p" && p[i].cmd.p[1] != undefined && p[i].cmd.p[0] != undefined) seenPlayers.push({ distance: p[i].p[0], angle: p[i].p[1] });
                else if (p[i].cmd.p[0] === "b") seenBall = { distance: p[i].p[0], angle: p[i].p[1] }
            }
        }
        return { seenFlags, seenPlayers, seenBall };
    }

    private sendCmd() {
        //отправляет следующую команду на сервер
        if (this.run) {
            //Игра начата
            this.act?.send(this.socket); //Есть команда от игрока
            this.act = null;
        }
    }
}
