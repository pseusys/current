import { EntryController } from "../controllers";
import { Position } from "../utils/constants";
import { TeamName } from "../app";
import { AlphaTactic } from "./tactic";
import { WorldInfo } from "../utils/locator";
import { Command } from "../utils/command";
import { Logger } from "../utils/logger";


export class AlphaEntry extends EntryController {
    constructor(id: number, side: Position, team: TeamName, log: Logger | null = null) {
        const info = new WorldInfo(side, team);
        super(id, side, new AlphaTactic(id, side, info, log), info, log);
    }

    act(input: any): Command | null {
        let result: Command | null;
        const upd = this.worldInfo.update(input, this.angle);
        if ((upd == true) || (upd == null)) result = this.execute(input);
        else result = null;
        if (result && (result.name == "turn")) this.angle = (this.angle + Number(result.value)) % 360;
        return result;
    }
}
