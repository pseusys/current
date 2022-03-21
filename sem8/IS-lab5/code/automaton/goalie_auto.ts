import { Act, Automaton, Body, Edge, Node, State, Sync, SyncGuard } from "./automaton";
import { Position } from "../utils/constants";
import { TeamName } from "../utils/app";
import { WorldInfo } from "./locator";
import { Command } from "../utils/command";


function init(info: WorldInfo, state: State) {
    state.local.set("goalie", true);
    state.local.set("catch", 0);
}

function beforeAction(info: WorldInfo, state: State) {
    if (info.ballPos) state.variables.set("dist", info.ballPos.dist);
}

function ctch(info: WorldInfo, state: State): Command | null {
    if (!info.ballPos?.angle && !info.ballPos?.dist) {
        state.next = true;
        return null;
    }
    state.next = false;
    if (info.ballPos.dist > 0.5) {
        if (state.local.get("goalie")) {
            if (state.local.get("catch") < 3) {
                state.local.set("catch", state.local.get("catch") + 1);
                return new Command("catch", info.ballPos.angle);
            } else state.local.set("catch", state.local.get("catch") + 1);
        }
        if (Math.abs(info.ballPos.angle) > 15) return new Command("turn", info.ballPos.angle);
        else return new Command("dash", info.ballPos.angle);
    }
    state.next = true;
    return null;
}

function kick(info: WorldInfo, state: State): Command | null {
    state.next = true;
    if (!info.ballPos) return null;
    if (info.ballPos.dist > 0.5) return null;
    const player = info.team ? info.team[0] : null;
    let target;
    if ((info.goal && !isNaN(info.prot.angle) || !isNaN(info.prot.dist)) && player) target = info.goal.dist < player.dist ? info.goal : player;
    else if (info.goal && !isNaN(info.prot.angle) || !isNaN(info.prot.dist)) target = info.goal;
    else if (player) target = player;
    if (target) return new Command("kick", `${target.dist * 2 + 40} ${target.angle}`);
    return new Command("kick", "30 45");
}

function goBack(info: WorldInfo, state: State): Command | null {
    state.next = false;
    if (!info.prot || isNaN(info.prot.angle) || isNaN(info.prot.dist)) return new Command("turn", 60);
    if (Math.abs(info.prot.angle) > 2) return new Command("turn", info.prot.angle);
    if (info.prot.dist < 2) {
        state.next = true;
        return new Command("turn", 180);
    }
    return new Command("dash", info.prot.dist * 2 + 20);
}

function lookAround(info: WorldInfo, state: State): Command | null {
    state.next = false;
    state.sync = "lookAround";
    if (!state.local.get("look")) state.local.set("look", "left");
    switch (state.local.get("look")) {
        case "left":
            state.local.set("look", "center");
            return new Command("turn", -60);
        case "center":
            state.local.set("look", "right");
            return new Command("turn", 60);
        case "right":
            state.local.set("look", "back");
            return new Command("turn", 60);
        case "back":
            state.local.set("look", "left");
            state.next = true;
            state.sync = undefined;
            return new Command("turn", -60);
        default:
            state.next = true
            return null;
    }
}

function canIntercept(info: WorldInfo, state: State): boolean {
    state.next = true;
    if (!info.ballPos) return false;
    if (info.enemy) return !info.enemy.find(rival => {
        const degrees = Math.sign(rival.angle) == Math.sign(info.ballPos!!.angle) ? Math.max(Math.abs(rival.angle), Math.abs(info.ballPos!!.angle)) - Math.min(Math.abs(rival.angle), Math.abs(rival.angle)) : Math.abs(rival.angle) + Math.abs(info.ballPos!!.angle);
        const rivalDistanceToBall = Math.sqrt(rival.dist ** 2 + info.ballPos!!.dist ** 2 - 2 * rival.dist * info.ballPos!!.dist * Math.cos(degrees * Math.PI / 180));
        return rivalDistanceToBall < info.ballPos!!.dist
    });
    if (!info.ballSpeed) return true;
    return info.ballPos.dist <= info.ballPos.dist - info.ballSpeed.dist + 0.5;

}

