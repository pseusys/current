import { EntryController } from "../controllers";
import { Position } from "../utils/constants";
import { GoalieTactic } from "./tactic";
import { WorldInfo } from "../utils/locator";
import { Command } from "../utils/command";
import { Logger } from "../utils/logger";


export class GoalieEntry extends EntryController {
    static readonly states = {
        ...EntryController.states,
        can_catch: "CAN_CATCH"
    }

    constructor(side: Position, team: string, log: Logger | null = null) {
        const info = new WorldInfo(side, team);
        super(-1, side, new GoalieTactic(side, info, log), info, log);
    }

    protected execute(input: any, accumulator: Map<string, any> = new Map()): Command | null {
        accumulator.set(GoalieEntry.states.can_catch, this.worldInfo.ball && this.worldInfo.ball.dist < 1.5);
        return super.execute(input, accumulator);
    }
}
