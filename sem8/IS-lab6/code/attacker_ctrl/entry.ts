import { EntryController } from "../controllers";
import { Position } from "../utils/constants";
import { TeamName } from "../app";
import { AttackerTactic } from "./tactic";
import { WorldInfo } from "../utils/locator";
import { Logger } from "../utils/logger";


export class AttackerEntry extends EntryController {
    constructor(id: number, side: Position, team: TeamName, log: Logger | null = null) {
        const info = new WorldInfo(side, team);
        super(id, side, new AttackerTactic(id, side, info, log), info, log);
    }
}