function runToBall(info: WorldInfo, state: State): Command | null {
    state.next = false;
    if (!info.ballPos) return goBack(info, state);
    if (info.ballPos.dist <= 2) {
        state.next = true;
        return null;
    }
    if (Math.abs(info.ballPos.angle) > 10) return new Command("turn", info.ballPos.angle);
    else return new Command("dash", 110);
}

function ok(info: WorldInfo, state: State): Command | null {
    state.next = true;
    return new Command("turn", 0);
}

function empty(info: WorldInfo, state: State) {
    state.next = true
}

const actions = new Map<string, Act | Sync | SyncGuard>();
actions.set("init", init);
actions.set("beforeAction", beforeAction);
actions.set("ctch", ctch);
actions.set("kick", kick);
actions.set("goBack", goBack);
actions.set("lookAround", lookAround);
actions.set("canIntercept", canIntercept);
actions.set("runToBall", runToBall);
actions.set("ok", ok);
actions.set("empty", empty);


const variables = new Map<string, any>()
variables.set("dist", null);

const timers = new Map<string, number>();
timers.set("t", 0);

const local = new Map<string, number>();


const nodes: Node[] = [
    { name: "start", connected: ["close", "near", "far"] },
    { name: "close", connected: ["ctch"] },
    { name: "ctch", connected: ["kick"] },
    { name: "kick", connected: ["start"] },
    { name: "far", connected: ["start"] },
    { name: "near", connected: ["intercept", "start"] },
    { name: "intercept", connected: ["start"] }
];


const start_to_close: Edge = {
    from: "start",
    to: "close",
    content: [
        { guard: [
            { left: { type: "timer", name: "dist" }, operator: "lt", rightVal: 2 }
        ] }
    ]
};

const start_to_near: Edge = {
    from: "start",
    to: "near",
    content: [
        { guard: [
            { left: { type: "var", name: "dist" }, operator: "lt", rightVal: 10 },
            { leftVal: 2, operator: "lte", right: { type: "var", name: "dist" } }
        ] }
    ]
};

const start_to_far: Edge = {
    from: "start",
    to: "far",
    content: [
        { guard: [
            { leftVal: 10, operator: "lte", right: { type: "var", name: "dist" } }
        ] }
    ]
};

const close_to_ctch: Edge = {
    from: "close",
    to: "ctch",
    content: [
        { sync: "ctch" }
    ]
};

const ctch_to_kick: Edge = {
    from: "ctch",
    to: "kick",
    content: [
        { sync: "kick" }
    ]
};

const kick_to_start: Edge = {
    from: "kick",
    to: "start",
    content: [ {
        sync: "goBack",
        assign: [
            { name: "t", value: 0, type: "timer" }
        ]
    } ]
};

const far_to_start: Edge = {
    from: "far",
    to: "start",
    content: [ {
        guard: [
            { leftVal: 10, operator: "lt", right: { type: "timer", name: "t" }}
        ],
        sync: "lookAround",
        assign: [
            { name: "t", value: 0, type: "timer" }
        ]
    }, {
        guard: [
            { left: { type: "timer", name: "t" }, operator: "lte", rightVal: 10 }
        ],
        sync: "ok"
    } ]
};

const near_to_start: Edge = {
    from: "near",
    to: "start",
    content: [ {
        sync: "empty",
        assign: [
            { name: "t", value: 0, type: "timer" }
        ]
    } ]
};

const near_to_intercept: Edge = {
    from: "near",
    to: "intercept",
    content: [
        { sync: "canIntercept" }
    ]
};

const intercept_to_start: Edge = {
    from: "intercept",
    to: "start",
    content: [ {
        sync: "runToBall",
        assign: [
            { name: "t", value: 0, type: "timer" }
        ]
    } ]
};

const edges: Edge[] = [
    start_to_close,
    start_to_near,
    start_to_far,
    close_to_ctch,
    ctch_to_kick,
    kick_to_start,
    far_to_start,
    near_to_start,
    near_to_intercept,
    intercept_to_start
];


const goalieState = {
    local: local,
    next: true,
    sync: undefined,
    timers: timers,
    variables: variables
};

const goalieBody: Body = {
    actions: actions,
    currentEdge: 0,
    currentNode: undefined,
    edges: edges,
    nodes: nodes,
    state: goalieState
};


export class GoalieAutomaton extends Automaton {
    constructor(side: Position, team: TeamName) {
        super(side, team, goalieBody);
    }
}
