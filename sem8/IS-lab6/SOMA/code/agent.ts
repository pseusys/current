import { parseMsg } from "./utils/msg";
import { Socket } from "dgram";
import { create } from "./utils/socket";
import { FIELD_X, FIELD_Y, Position } from "./utils/constants";
import { Command } from "./utils/command";
import { reset_player } from "./utils/positioner";
import { Coordinate } from "./utils/locator";
import { EntryController } from "./controllers";
import { Logger } from "./utils/logger";
import { GoalieEntry } from "./goalie_ctrl/entry";
import { SemidefEntry } from "./semidef_ctrl/entry";
import { DefenderEntry } from "./defender_ctrl/entry";
import { AttackerEntry } from "./attacker_ctrl/entry";
import { AlphaEntry } from "./alpha_ctrl/entry";

export type Role = "goalie" | "defender" | "semidef" | "attacker" | "alpha";


export class Agent {
    readonly teamName: string;
    private position: Position;
    private readonly role: Role;
    private run: boolean;
    private act: Command | null;

    private ctrl: EntryController | undefined;
    private log: Logger | undefined;

    private readonly socket: Socket;
    id: number | undefined;
    artefact: number;

    constructor(team: string, artefact: number, position: Position, coordinate: Coordinate, role: Role) {
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

        if (isNaN(coordinate.x) || isNaN(coordinate.y) || coordinate.x < -FIELD_X || coordinate.x > FIELD_X || coordinate.y < -FIELD_Y || coordinate.y > FIELD_Y) console.log("Неверные аргументы!");
        else setTimeout(() => { (new Command("move", `${coordinate.x} ${coordinate.y}`)).send(this.socket) }, 50);
        setTimeout(() => { if (position == "r") (new Command("turn", 180)).send(this.socket) }, 150);
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
        if (this.run) this.act = this.ctrl!!.act(data) ?? null;
    }

    private initAgent(p: [string, string, string]) {
        if (p[0] == "r") this.position = "r";
        else this.position = "l";
        if (p[1]) {
            this.id = Number(p[1]);
            this.log = new Logger(this.id, this.teamName);
            switch (this.role) {
                case "goalie": this.ctrl = new GoalieEntry(this.position, this.teamName, this.log); break;
                case "defender": this.ctrl = new DefenderEntry(this.artefact, this.position, this.teamName, this.log); break;
                case "semidef": this.ctrl = new SemidefEntry(this.artefact, this.position, this.teamName, this.log); break;
                case "attacker": this.ctrl = new AttackerEntry(this.artefact, this.position, this.teamName, this.log); break;
                case "alpha": this.ctrl = new AlphaEntry(this.artefact, this.position, this.teamName, this.log); break;
                default: throw Error(`Strange role: ${this.role}`);
            }
        }
    }

    private sendCmd() {
        if (this.run) {
            this.act?.send(this.socket);
            this.act = null;
        }
    }
}
