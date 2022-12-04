import { StrategicController } from "../controllers";
import { Command } from "../utils/command";
import { AlphaEntry } from "./entry";


export class AlphaStrategic extends StrategicController {
    execute(input: any, accumulator: Map<string, any>): Command | null {
        const sup = super.execute(input, accumulator);
        if (sup) return sup;
        const react = this.react(accumulator);
        if (react) return react;
        return null;
    }

    protected immediate(accumulator: Map<string, any>): Command | null {
        if (this.worldInfo.ball && !isNaN(this.worldInfo.ball.x + this.worldInfo.ball.y) && accumulator.get(AlphaEntry.states.can_kick)) {
            if (this.worldInfo.goal) return new Command("kick", `100 ${this.worldInfo.goal.angle}`);
            else return new Command("kick", `5 45`);
        } else return null;
    }

    private react(accumulator: Map<string, any>): Command | null {
        if (this.worldInfo.ball && isNaN(this.worldInfo.ball.x + this.worldInfo.ball.y) && accumulator.get(AlphaEntry.states.can_kick)) return new Command("kick", "100 180");
        else return null;
    }
}
