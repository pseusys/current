import { Command } from "./command";
import { findFlag, Flag, FlagName, getVisible, Instance, speed } from "./locator";
import { Agent } from "./agent";

export interface Action {
    name: string,
    what: FlagName | "b",
    where?: FlagName | "p" | "b"
}

export class DecisionTreeState {
    private current: number
    private player: Agent | undefined
    private readonly flow: Action[]
    message: any
    params: Map<string, any>

    private what: Instance | undefined
    private where: Instance | undefined
    private seenFlags: Flag[] = []

    private whereSpeed: Instance | undefined
    private whereStickySpeed: Instance | undefined
    private whereSpeedCounter = 0

    constructor(flow: Action[]) {
        this.current = 0;
        this.flow = flow;
        this.params = new Map<string, any>();
    }

    private init(newFrame: boolean) {
        const now = this.flow[this.current];
        this.what = now.what != "b" ? findFlag(this.message, now.what as FlagName) : getVisible(this.message).seenBall;
        if (now.where == undefined) this.where = undefined;
        else {
            let nw: Instance | undefined;
            switch (now.where) {
                case "b": nw = getVisible(this.message).seenBall; break;
                case "p": nw = getVisible(this.message).seenPlayers.find(player => (player.team == this.player!!.teamName)); break;
                default: nw = findFlag(this.message, now.where as FlagName);
            }
            if (!this.whereStickySpeed) this.whereStickySpeed = nw;
            if (newFrame && nw && this.whereStickySpeed && (nw.distance != this.whereStickySpeed.distance)) {
                const flags = getVisible(this.message).seenFlags;
                const seenNames = this.seenFlags.map((fl) => fl.name);
                const anchor = flags.find((flag) => seenNames.includes(flag.name));
                if (anchor) this.whereSpeed = speed(this.seenFlags.find(flag => flag.name == anchor.name)!!, anchor, this.whereStickySpeed, nw, this.whereSpeedCounter);
                this.seenFlags = flags;
                this.whereStickySpeed = nw;
                this.whereSpeedCounter = 0;
            } else this.whereSpeedCounter++;
            this.where = nw;
        }
    }

    ply(player: Agent) {
        this.player = player;
    }

    msg(message: any) {
        this.message = message;
        this.init(true);
    }

    reset() {
        this.params.clear();
        this.current = 0;
        this.init(true);
    }

    proceed() {
        this.current++;
        this.init(false);
    }

    name(): string {
        return this.flow[this.current].name;
    }

    findWhat(): Instance | undefined {
        return this.what;
    }

    findWhere(): Instance | undefined {
        return this.where;
    }

    findWhereSpeed(): Instance | undefined {
        return this.whereSpeed;
    }
}

export interface DecisionTreeLeaf {
    run?: (state: DecisionTreeState) => void
    command?: (state: DecisionTreeState) => Command
    condition?: (state: DecisionTreeState) => boolean
    resultTrue?: DecisionTreeLeaf
    resultFalse?: DecisionTreeLeaf
    resultNext?: DecisionTreeLeaf
    reset?: (state: DecisionTreeState) => void
}


export class DecisionTree {
    private readonly root: DecisionTreeLeaf
    private readonly state: DecisionTreeState

    constructor(root: DecisionTreeLeaf, state: DecisionTreeState, player: Agent) {
        this.root = root;
        this.state = state;
        this.state.ply(player);
    }

    private execute(leaf: DecisionTreeLeaf): Command {
        if (leaf.run != null) leaf.run(this.state);
        if (leaf.resultNext != null) return this.execute(leaf.resultNext)
        else if ((leaf.condition != null) && (leaf.resultTrue != null) && (leaf.resultFalse != null)) {
            if (leaf.condition(this.state)) return this.execute(leaf.resultTrue);
            else return this.execute(leaf.resultFalse);
        } else if (leaf.command != null) {
            const cmd = leaf.command(this.state);
            if (leaf.reset != null) leaf.reset(this.state);
            return cmd;
        }
        else throw new Error(`Unexpected node in tree: ${JSON.stringify(leaf)}`);
    }

    param(key: string, value: any) {
        this.state.params.set(key, value);
    }

    run(message: any): Command {
        this.state.msg(message);
        return this.execute(this.root);
    }

    reset() {
        this.state.reset();
    }
}
