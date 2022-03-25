import { Act, Automaton, Body, Edge, Node, State, Sync, SyncGuard } from "./automaton";
import { Position } from "../utils/constants";
import { TeamName } from "../utils/app";
import { WorldInfo } from "./locator";
import { Command } from "../utils/command";


function init(info: WorldInfo, state: State) {
    state.local.set("catch", 0);
}

function before(info: WorldInfo, state: State) {
    if (info.ball) state.variables.set("dist", info.ball.dist);
}

function start(info: WorldInfo, state: State): Command | null {
    state.next = false;
    if (!info.ball) {
        return new Command("turn", 60);
    } else {
        state.next = true;
        return null;
    }
}

function ctch(info: WorldInfo, state: State): Command | null {
    if (!info.ball) {
        state.next = true;
        return null;
    }
    if (info.ball.dist >= 1.5) {
        if (Math.abs(info.ball.angle) > 15) return new Command("turn", info.ball.angle);
        else return new Command("dash", 50);
    }
    state.next = false;
    if (info.ball.dist > 0.5) {
        if (state.local.get("catch") < 3) {
            state.local.set("catch", state.local.get("catch") + 1);
            return new Command("catch", info.ball.angle);
        } else {
            state.next = true;
            state.local.set("catch", 0);
        }
    }
    return null;
}

function kick(info: WorldInfo, state: State): Command | null {
    state.next = true;
    if (!info.ball) return null;
    const player = info.team ? info.team[0] : null;
    let target;
    if (info.goal && player) target = info.goal.dist < player.dist ? info.goal : player;
    else if (info.goal) target = info.goal;
    else if (player) target = player;
    if (target) return new Command("kick", `${target.dist * 2 + 40} ${target.angle}`);
    return new Command("kick", "30 45");
}

function goBack(info: WorldInfo, state: State): Command | null {
    state.next = false;
    if (!info.prot) return new Command("turn", 60);
    if (Math.abs(info.prot.angle) > 2) return new Command("turn", info.prot.angle);
    if (info.prot.dist < 5) {
        state.next = true;
        return new Command("turn", 180);
    }
    return new Command("dash", info.prot.dist * 2 + 20);
}

function lookAround(info: WorldInfo, state: State): Command | null {
    state.next = false;
    state.sync = "lookAround";
    if (!state.local.get("look")) state.local.set("look", 0);
    if (state.local.get("look") < 5) state.local.set("look", state.local.get("look") + 1);
    else {
        state.local.set("look", 0);
        state.next = true;
        state.sync = undefined;
    }
    return new Command("turn", 60);
}

function canIntercept(info: WorldInfo, state: State): boolean {
    state.next = true;
    if (!info.ball) return false;
    if (info.enemy) return !info.enemy.find(rival => {
        const degrees = Math.sign(rival.angle) == Math.sign(info.ball!!.angle) ? Math.max(Math.abs(rival.angle), Math.abs(info.ball!!.angle)) - Math.min(Math.abs(rival.angle), Math.abs(rival.angle)) : Math.abs(rival.angle) + Math.abs(info.ball!!.angle);
        const rivalDistanceToBall = Math.sqrt(rival.dist ** 2 + info.ball!!.dist ** 2 - 2 * rival.dist * info.ball!!.dist * Math.cos(degrees * Math.PI / 180));
        return rivalDistanceToBall < info.ball!!.dist
    });
    if (!info.ballSpeed) return true;
    return info.ball.dist <= info.ball.dist - info.ballSpeed.dist + 0.5;
}

function runToBall(info: WorldInfo, state: State): Command | null {
    state.next = false;
    if (!info.ball) return goBack(info, state);
    console.log(info.ball.dist);
    if (info.ball.dist < 1.5) {
        state.next = true;
        return null;
    }
    if (Math.abs(info.ball.angle) > 10) return new Command("turn", info.ball.angle);
    else return new Command("dash", 100);
}

function ok(info: WorldInfo, state: State): Command | null {
    state.next = true;
    return new Command("turn", 0);
}

function empty(info: WorldInfo, state: State) {
    state.next = true
}

const actions = new Map<string, Act | Sync | SyncGuard>();
actions.set("start", start);
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
            { left: { type: "var", name: "dist" }, operator: "lt", rightVal: 1.5 }
        ] }
    ]
};

const start_to_near: Edge = {
    from: "start",
    to: "near",
    content: [
        { guard: [
            { left: { type: "var", name: "dist" }, operator: "lt", rightVal: 10 },
            { leftVal: 1.5, operator: "lte", right: { type: "var", name: "dist" } }
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
        sync: "ok",
        assign: [
            { name: "t", value: 0, type: "timer" }
        ]
    } ]
};

const near_to_intercept: Edge = {
    from: "near",
    to: "intercept",
    content: [
        { syncGuard: "canIntercept" }
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
    variables: variables,
    angle: 0
};

const goalieBody: Body = {
    actions: actions,
    currentEdge: undefined,
    currentNode: 0,
    edges: edges,
    nodes: nodes,
    state: goalieState,
    before: before,
    init: init
};


export class GoalieAutomaton extends Automaton {
    constructor(side: Position, team: TeamName, log: boolean = false) {
        super(side, team, goalieBody, log);
    }
}
