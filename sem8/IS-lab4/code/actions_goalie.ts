import { DecisionTree, DecisionTreeLeaf, DecisionTreeState } from "./actions";
import { Command } from "./command";

const state = new DecisionTreeState([
    { name: "dash", what: "gr" },
    { name: "catch", what: "b" },
    { name: "kick", what: "b", where: "fprc" }
]);

const seek: DecisionTreeLeaf = {
    command: _ => new Command("turn", 45)
};

const turn: DecisionTreeLeaf = {
    command: s => new Command("turn", s.findWhat()?.angle!!)
};

const dash: DecisionTreeLeaf = {
    command: _ => new Command("dash", 100)
};

const approach: DecisionTreeLeaf = {
    condition: s => Math.abs(s.findWhat()?.angle!!) > 5,
    resultTrue: turn,
    resultFalse: dash
};

const waitForBall: DecisionTreeLeaf = {
    command: _ => new Command("say", 'come_here?')
};

const kick: DecisionTreeLeaf = {
    command: s => new Command("kick", `100 ${s.findWhere()?.angle!!}`),
    reset: s => s.reset()
};

const randomKick: DecisionTreeLeaf = {
    command: _ => new Command("kick", `100 ${Math.floor(Math.random() * 180 - 90)}`),
    reset: s => s.reset()
};

const doKick: DecisionTreeLeaf = {
    condition: s => s.findWhere()?.angle != undefined,
    resultTrue: kick,
    resultFalse: randomKick
};

const ctch: DecisionTreeLeaf = {
    run: s => console.log(`Goalie: trying to ${s.name()} ${s.findWhat()?.angle!!} ${s.findWhat()?.distance!!}`),
    command: s => new Command("catch", s.findWhat()?.angle!!)
};

const kickOrCatch: DecisionTreeLeaf = {
    condition: s => s.name() == "kick",
    resultTrue: doKick,
    resultFalse: ctch
};

const intercept: DecisionTreeLeaf = {
    run: s => (s.findWhat()?.distance!! <= 0.75 && s.name() == "catch") ? s.proceed() : undefined,
    condition: s => s.findWhat()?.distance!! > 1.5,
    resultTrue: approach,
    resultFalse: kickOrCatch
};

const ballVisible: DecisionTreeLeaf = {
    condition: s => s.findWhat()?.distance!! > 15,
    resultTrue: waitForBall,
    resultFalse: intercept
};

const searchBall: DecisionTreeLeaf = {
    condition: s => s.findWhat() != undefined,
    resultTrue: ballVisible,
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

const root: DecisionTreeLeaf = {
    condition: s => s.name() == "dash",
    resultTrue: searchGoal,
    resultFalse: searchBall
}

export const goaile = new DecisionTree(root, state);
