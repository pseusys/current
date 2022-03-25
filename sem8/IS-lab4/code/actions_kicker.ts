import { DecisionTreeLeaf, DecisionTreeState } from "./actions";
import { Command } from "./command";

const seek: DecisionTreeLeaf = {
    command: _ => new Command("turn", 45)
};

const turn: DecisionTreeLeaf = {
    command: s => new Command("turn", s.findWhat()?.angle!!)
};

const dash: DecisionTreeLeaf = {
    command: s => new Command("dash", s.name() == "kick" ? 100 : 58)
};

const approach: DecisionTreeLeaf = {
    condition: s => Math.abs(s.findWhat()?.angle!!) > 5,
    resultTrue: turn,
    resultFalse: dash
};

const kick: DecisionTreeLeaf = {
    run: s => console.log(`kick: ${JSON.stringify(s.findWhat())} ${JSON.stringify(s.findWhere())}`),
    command: s => new Command("kick", `100 ${s.findWhere()?.angle!!}`)
};

const search: DecisionTreeLeaf = {
    run: s => console.log(`search: ${JSON.stringify(s.findWhat())} ${JSON.stringify(s.findWhere())}`),
    command: _ => new Command("kick", "5 45")
};

const doKick: DecisionTreeLeaf = {
    condition: s => s.findWhere()?.angle != undefined,
    resultTrue: kick,
    resultFalse: search
};

const intercept: DecisionTreeLeaf = {
    condition: s => s.findWhat()?.distance!! > 1,
    resultTrue: approach,
    resultFalse: doKick
};

const searchBall: DecisionTreeLeaf = {
    condition: s => s.findWhat() != undefined,
    resultTrue: intercept,
    resultFalse: seek
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
    resultFalse: seek
};

const searchGoalTracingBall: DecisionTreeLeaf = {
    run: s => (s.params.get("signal") && s.name() == "dash" && s.findWhere()) ? s.proceed() : undefined,
    condition: s => s.findWhat() != undefined,
    resultTrue: searchGoal,
    resultFalse: seek
};

const root: DecisionTreeLeaf = {
    condition: s => s.name() == "dash",
    resultTrue: searchGoalTracingBall,
    resultFalse: searchBall
}

export const kickerRoot = root;
export function kickerState(): DecisionTreeState {
    return new DecisionTreeState([
        { name: "dash", what: "fplb" },
        { name: "dash", what: "fprb", where: "b" },
        { name: "kick", what: "b", where: "gr" }
    ]);
}
