import { parseMsg } from "./msg"; // Подключение модуля разбора сообщений от сервера
import { Socket } from "dgram";
import { create } from "./socket";
import { Coordinate, getVisible } from "./locator";
import { FIELD_X, FIELD_Y, Position, Signal } from "./constants";
import { Command } from "./command";
import { kickerRoot, kickerState } from "./actions_kicker";
import { passerRoot, passerState } from "./actions_passer";
import { reset_player } from "./positioner";
import { DecisionTree } from "./actions";
import { TeamName } from "./app";

export type Role = "goalie" | "player";

export class Agent {
    readonly teamName: TeamName;
    private position: Position;
    private readonly role: Role;
    private run: boolean;
    private act: Command | null;
    private dt: DecisionTree | undefined;

    private readonly socket: Socket;
    id: number | undefined;
    artefact: number;

    constructor(team: TeamName, artefact: number, position: Position, coordinate: Coordinate, role: Role) {
        this.teamName = team;
        this.artefact = artefact;
        this.role = role;
        this.position = position;
        this.run = false; // Игра начата
        this.act = null; // Действия

        this.socket = create(this.teamName, msg => {
            // Получение сообщения
            let data = msg.toString("utf8"); // Приведение к строке
            this.processMsg(data); // Разбор сообщения
            this.sendCmd(); // Отправка команды
        }, role == "goalie");

        if (
            isNaN(coordinate.x) ||
            isNaN(coordinate.y) ||
            coordinate.x < -FIELD_X ||
            coordinate.x > FIELD_X ||
            coordinate.y < -FIELD_Y ||
            coordinate.y > FIELD_Y
        ) this.log("Неверные аргументы!");
        else (new Command("move", `${coordinate.x} ${coordinate.y}`)).send(this.socket)
    }

    private log(msg: string) {
        console.log(`(Player ${this.id} of team ${this.teamName}): ${msg}`);
    }

    private processMsg(msg: string) {
        // Обработка сообщения
        let data = parseMsg(msg); // Разбор сообщения
        if (!data) throw new Error("Parse error \n" + msg);
        if (data.cmd == "hear" && data.p[1] == "referee" && data.p[2].includes("play_on")) this.run = true; // Первое (hear) - начало игры
        if (data.cmd === "hear" && data.p[1] == "referee" && data.p[2].includes("goal")) {
            const coords = reset_player(this);
            (new Command("move", `${coords.x} ${coords.y}`)).send(this.socket);
            this.run = false;
            this.dt!!.reset();
        }
        if (data.cmd == "init") this.initAgent(data.p); // Инициализация
        this.analyzeEnv(data.cmd, data.p); // Обработка
    }

    private initAgent(p: [string, string, string]) {
        // Игрок должен быть зарегистрирован на сервере, здесь обрабатывается ответ сервера
        if (p[0] == "r") this.position = "r"; // Правая половина поля
        else this.position = "l";
        if (p[1]) {
            this.id = Number(p[1]);
            if (this.id == 1) this.dt = new DecisionTree(kickerRoot, kickerState(), this);
            else this.dt = new DecisionTree(passerRoot, passerState(), this);
        } // id игрока
    }

    private analyzeEnv(cmd: Signal, p: any) {
        // Обработка сообщений от сервера
        // Анализ сообщения
        if (this.run && (this.teamName == "team1")) {
            if (cmd == "hear") this.dt!!.param("signal", p[2].includes("go"));
            if (cmd == "see") this.act = this.dt!!.run(p);
        }
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
