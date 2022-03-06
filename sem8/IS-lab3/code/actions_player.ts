import { DecisionTree, DecisionTreeLeaf, DecisionTreeState } from "./actions";
import { Command } from "./command";

const state = new DecisionTreeState([
    //{ name: "dash", what: "fplt" },
    //{ name: "dash", what: "fplb" },
    { name: "kick", what: "b", where: "gr" }
]);

const seekRight: DecisionTreeLeaf = {
    command: _ => new Command("turn", -20)
};

const seekLeft: DecisionTreeLeaf = {
    command: _ => new Command("turn", 20)
};

const wait: DecisionTreeLeaf = {
    command: _ => new Command("say", "wait_for_team_leader_actions")
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
    command: s => new Command("kick", `100 ${s.findWhere()!!.angle}`)
};

const searchBallAndGoal: DecisionTreeLeaf = {
    condition: s => s.findWhere() != undefined,
    resultTrue: kick,
    resultFalse: search
};

const ballVisible: DecisionTreeLeaf = {
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

const activeRoot: DecisionTreeLeaf = {
    condition: s => s.name() == "dash",
    resultTrue: searchGoal,
    resultFalse: searchBall
}

const follow: DecisionTreeLeaf = {
    condition: s => s.findTeamLeader()!!.distance > 5,
    resultTrue: dash,
    resultFalse: wait
}

const turnLeft: DecisionTreeLeaf = {
    command: s => new Command("turn", `${s.findTeamLeader()!!.angle - 30}`)
}

const followLeft: DecisionTreeLeaf = {
    condition: s => Math.abs(s.findTeamLeader()!!.angle - 30) > 20,
    resultTrue: turnLeft,
    resultFalse: follow
}

const turnRight: DecisionTreeLeaf = {
    command: s => new Command("turn", `${s.findTeamLeader()!!.angle + 30}`)
}

const followRight: DecisionTreeLeaf = {
    condition: s => Math.abs(s.findTeamLeader()!!.angle + 30) > 20,
    resultTrue: turnRight,
    resultFalse: follow
}

const checkRole: DecisionTreeLeaf = {
    condition: s => s.getRole() == "l",
    resultTrue: followLeft,
    resultFalse: followRight
}

const seekWithRole: DecisionTreeLeaf = {
    condition: s => s.getRole() == "l",
    resultTrue: seekLeft,
    resultFalse: seekRight
}

const followerRoot: DecisionTreeLeaf = {
    condition: s => s.findTeamLeader() != undefined,
    resultTrue: checkRole,
    resultFalse: seekWithRole
}

const root: DecisionTreeLeaf = {
    condition: s => s.isTeamLeader(),
    resultTrue: activeRoot,
    resultFalse: followerRoot
}

export const player = new DecisionTree(root, state);
