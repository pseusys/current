import { StrategicController } from "../controllers";
import { Command } from "../utils/command";
import { SemidefEntry } from "./entry";
import { dist } from "../utils/math";


export class SemidefStrategic extends StrategicController {
    private last: string | null = null;

    execute(input: any, accumulator: Map<string, any>): Command | null {
        const sup = super.execute(input, accumulator);
        if (sup) return sup;

        const intercept = this.intercept();
        if (intercept) return intercept;

        const attack = this.attack();
        if (attack) return attack;

        if ((this.last == "intercept") || (this.last == "attack")) accumulator.set(StrategicController.states.newAction, "return");
        else accumulator.delete(StrategicController.states.newAction)
        this.last = "previous";
        return null;
    }

    protected immediate(accumulator: Map<string, any>): Command | null {
        if (accumulator.get(SemidefEntry.states.can_kick)) {
            const closest = this.worldInfo.team.sort((a, b) => a.dist - b.dist).filter(player => (player.id) && (player.id > 7))[0];
            if (closest) return new Command("kick", `${closest.dist + 30} ${closest.angle}`);
            else if (this.worldInfo.goal) return new Command("kick", `100 ${this.worldInfo.goal.angle}`);
            else {
                const centre = this.worldInfo.flags.find(flag => flag.name == (this.side == "l" ? "fprc" : "fplc"));
                if (centre) return new Command("kick", `100 ${centre.angle}`);
                else return new Command("kick", `60 0`);
            }
        } else return null;
    }

    private intercept(): Command | null {
        if (this.worldInfo.ball && (this.worldInfo.ball.dist < 15)) {
            this.last = "intercept";
            if (Math.abs(this.worldInfo.ball.angle) > 5) return new Command("turn", this.worldInfo.ball.angle);
            else return new Command("dash", 100);
        } else return null;
    }

    private attack(): Command | null {
        if (this.worldInfo.ball && this.worldInfo.goal && (dist(this.worldInfo.ball, this.worldInfo.goal) < 20)) {
            this.last = "attack";
            if (Math.abs(this.worldInfo.ball.angle) > 5) return new Command("turn", this.worldInfo.ball.angle);
            else return new Command("dash", 100);
        } else return null;
    }
}
