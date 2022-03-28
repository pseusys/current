import { TacticController } from "../controllers";
import { Command } from "../utils/command";
import { WorldInfo } from "../utils/locator";
import { AlphaStrategic } from "./strategic";
import { Position } from "../utils/constants";
import { Logger } from "../utils/logger";


export class AlphaTactic extends TacticController {
    protected action: string = "return";

    constructor(id: number, side: Position, info: WorldInfo, log: Logger | null = null) {
        super(id, side, new AlphaStrategic(id, side, info, log), info, log);
    }

    execute(input: any, accumulator: Map<string, any>): Command | null {
        this.command = this.return();
        return super.execute(input, accumulator);
    }

    private return(): Command | null {
        if (!this.worldInfo.ball) return new Command("turn", 60);
        if (Math.abs(this.worldInfo.ball.angle) > 10) return new Command("turn", this.worldInfo.ball.angle);
        if (this.worldInfo.ball.dist > 0.5) return new Command("dash", 100);
        return null;
    }
}
