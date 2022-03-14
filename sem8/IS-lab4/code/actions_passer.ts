import { DecisionTreeLeaf, DecisionTreeState } from "./actions";
import { Command } from "./command";

const state = new DecisionTreeState([
    { name: "dash", what: "fplt" },
    { name: "kick", what: "b", where: "p" }
]);

const seekLeft: DecisionTreeLeaf = {
    command: _ => new Command("turn", 20)
};

const wait: DecisionTreeLeaf = {
    command: _ => new Command("say", "wait_for_passer")
};

const search: DecisionTreeLeaf = {
    command: _ => new Command("kick", "10 -45")
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

const kick: DecisionTreeLeaf = {
    run: s => { s.params.set("kicked", true); console.log(`PASS: ${JSON.stringify(s.findWhere()!!.angle)} + ${JSON.stringify(s.findWhereSpeed()!!.angle)}`);},
    command: s => new Command("kick", `100 ${s.findWhere()!!.angle + s.findWhereSpeed()!!.angle}`)
};

const waitBallAndGoal: DecisionTreeLeaf = {
    condition: s => (s.findWhere() != undefined) && (s.findWhereSpeed() != undefined),
    resultTrue: kick,
    resultFalse: wait
};

const searchBallAndGoal: DecisionTreeLeaf = {
    condition: s => s.findWhere() != undefined,
    resultTrue: waitBallAndGoal,
    resultFalse: search
};

const ballVisible: DecisionTreeLeaf = {
    run: s => { if (s.findWhere() && s.findWhereSpeed()) console.log(`Player coords: (${s.findWhere()?.distance}, ${s.findWhere()?.angle}) -> (${s.findWhere()!!.distance + s.findWhereSpeed()!!.distance}, ${s.findWhere()!!.angle  + s.findWhereSpeed()!!.angle})`); },
    condition: s => s.findWhat()?.distance!! <= 1,
    resultTrue: searchBallAndGoal,
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
export const passerState = state;
