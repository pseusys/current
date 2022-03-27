import { StrategicController } from "../controllers";
import { Command } from "../utils/command";
import { AlphaEntry } from "./entry";


export class AlphaStrategic extends StrategicController {
    execute(input: any, accumulator: Map<string, any>): Command | null {
        const sup = super.execute(input, accumulator);
        if (sup) return sup;
        const react = this.react();
        if (react) return react;
        else return null;
    }

    protected immediate(accumulator: Map<string, any>): Command | null {
        if (accumulator.get(AlphaEntry.states.can_kick)) {
            if (this.worldInfo.goal) return new Command("kick", `100 ${this.worldInfo.goal.angle}`);
            else return new Command("kick", `5 45`);
        } else return null;
    }

    private react(): Command | null {
        if (this.worldInfo.ball && (this.worldInfo.ball.dist < 1)) return new Command("kick", "100 180");
        else return null;
    }
}
