import { Command } from "../utils/command";
import { WorldInfo } from "./locator";
import { Position } from "../utils/constants";
import { TeamName } from "../utils/app";


export type Act = (info: WorldInfo, state: State) => void;

export type Sync = (info: WorldInfo, state: State) => Command | null;

export type SyncGuard = (info: WorldInfo, state: State) => boolean;

interface Reference {
    name: string;
    type: "var" | "timer";
}

interface Assign extends Reference {
    value: any;
}

type Operator = "gt" | "lt" | "gte" | "lte" | "e";

interface Guard {
    left?: Reference;
    leftVal?: number;
    operator: Operator;
    right?: Reference;
    rightVal?: number;
}


export interface State {
    variables: Map<string, any>;
    local: Map<string, any>;
    timers: Map<string, number>;
    next: boolean;
    sync?: string;
    angle: number;
}

export interface Node {
    name: string;
    connected: string[];
}

export interface Edge {
    from: string;
    to: string;
    content: EdgeContent[];
}

interface EdgeContent {
    guard?: Guard[];
    assign?: Assign[];
    sync?: string;
    syncGuard?: string;
}

export interface Body {
    currentNode: number | undefined;
    currentEdge: number | undefined;
    state: State;
    nodes: Node[];
    edges: Edge[];
    actions: Map<string, Act | Sync | SyncGuard>;
    init?: Act;
    before?: Act;
}


export class Automaton {
    private readonly worldInfo: WorldInfo;
    private body: Body;
    private readonly log: boolean;
    private stackCounter = 0;
    private stackTrace = "";

    private lastTime: number;

    constructor(side: Position, team: TeamName, body: Body, log: boolean) {
        this.worldInfo = new WorldInfo(side, team);
        this.body = body;
        this.lastTime = 0;
        this.log = log;
        if (this.body.init) this.body.init(this.worldInfo, body.state);
    }

    getCommand(input: any): Command | null {
        this.stackCounter = 0;
        this.stackTrace = "";
        const update = this.worldInfo.update(input, this.body.state.angle);
        switch (update) {
            case false: return null;
            case null: {
                this.body.state.angle += 60;
                this.body.state.angle %= 360;
                return new Command("turn", 60);
            }
        }
        this.increaseTimers();
        if (this.body.before) this.body.before(this.worldInfo, this.body.state);
        const command = this.execute();
        if (this.log) console.log(`${command?.name} ${command?.value}`);
        if (command?.name == "turn") {
            this.body.state.angle += Number(command.value);
            this.body.state.angle %= 360;
        }
        return command;
    }

    private increaseTimers() {
        if (this.worldInfo.time > this.lastTime) {
            this.lastTime = this.worldInfo.time;
            for (let key of this.body.state.timers.keys()) {
                const val = this.body.state.timers.get(key)!!;
                this.body.state.timers.set(key, val + 1);
            }
        }
    }

    private execute(): Command | null {
        this.stackCounter++;
        try {
            if (this.body.state.sync) {
                const name = this.body.state.sync;
                return (this.body.actions.get(name) as Sync)(this.worldInfo, this.body.state);
            } else if (this.body.state.next) {
                if (this.body.currentNode != undefined) return this.nextState();
                else if (this.body.currentEdge != undefined) return this.nextEdge();
                else throw Error(`Current state (${this.body.state}) doesn't have current!`);
            } else if (this.body.currentNode != undefined) return this.executeState();
            else if (this.body.currentEdge != undefined) return this.executeEdge();
            else throw Error(`Unexpected state: ${this.body.state}`);
        } catch (e) {
            throw Error(`ERROR:\nTrace: ${this.stackTrace}\nState: ${JSON.stringify(this.body.state)}\nStack calls: ${this.stackCounter}\nCurrent edge: ${this.body.currentEdge}\nCurrent node: ${this.body.currentNode}`);
        }
    }

