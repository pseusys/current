import { TacticController } from "../controllers";
import { Command } from "../utils/command";
import { WorldInfo } from "../utils/locator";
import { GoalieStrategic } from "./strategic";
import { Position } from "../utils/constants";
import { Logger } from "../utils/logger";


export class GoalieTactic extends TacticController {
    protected action: string = "return";
    private turn: "fct" | "fcb";

    constructor(side: Position, info: WorldInfo, log: Logger | null = null) {
        super(-1, side, new GoalieStrategic(side, info, log), info, log);
        this.turn = "fct";
    }

    execute(input: any, accumulator: Map<string, any>): Command | null {
        switch (this.action) {
            case "return": this.command = this.return(); break;
            case "rotate": this.command = this.rotate(); break;
            case "seek": this.command = this.seek(); break;
        }
        return super.execute(input, accumulator);
    }

    private return(): Command | null {
        if (!this.worldInfo.prot) return new Command("turn", 60);
        if (Math.abs(this.worldInfo.prot.angle) > 10) return new Command("turn", this.worldInfo.prot.angle);
        if (this.worldInfo.prot.dist > 5) return new Command("dash", this.worldInfo.prot.dist * 2 + 30);
        this.action = "rotate";
        return new Command("turn", 180);
    }

    private rotate(): Command | null {
        const center = this.worldInfo.flags.find(flag => flag.name == "fc");
        if (!center) return new Command("turn", 60);
        else {
            this.action = "seek";
            return new Command("turn", center.angle);
        }
    }

    private seek(): Command | null {
        const side = this.worldInfo.flags.find(flag => flag.name == this.turn);
        if (side) {
            if (Math.abs(side.angle) > 10) return new Command("turn", side.angle);
            if (this.turn == "fct") this.turn = "fcb";
            else if (this.turn == "fcb") this.turn = "fct";
        }
        return new Command("turn", ((this.side == "l") != (this.turn == "fct")) ? 30 : -30);
    }
}
