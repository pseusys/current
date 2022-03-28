import { StrategicController } from "../controllers";
import { Command } from "../utils/command";
import { DefenderEntry } from "./entry";


export class DefenderStrategic extends StrategicController {
    private back: boolean = false;

    execute(input: any, accumulator: Map<string, any>): Command | null {
        const sup = super.execute(input, accumulator);
        if (sup) return sup;

        const intercept = this.intercept();
        if (intercept) return intercept;

        if (this.worldInfo.kickIn || this.back) accumulator.set(StrategicController.states.newAction, "return");
        else accumulator.delete(StrategicController.states.newAction);
        this.back = false;
        return null;
    }

    protected immediate(accumulator: Map<string, any>): Command | null {
        if (accumulator.get(DefenderEntry.states.can_kick)) {
            const closest = this.worldInfo.team.sort((a, b) => a.dist - b.dist).filter(player => (player.id) && (player.id > 4))[0];
            if (closest) return new Command("kick", `${closest.dist + 10} ${closest.angle}`);
            else if (this.worldInfo.goal) return new Command("kick", `100 ${this.worldInfo.goal.angle}`);
            else {
                const centre = this.worldInfo.flags.find(flag => flag.name == "fc");
                if (centre) return new Command("kick", `100 ${centre.angle}`);
                else return new Command("kick", `60 0`);
            }
        } else return null;
    }

    private intercept(): Command | null {
        if (this.worldInfo.ball && (this.worldInfo.ball.dist < 15)) {
            this.back = true;
            if (Math.abs(this.worldInfo.ball.angle) > 5) return new Command("turn", this.worldInfo.ball.angle);
            else return new Command("dash", 100);
        } else return null;
    }
}
