import * as flags from "../utils/flags.json";
import { TeamName } from "../utils/app";
import { FIELD_X, FIELD_Y, Position } from "../utils/constants";

type FlagName = keyof typeof flags;


interface Instance {
    dist: number;
    angle: number;
}

interface RawPlayer extends Instance {
    team: TeamName | null;
    id: number | null;
    goalie: boolean | null;
}

interface RawFlag extends Instance {
    name: FlagName;
}


export interface Coordinate {
    x: number;
    y: number;
}

interface Flag extends Coordinate {
    d: number;
}

interface Signal {
    author: string;
    time: number;
    msg: string;
}

type Ball = Instance & Coordinate

type Player = Coordinate & RawPlayer


function isExistCoord(p: Coordinate) {
    return p.x <= FIELD_X && p.x >= -FIELD_X && p.y <= FIELD_Y && p.y >= -FIELD_Y;
}

function distBetween(f1: Instance, f2: Instance): number {
    return Math.sqrt(f1.dist ** 2 + f2.dist ** 2 - 2 * f1.dist * f2.dist * Math.cos(Math.abs(f1.angle - f2.angle)));
}

function coord(f1: RawFlag, f2: RawFlag, f3: RawFlag | null = null): Coordinate {
    const flag1 = { x: flags[f1.name].x, y: flags[f1.name].y, d: f1.dist };
    const flag2 = { x: flags[f2.name].x, y: flags[f2.name].y, d: f2.dist };
    const flag3 = (f3) ? { x: flags[f3.name].x, y: flags[f3.name].y, d: f3.dist } : null;
    return triangulate(flag1, flag2, flag3);
}

function triangulate(f1: Flag, f2: Flag, f3: Flag | null = null): Coordinate {
    let possiblePoints: Flag[] = [];
    const createQuadDist = (p: number, d: number): number[] => {
        const part = Math.sqrt(f1.d ** 2 - d ** 2) || 0;
        return [p + part, p - part];
    }

    if (f1.x == f2.x) {
        const y = (f2.y**2 - f1.y**2 + f1.d**2 - f2.d**2) / (2 * (f2.y - f1.y));
        possiblePoints.push(...createQuadDist(f1.x, (y - f1.y)).map(val => { return { x: val, y: y, d: 0 }}));

    } else if (f1.y == f2.y) {
        const x = (f2.x**2 - f1.x**2 + f1.d**2 - f2.d**2) / (2 * (f2.x - f1.x));
        possiblePoints.push(...createQuadDist(f1.y, (x - f1.x)).map(val => { return { x: x, y: val, d: 0 }}));

    } else {
        const alpha = (f1.y - f2.y) / (f2.x - f1.x);
        const beta = (f2.y**2 - f1.y**2 + f2.x**2 - f1.x**2 + f1.d**2 - f2.d**2) / (2 * (f2.x - f1.x));

        const a = alpha ** 2 + 1;
        const b = -2 * (alpha * (f1.x - beta) + f1.y);
        const c = (f1.x - beta) ** 2 + f1.y ** 2 - f1.d ** 2;

        const yPart = Math.sqrt(b ** 2 - 4 * a * c) || 0;
        const resY1 = (-b + yPart) / (2 * a);
        const resY2 = (-b - yPart) / (2 * a);

        possiblePoints.push(...createQuadDist(f1.x, (resY1 - f1.y)).map(val => { return { x: val, y: resY1, d: 0 }}));
        possiblePoints.push(...createQuadDist(f1.x, (resY2 - f1.y)).map(val => { return { x: val, y: resY2, d: 0 }}));
    }

    if (f3 != null) possiblePoints = possiblePoints.sort((a, b) => {
        const lossA = Math.abs((a.x - f3.x) ** 2 + (a.y - f3.y) ** 2 - f3.d ** 2);
        const lossB = Math.abs((b.x - f3.x) ** 2 + (b.y - f3.y) ** 2 - f3.d ** 2);
        return lossA - lossB;
    });
    return possiblePoints.find(point => isExistCoord(point))!!;
}


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

function getCoords(me: Coordinate, object: Instance | undefined, visibleFlags: RawFlag[]): Ball | undefined {
    if (!object) return undefined;
    const end1 = distBetween(visibleFlags[0], { dist: 0, angle: object.angle });
    const end2 = distBetween(visibleFlags[1], { dist: 0, angle: object.angle });
    const res = triangulate(
        { x: me.x, y: me.y, d: object.dist },
        { x: flags[visibleFlags[0].name].x, y: flags[visibleFlags[0].name].y, d: end1 },
        { x: flags[visibleFlags[1].name].x, y: flags[visibleFlags[1].name].y, d: end2 }
    );
    return { ...res, ...object };
}

function getPlayerCoords(me: Coordinate, player: RawPlayer, visibleFlags: RawFlag[]): Player {
    let coords = getCoords(me, player, visibleFlags)!!;
    return { ...coords, ...player };
}

export class WorldInfo {
    private teamName: TeamName;
    private panic: boolean;

    time: number;
    signals: Signal[];
    ballPos?: Ball; // If ballPos angle or dist are NaN then x and y are estimated.
    ballSpeed?: Ball;
    team: Player[];
    enemy: Player[];
    prot: RawFlag;
    goal: RawFlag;

    constructor(side: Position, team: TeamName) {
        this.panic = false;
        this.teamName = team;
        this.time = 0;
        this.signals = [];
        this.team = [];
        this.enemy = [];
        this.prot = side == "l" ? { name: "gl", dist: NaN, angle: NaN } : { name: "gr", dist: NaN, angle: NaN };
        this.goal = side == "r" ? { name: "gl", dist: NaN, angle: NaN } : { name: "gr", dist: NaN, angle: NaN };
    }

    update(input: any) {
        this.time = input[0];
        switch (input.cmd) {
            case "hear": this.updateHear(input.p); break;
            case "see": this.updateSee(input.p); break;
        }
    }

    private updateSee(input: any) {
        const visible = getVisible(input);
        if (visible.seenFlags.length < 2) { this.panic = true; return; }
        const me = coord(visible.seenFlags[0], visible.seenFlags[1], visible.seenFlags[2] ? visible.seenFlags[2] : null);

        const prot = visible.seenFlags.find(flag => flag.name == this.prot.name);
        if (prot) this.prot = prot;
        const goal = visible.seenFlags.find(flag => flag.name == this.goal.name);
        if (goal) this.goal = goal;

        const ballPos = getCoords(me, visible.seenBall, visible.seenFlags);
        this.ballSpeed = (ballPos) && (this.ballPos) ? { x: ballPos.x - this.ballPos.x, y: ballPos.y - this.ballPos.y, dist: ballPos.dist - this.ballPos.dist, angle: ballPos.angle - this.ballPos.angle } : undefined;
        const estimated = (this.ballSpeed) && (this.ballPos) ? { x: this.ballPos!!.x + this.ballSpeed.x, y: this.ballPos.y + this.ballSpeed.y, dist: NaN, angle: NaN } : undefined;
        this.ballPos = ballPos ? ballPos : estimated;

        this.team = visible.seenPlayers.filter(player => player.team == this.teamName).map(player => getPlayerCoords(me, player, visible.seenFlags));
        this.enemy = visible.seenPlayers.filter(player => player.team != this.teamName).map(player => getPlayerCoords(me, player, visible.seenFlags));
    }

    private updateHear(input: any) {
        this.signals.push(input[2]);
        // TODO: check
    }
}
