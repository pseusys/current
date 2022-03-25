import { Act, Automaton, Body, Edge, Node, State, Sync, SyncGuard } from "./automaton";
import { Position } from "../utils/constants";
import { TeamName } from "../utils/app";
import { WorldInfo } from "./locator";
import { Command } from "../utils/command";


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

function kick(info: WorldInfo, state: State): Command | null {
    state.next = true;
    if (!info.ball) return null;
    if (info.ball.dist > 0.5) return null;
    if (info.goal && (info.goal?.angle != null)) return new Command("kick", `60 ${info.goal.angle}`);
    return new Command("kick", "5 45");
}

function lookAround(info: WorldInfo, state: State): Command | null {
    state.next = false;
    state.sync = "lookAround";
    if (!state.local.get("look")) state.local.set("look", "left");
    switch (state.local.get("look")) {
        case "left":
            state.local.set("look", "center");
            return new Command("turn", 90);
        case "center":
            state.local.set("look", "right");
            return new Command("turn", 90);
        case "right":
            state.local.set("look", "back");
            return new Command("turn", 90);
        case "back":
            state.local.set("look", "left");
            state.next = true;
            state.sync = undefined;
            return new Command("turn", 90);
        default:
            state.next = true
            return null;
    }
}

function canIntercept(info: WorldInfo, state: State): boolean {
    state.next = true;
    if (!info.ball) return false;
    return info.ball.dist > 0.5;
}

function runToBall(info: WorldInfo, state: State): Command | null {
    state.next = false;
    if (!info.ball) return new Command("turn", 60);
    if (info.ball.dist <= 0.5) {
        state.next = true;
        return null;
    }
    if (Math.abs(info.ball.angle) > 10) return new Command("turn", info.ball.angle);
    if(info.ball.dist < 0.5) {
        state.next = true;
        return null;
    } else return new Command("dash", 60);
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
actions.set("kick", kick);
actions.set("lookAround", lookAround);
actions.set("canIntercept", canIntercept);
actions.set("runToBall", runToBall);
actions.set("ok", ok);
actions.set("empty", empty);


const variables = new Map<string, any>()
variables.set("dist", null);
variables.set("angle", null);

const timers = new Map<string, number>();
timers.set("t", 0);

const local = new Map<string, number>();


const nodes: Node[] = [
    { name: "start", connected: ["close", "near"] },
    { name: "close", connected: ["kick"] },
    { name: "kick", connected: ["start"] },
    { name: "near", connected: ["intercept", "start"] },
    { name: "intercept", connected: ["start"] }
];


const start_to_close: Edge = {
    from: "start",
    to: "close",
    content: [
        { guard: [
            { left: { type: "var", name: "dist" }, operator: "lte", rightVal: 0.5 }
        ] }
    ]
};

const start_to_near: Edge = {
    from: "start",
    to: "near",
    content: [
        { guard: [
            { leftVal: 0.5, operator: "lte", right: { type: "var", name: "dist" } }
        ] }
    ]
};

const close_to_kick: Edge = {
    from: "close",
    to: "kick",
    content: [
        { sync: "kick" }
    ]
};

const kick_to_start: Edge = {
    from: "kick",
    to: "start",
    content: [ {
        sync: "ok"
    } ]
};

const near_to_start: Edge = {
    from: "near",
    to: "start",
    content: [ {
        sync: "lookAround"
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
        sync: "runToBall"
    } ]
};

const edges: Edge[] = [
    start_to_close,
    start_to_near,
    close_to_kick,
    kick_to_start,
    near_to_start,
    near_to_intercept,
    intercept_to_start
];


const kickerState = {
    local: local,
    next: true,
    sync: undefined,
    timers: timers,
    variables: variables,
    angle: 0
};

const kickerBody: Body = {
    actions: actions,
    currentEdge: undefined,
    currentNode: 0,
    edges: edges,
    nodes: nodes,
    state: kickerState,
    before: before
};


export class KickerAutomaton extends Automaton {
    constructor(side: Position, team: TeamName, log: boolean = false) {
        super(side, team, kickerBody, log);
    }
}