    private nextState(): Command | null {
        const node = this.body.nodes[this.body.currentNode!!];
        for (let name of node.connected) {
            const ind = this.body.edges.findIndex(edge => (edge.from == node.name) && (edge.to == name));
            const edge = this.body.edges[ind];
            if (!edge) throw Error(`Unexpected edge from ${node.name} to ${name}`);

            const condition = edge.content.find(content => {
                if (content.guard) {
                    if (!content.guard.every(rule => this.checkGuard(rule))) return false;
                } else if (content.syncGuard) {
                    if (!this.body.actions.get(content.syncGuard)) throw Error(`Unexpected syncGuard: ${content.syncGuard}`);
                    if (!(this.body.actions.get(content.syncGuard) as SyncGuard)(this.worldInfo, this.body.state)) return false;
                }
                return true;
            });

            if (condition) {
                this.body.currentNode = undefined;
                this.body.currentEdge = ind;
                this.body.state.next = false;
                if (this.log) console.log(`Going to edge ${this.body.currentEdge}`);
                return this.execute();
            }
        }
        return null;
    }

    private nextEdge(): Command | null {
        const name = this.body.edges[this.body.currentEdge!!].to;
        this.body.currentEdge = undefined;
        this.body.currentNode = this.body.nodes.findIndex(node => node.name == name);
        this.body.state.next = false;
        //if (this.log) console.log(`Going to node ${this.body.currentNode}`);
        return this.execute();
    }

    private executeState(): Command | null {
        this.stackTrace += `${this.body.currentNode!!} -> `;
        const node = this.body.nodes[this.body.currentNode!!];
        if (this.log) {
            console.log(`Executing node ${node.name}`);
            //console.log(`Goal: ${JSON.stringify(this.worldInfo.goal)}`);
        }
        if (this.body.actions.get(node.name)) {
            const action = this.body.actions.get(node.name)!!(this.worldInfo, this.body.state);
            if (!action && this.body.state.next) return this.execute();
            else return action as Command;
        } else {
            this.body.state.next = true;
            return this.execute();
        }
    }

    private executeEdge(): Command | null {
        const edges = this.body.edges[this.body.currentEdge!!];
        //if (this.log) console.log(`Executing edge ${edges.from}_to_${edges.to}`);
        for (let content of edges.content) {
            if (content.guard && !content.guard.every(rule => this.checkGuard(rule))) continue;

            if (content.assign) {
                content.assign.forEach(assign => {
                    if (assign.type == "timer") {
                        if (this.body.state.timers.get(assign.name)) this.body.state.timers.set(assign.name, assign.value);
                    } else {
                        if (this.body.state.variables.get(assign.name)) this.body.state.variables.set(assign.name, assign.value);
                    }
                });
            }

            if (content.sync) {
                if (!this.body.actions.get(content.sync)) throw `Unexpected synch: ${content.sync}`;
                const action = (this.body.actions.get(content.sync) as Sync)(this.worldInfo, this.body.state);
                //if (this.log && edges.from == 'intercept' && edges.to == 'start') console.log(JSON.stringify(action));
                if (!action) {
                    this.body.state.next = true;
                    return this.execute();
                } else return action;
            }
        }
        this.body.state.next = true;
        return this.execute();
    }

    private checkGuard(guard: Guard): boolean {
        if (guard.left) guard.leftVal = guard.left.type == "timer" ? this.body.state.timers.get(guard.left.name) : this.body.state.variables.get(guard.left.name);
        if (guard.right) guard.rightVal = guard.right.type == "timer" ? this.body.state.timers.get(guard.right.name) : this.body.state.variables.get(guard.right.name);
        if ((guard.leftVal == undefined) || (guard.rightVal == undefined)) return false;

        switch (guard.operator) {
            case "e": return guard.leftVal == guard.rightVal;
            case "gt": return guard.leftVal!! > guard.rightVal!!;
            case "gte": return guard.leftVal!! >= guard.rightVal!!;
            case "lt": return guard.leftVal!! < guard.rightVal!!;
            case "lte": return guard.leftVal!! <= guard.rightVal!!;
            default: throw Error(`Unexpected guard: ${JSON.stringify(guard)}`);
        }
    }
}
