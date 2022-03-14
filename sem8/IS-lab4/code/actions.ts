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
    private speedCounter = 0

    constructor(flow: Action[]) {
        this.current = 0;
        this.flow = flow;
        this.params = new Map<string, any>();
    }

    private init(newFrame: boolean) {
        const now = this.flow[this.current];
        const kicker = getVisible(this.message).seenPlayers.find(player => (player.team == this.player!!.teamName));
        this.what = now.what != "b" ? findFlag(this.message, now.what as FlagName) : getVisible(this.message).seenBall;
        if (now.where == undefined) this.where = undefined;
        else {
            const nw = now.where != "p" ? findFlag(this.message, now.where as FlagName) : kicker;
            if (newFrame) {
                if (this.speedCounter == 0) {
                    const flags = getVisible(this.message).seenFlags;
                    const seenNames = this.seenFlags.map((fl) => fl.name);
                    const anchor = flags.find((flag) => seenNames.includes(flag.name));
                    if (nw && this.where && anchor) this.whereSpeed = speed(anchor, this.seenFlags.find(flag => flag.name == anchor.name)!!, this.where, nw);
                    this.seenFlags = flags;
                    this.speedCounter = 5;
                } else this.speedCounter--;
            }
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
        // console.log(`Delta: ${JSON.stringify(this.whereSpeed)}`);
        return this.whereSpeed;
    }

    signalDone(): boolean {
        return this.params.get("signal");
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
