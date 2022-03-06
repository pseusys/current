import { Command } from "./command";
import { findFlag, Flag, FlagName, getVisible, Instance, Player } from "./locator";
import { Agent } from "./agent";

type TargetName = FlagName | "b" | "p"

export interface Action {
    name: string,
    what: TargetName,
    where?: FlagName
}

export class DecisionTreeState {
    private current: number
    private player: Agent | undefined
    private readonly flow: Action[]
    message: any
    params: Map<string, any>

    private what: Instance | undefined
    private where: Flag | undefined
    private leader: Player | undefined

    constructor(flow: Action[]) {
        this.current = 0;
        this.flow = flow;
        this.params = new Map<string, any>();
    }

    private init() {
        const now = this.flow[this.current];
        this.leader = getVisible(this.message).seenPlayers.find(player => (player.team == this.player!!.teamName) && (player.id == 1));
        this.what = now.what != "b" ? findFlag(this.message, now.what as FlagName) : getVisible(this.message).seenBall;
        if (now.where == undefined) this.where = undefined;
        else this.where = findFlag(this.message, now.where as FlagName);
    }

    setup(message: any, player: Agent) {
        this.player = player;
        this.message = message;
        this.init();
    }

    reset() {
        this.current = 0;
        this.init();
    }

    proceed() {
        this.current++;
        this.init();
    }

    name(): string {
        return this.flow[this.current].name;
    }

    isTeamLeader(): boolean {
        return this.player!!.id == 1
    }

    getRole(): "l" | "r" {
        return this.player!!.id == 2 ? "r" : "l"
    }

    findTeamLeader(): Player | undefined {
        return this.leader;
    }

    findWhat(): Instance | undefined {
        return this.what;
    }

    findWhere(): Instance | undefined {
        return this.where;
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

    constructor(root: DecisionTreeLeaf, state: DecisionTreeState) {
        this.root = root;
        this.state = state;
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

    run(message: any, player: Agent) {
        this.state.setup(message, player);
        return this.execute(this.root);
    }

    reset() {
        this.state.reset();
    }
}
