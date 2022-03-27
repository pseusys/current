import { TacticController } from "../controllers";
import { Command } from "../utils/command";
import { WorldInfo } from "../utils/locator";
import { AttackerStrategic } from "./strategic";
import { Position } from "../utils/constants";
import { Logger } from "../utils/logger";


export class AttackerTactic extends TacticController {
    protected action: string = "return";

    constructor(id: number, side: Position, info: WorldInfo, log: Logger | null = null) {
        super(id, side, new AttackerStrategic(id, side, info, log), info, log);
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
        const home = this.worldInfo.flags.find(flag => {
            switch (this.id) {
                case 30: return flag.name == (this.side == "l" ? "fprt" : "fplt");
                case 33: return flag.name == (this.side == "l" ? "fprb" : "fplb");
                default: throw Error(`Strange defender, artefact: ${this.id}`);
            }
        });

        if (!home) return new Command("turn", 60);
        if (Math.abs(home.angle) > 10) return new Command("turn", home.angle);
        if (home.dist > 5) return new Command("dash", 75);
        this.action = "rotate";
        return new Command("turn", 90);
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
        if (!this.worldInfo.ball) return new Command("turn", 60);
        if (Math.abs(this.worldInfo.ball.angle) > 5) return new Command("turn", this.worldInfo.ball.angle);
        return null;
    }
}
