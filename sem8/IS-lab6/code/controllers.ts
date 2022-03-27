import { Command } from "./utils/command";
import { WorldInfo } from "./utils/locator";
import { Position } from "./utils/constants";
import { Logger } from "./utils/logger";


export abstract class Controller {
    protected readonly next: TacticController | StrategicController | null;
    protected readonly worldInfo: WorldInfo;
    protected readonly log: Logger | null;
    protected readonly side: Position;
    protected readonly id: number;

    static readonly states = {}

    protected constructor(id: number, side: Position, next: TacticController | StrategicController | null, info: WorldInfo, log: Logger | null = null) {
        this.worldInfo = info;
        this.next = next;
        this.log = log;
        this.side = side;
        this.id = id;
    }

    protected abstract execute(input: any, accumulator: Map<string, any>): Command | null;
}

export class EntryController extends Controller {
    protected angle: number = 0;

    static readonly states = {
        ...Controller.states,
        can_kick: "CAN_KICK"
    }

    act(input: any): Command | null {
        let result: Command | null;
        const upd = this.worldInfo.update(input, this.angle);
        if (upd == true) result = this.execute(input);
        else if (upd == null) result = new Command("turn", 60);
        else result = null;
        if (result && (result.name == "turn")) this.angle = (this.angle + Number(result.value)) % 360;
        return result;
    }

    protected execute(input: any, accumulator: Map<string, any> = new Map()): Command | null {
        accumulator.set(EntryController.states.can_kick, this.worldInfo.ball && this.worldInfo.ball.dist < 0.5);
        return this.next!!.execute(input, accumulator);
    }
}

export abstract class TacticController extends Controller {
    protected abstract action: string;
    protected command: Command | null = null;

    static readonly states = {
        ...Controller.states,
        action: "ACTION"
    }

    execute(input: any, accumulator: Map<string, any>): Command | null {
        accumulator.set(TacticController.states.action, this.action);

        const command = this.next!!.execute(input, accumulator);
        if (command) return command;
        if (accumulator.get(StrategicController.states.newAction)) this.action = accumulator.get(StrategicController.states.newAction);

        return this.command;
    }
}

export abstract class StrategicController extends Controller {
    static readonly states = {
        ...Controller.states,
        newAction: "NEW_ACTION"
    }

    constructor(id: number, side: Position, info: WorldInfo, log: Logger | null = null) {
        super(id, side, null, info, log);
    }

    protected abstract immediate(accumulator: Map<string, any>): Command | null

    execute(input: any, accumulator: Map<string, any>): Command | null {
        const immediate = this.immediate(accumulator);
        if (immediate) return immediate;
        else return null;
    }
}
