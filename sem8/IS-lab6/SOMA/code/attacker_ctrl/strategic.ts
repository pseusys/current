import { StrategicController } from "../controllers";
import { Command } from "../utils/command";
import { AttackerEntry } from "./entry";


export class AttackerStrategic extends StrategicController {
    private last: string | null = null;

    execute(input: any, accumulator: Map<string, any>): Command | null {
        const sup = super.execute(input, accumulator);
        if (sup) return sup;

        const intercept = this.intercept();
        if (intercept) return intercept;

        if (this.worldInfo.kickIn || (this.last == "intercept")) accumulator.set(StrategicController.states.newAction, "return");
        else accumulator.delete(StrategicController.states.newAction)
        this.last = "previous";
        return null;
    }

    protected immediate(accumulator: Map<string, any>): Command | null {
        if (accumulator.get(AttackerEntry.states.can_kick)) {
            if (this.worldInfo.goal) return new Command("kick", `100 ${this.worldInfo.goal.angle}`);
            else {
                const centre = this.worldInfo.flags.find(flag => flag.name == (this.side == "l" ? "fprc" : "fplc"));
                if (centre) return new Command("kick", `100 ${centre.angle}`);
                else return new Command("kick", `60 -90`);
            }
        } else return null;
    }

    private intercept(): Command | null {
        const centre = this.worldInfo.flags.find(flag => ["fct", "fc", "fcb"].includes(flag.name));
        if (this.worldInfo.ball && (((centre) && (this.worldInfo.ball.dist < centre.dist)) || (this.worldInfo.ball.dist < 15))) {
            this.last = "intercept";
            if (Math.abs(this.worldInfo.ball.angle) > 5) return new Command("turn", this.worldInfo.ball.angle);
            else return new Command("dash", 100);
        } else return null;
    }
}
