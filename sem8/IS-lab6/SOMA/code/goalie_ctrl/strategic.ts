import { StrategicController } from "../controllers";
import { Command } from "../utils/command";
import { GoalieEntry } from "./entry";
import { dist } from "../utils/math";
import { Position } from "../utils/constants";
import { WorldInfo } from "../utils/locator";
import { Logger } from "../utils/logger";


export class GoalieStrategic extends StrategicController {
    private last: string | null = null;

    constructor(side: Position, info: WorldInfo, log: Logger | null = null) {
        super(-1, side, info, log);
    }

    execute(input: any, accumulator: Map<string, any>): Command | null {
        const sup = super.execute(input, accumulator);
        if (sup) return sup;

        const intercept = this.intercept();
        if (intercept) return intercept;

        if (this.last == "intercept" || this.last == "kick") accumulator.set(StrategicController.states.newAction, "return");
        else accumulator.delete(StrategicController.states.newAction);
        this.last = "previous";
        return null;
    }

    protected immediate(accumulator: Map<string, any>): Command | null {
        if (accumulator.get(GoalieEntry.states.can_kick)) {
            this.last = "kick";
            const closest = this.worldInfo.team.sort((a, b) => a.dist - b.dist)[0];
            if (closest) return new Command("kick", `${closest.dist + 10} ${closest.angle}`);
            else if (this.worldInfo.goal) return new Command("kick", `100 ${this.worldInfo.goal.angle}`);
            else return new Command("kick", `60 0`);
        } else if (accumulator.get(GoalieEntry.states.can_catch)) {
            this.last = "catch";
            return new Command("catch", this.worldInfo.ball!!.angle);
        } else return null;
    }

    private intercept(): Command | null {
        if (this.worldInfo.ball) {
            const enemy = this.worldInfo.enemy.find(player => dist(player, this.worldInfo.ball!!) < this.worldInfo.ball!!.dist);

            const reachable = !!(this.side === "l" ? ["fplb", "fplc", "fplt"] : ["fprb", "fprc", "fprt"]).find(name => {
                return this.worldInfo.flags.find(flag => (flag.name == name) && (this.worldInfo.ball!!.dist < flag.dist));
            });

            if ((this.worldInfo.ball.dist < 10) || (!enemy && reachable)) {
                this.last = "intercept";
                if (Math.abs(this.worldInfo.ball.angle) > 5) return new Command("turn", this.worldInfo.ball.angle);
                else return new Command("dash", 100);
            } else return null;
        } else return null;
    }
}
