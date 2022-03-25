import { parseMsg } from "./utils/msg";
import { Socket } from "dgram";
import { create } from "./utils/socket";
import { FIELD_X, FIELD_Y, Position } from "./utils/constants";
import { Command } from "./utils/command";
import { reset_player } from "./utils/positioner";
import { TeamName } from "./utils/app";
import { Coordinate } from "./automaton/locator";
import { Automaton } from "./automaton/automaton";
import { GoalieAutomaton } from "./automaton/goalie_auto";
import { KickerAutomaton } from "./automaton/kicker_auto";

export type Role = "goalie" | "player";

export class Agent {
    readonly teamName: TeamName;
    private position: Position;
    private readonly role: Role;
    private run: boolean;
    private act: Command | null;

    private auto: Automaton | undefined;

    private readonly socket: Socket;
    id: number | undefined;
    artefact: number;

    constructor(team: TeamName, artefact: number, position: Position, coordinate: Coordinate, role: Role) {
        this.teamName = team;
        this.artefact = artefact;
        this.role = role;
        this.position = position;
        this.run = false;
        this.act = null;

        this.socket = create(this.teamName, msg => {
            this.processMsg(msg.toString("utf8"));
            this.sendCmd();
        }, role == "goalie");

        if (isNaN(coordinate.x) || isNaN(coordinate.y) || coordinate.x < -FIELD_X || coordinate.x > FIELD_X || coordinate.y < -FIELD_Y || coordinate.y > FIELD_Y) this.log("Неверные аргументы!");
        else (new Command("move", `${coordinate.x} ${coordinate.y}`)).send(this.socket);
        setTimeout(() => { if (position == "r") (new Command("turn", 180)).send(this.socket) }, 150);
    }

    private log(msg: string) {
        console.log(`(Player ${this.id} of team ${this.teamName}): ${msg}`);
    }

    private processMsg(msg: string) {
        let data = parseMsg(msg);
        if (!data) throw new Error("Parse error \n" + msg);
        if (data.cmd == "hear" && data.p[1] == "referee" && data.p[2].includes("play_on")) this.run = true;
        if (data.cmd === "hear" && data.p[1] == "referee" && data.p[2].includes("goal")) {
            const coords = reset_player(this);
            (new Command("move", `${coords.x} ${coords.y}`)).send(this.socket);
            this.run = false;
        }
        if (data.cmd == "init") this.initAgent(data.p);
        if (this.run) this.act = this.auto!!.getCommand(data);
    }

    private initAgent(p: [string, string, string]) {
        if (p[0] == "r") this.position = "r";
        else this.position = "l";
        if (p[1]) {
            this.id = Number(p[1]);
            if (this.role == "goalie") this.auto = new GoalieAutomaton(this.position, this.teamName, false);
            else this.auto = new KickerAutomaton(this.position, this.teamName, true);
        }
    }

    private sendCmd() {
        if (this.run) {
            this.act?.send(this.socket);
            this.act = null;
        }
    }
}
