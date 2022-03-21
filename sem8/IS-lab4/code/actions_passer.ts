import { DecisionTreeLeaf, DecisionTreeState } from "./actions";
import { Command } from "./command";

const seekLeft: DecisionTreeLeaf = {
    command: _ => new Command("turn", 20)
};

const search: DecisionTreeLeaf = {
    command: _ => new Command("kick", "10 -45")
};

const wait: DecisionTreeLeaf = {
    command: _ => new Command("say", "wait_for_passer")
};

const turn: DecisionTreeLeaf = {
    command: s => new Command("turn", s.findWhat()?.angle!!)
};

const dash: DecisionTreeLeaf = {
    command: _ => new Command("dash", 70)
};

const approach: DecisionTreeLeaf = {
    condition: s => Math.abs(s.findWhat()?.angle!!) > 5,
    resultTrue: turn,
    resultFalse: dash
};

const kick: DecisionTreeLeaf = {
    run: s => s.params.set("kicked", true),
    command: s => {
        const computed = s.findWhere()!!.angle + s.findWhereSpeed()!!.angle * 60;
        const result = (computed > s.findWhere()!!.angle - 20 || computed < -10) ? s.findWhere()!!.angle - 20 : computed;
        console.log(`PASS: ${computed} vs ${result}`)
        return new Command("kick", `85 ${result}`)
    }
};

const waitTimer: DecisionTreeLeaf = {
    run: s => s.params.get("waited") ? s.params.set("waited", s.params.get("waited") + 1) : s.params.set("waited", 0),
    condition: s => (s.params.get("waited") >= 10),
    resultTrue: search,
    resultFalse: wait
};

const waitBallAndGoal: DecisionTreeLeaf = {
    condition: s => (s.findWhere() != undefined) && (s.findWhereSpeed() != undefined) && (Math.abs(s.findWhereSpeed()!!.angle) > 0.1),
    resultTrue: kick,
    resultFalse: waitTimer
};

const ballVisible: DecisionTreeLeaf = {
    condition: s => s.findWhat()?.distance!! <= 1,
    resultTrue: waitBallAndGoal,
    resultFalse: approach
};

const searchBall: DecisionTreeLeaf = {
    condition: s => s.findWhat() != undefined,
    resultTrue: ballVisible,
    resultFalse: seekLeft
};

const goalVisible: DecisionTreeLeaf = {
    run: s => (s.findWhat()?.distance!! <= 3 && s.name() == "dash") ? s.proceed() : undefined,
    condition: s => s.name() == "dash",
    resultTrue: approach,
    resultFalse: searchBall
};

const searchGoal: DecisionTreeLeaf = {
    condition: s => s.findWhat() != undefined,
    resultTrue: goalVisible,
    resultFalse: seekLeft
};

const inGame: DecisionTreeLeaf = {
    condition: s => s.name() == "dash",
    resultTrue: searchGoal,
    resultFalse: searchBall
}

const say: DecisionTreeLeaf = {
    run: s => s.params.set("said", true),
    command: _ => new Command("say", "go")
};

const stuck: DecisionTreeLeaf = {
    condition: s => s.params.get("said"),
    resultTrue: wait,
    resultFalse: say
};

const root: DecisionTreeLeaf = {
    condition: s => s.params.get("kicked"),
    resultTrue: stuck,
    resultFalse: inGame
}

export const passerRoot = root;
export function passerState() {
    return new DecisionTreeState([
        { name: "dash", what: "fplt" },
        { name: "kick", what: "b", where: "p" }
    ]);
}
