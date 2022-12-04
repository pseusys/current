import * as flags from "./flags.json";
import { Position } from "./constants";
import { cosine, dist, distBetween, locate, triangulate } from "./math";

type FlagName = keyof typeof flags;


export interface Instance {
    dist: number;
    angle: number;
}

interface RawPlayer extends Instance {
    team: string | null;
    id: number | null;
    goalie: boolean | null;
}

export interface RawFlag extends Instance {
    name: FlagName;
}


export interface Coordinate {
    x: number;
    y: number;
}


interface Signal {
    author: string;
    time: number;
    msg: string;
}


export type Object = Instance & Coordinate

type Player = Coordinate & RawPlayer


function getVisible(p: any): { seenFlags: RawFlag[], seenPlayers: RawPlayer[], seenBall: Instance | undefined } {
    const seenFlags: RawFlag[] = [];
    const seenPlayers: RawPlayer[] = [];
    let seenBall: Instance | undefined = undefined;
    if (p) for (let i = 1; i < p.length; ++i) {
        if (p[i].cmd.p.length >= 0 && p[i].p.length >= 2) {
            if (p[i].cmd.p[0] == "f" || p[i].cmd.p[0] == "g") seenFlags.push({ name: p[i].cmd.p.join(""), dist: p[i].p[0], angle: p[i].p[1] });
            else if (p[i].cmd.p[0] == "p" && p[i].cmd.p[1] != undefined) seenPlayers.push({ team: p[i].cmd.p[1].slice(1, -1), id: p[i].cmd.p[2], goalie: p[i].cmd.p[3], dist: p[i].p[0], angle: p[i].p[1] });
            else if (p[i].cmd.p[0] == "b") seenBall = { dist: p[i].p[0], angle: p[i].p[1] }
        }
    }
    return { seenFlags, seenPlayers, seenBall };
}

function getCoords(me: Coordinate | null, object: Instance | undefined, visibleFlags: RawFlag[]): Object | undefined {
    if (!object) return undefined;
    if (!me) return { ...object, x: NaN, y: NaN }
    const end1 = distBetween(visibleFlags[0], { dist: 0, angle: object.angle });
    const end2 = distBetween(visibleFlags[1], { dist: 0, angle: object.angle });
    const res = triangulate(
        { x: me.x, y: me.y, d: object.dist },
        { x: flags[visibleFlags[0].name].x, y: flags[visibleFlags[0].name].y, d: end1 },
        { x: flags[visibleFlags[1].name].x, y: flags[visibleFlags[1].name].y, d: end2 }
    );
    return { ...res, ...object };
}

function getPlayerCoords(me: Coordinate | null, player: RawPlayer, visibleFlags: RawFlag[]): Player {
    if (!me) return { ...player, x: NaN, y: NaN }
    let coords = getCoords(me, player, visibleFlags)!!;
    return { ...coords, ...player };
}


function predictPosition(base: Object, prev: Coordinate, curr: Coordinate, angle: number): Object {
    const len = dist(curr, base);
    let diff = dist(prev, curr);
    if (diff < 1) diff = Math.round(diff);
    return {
        x: base.x,
        y: base.y,
        dist: len,
        angle: (diff ? 180 - cosine(dist(prev!!, base), len, diff) : base.angle) + angle
    };
}

export class WorldInfo {
    private readonly side: Position;
    private readonly teamName: string;
    private me: Coordinate | undefined;
    private validAngle: number;

    signals: Signal[];
    team: Player[];
    enemy: Player[];
    prot?: Object;
    goal?: Object;
    ball?: Object;
    flags: RawFlag[];
    kickIn: boolean;

    constructor(side: Position, team: string) {
        this.side = side;
        this.teamName = team;
        this.validAngle = 0;

        this.signals = [];
        this.team = [];
        this.enemy = [];
        this.flags = [];
        this.kickIn = false;
    }

    update(input: any, angle: number): boolean | null {
        switch (input.cmd) {
            case "hear":
                this.updateHear(input.p);
                return false;
            case "see":
                return this.updateSee(input.p, angle) ? true : null;
            default: return false;
        }
    }

    private updateSee(input: any, angle: number): boolean {
        const visible = getVisible(input);
        let me: Coordinate | null;
        if (visible.seenFlags.length < 2) me = null;
        else me = locate(visible.seenFlags[0], visible.seenFlags[1], visible.seenFlags[2] ? visible.seenFlags[2] : null);

        let prot: Object | undefined, goal: Object | undefined;
        if (me) visible.seenFlags.forEach(flag => {
            if (flag.name == "gr") {
                if (this.side == "r") prot = getCoords(me, flag, visible.seenFlags);
                else goal = getCoords(me, flag, visible.seenFlags);
            } else if (flag.name == "gl") {
                if (this.side == "l") prot = getCoords(me, flag, visible.seenFlags);
                else goal = getCoords(me, flag, visible.seenFlags);
            }
        });
        if (prot) this.prot = prot;
        else if (this.me && me && this.prot) this.prot = predictPosition(this.prot, this.me, me, angle - this.validAngle);
        else this.prot = undefined;
        if (goal) this.goal = goal;
        else if (this.me && me && this.goal) this.goal = predictPosition(this.goal, this.me, me, angle - this.validAngle);
        else this.goal = undefined;

        const ball = getCoords(me, visible.seenBall, visible.seenFlags);
        if (ball) this.ball = ball;
        else this.ball = undefined;

        this.team = visible.seenPlayers.filter(player => player.team == this.teamName).map(player => getPlayerCoords(me, player, visible.seenFlags));
        this.enemy = visible.seenPlayers.filter(player => player.team != this.teamName).map(player => getPlayerCoords(me, player, visible.seenFlags));

        this.flags = visible.seenFlags;
        this.validAngle = angle;
        this.me = me ?? undefined;
        return !!me;
    }

    private updateHear(input: any) {
        if (input[1] == "referee") this.signals.push(input[2]);
        this.kickIn = (input[2].includes("kick_in")) || (input[2].includes("corner_kick"));
    }
}
