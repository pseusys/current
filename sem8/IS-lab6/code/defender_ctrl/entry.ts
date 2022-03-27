import { EntryController } from "../controllers";
import { Position } from "../utils/constants";
import { TeamName } from "../app";
import { DefenderTactic } from "./tactic";
import { WorldInfo } from "../utils/locator";
import { Logger } from "../utils/logger";


export class DefenderEntry extends EntryController {
    constructor(id: number, side: Position, team: TeamName, log: Logger | null = null) {
        const info = new WorldInfo(side, team);
        super(id, side, new DefenderTactic(id, side, info, log), info, log);
    }
}
