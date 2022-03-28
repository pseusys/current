import * as flags from "../utils/flags.json";
import { TeamName } from "../utils/app";
import { Position, SPEED_TIME } from "../utils/constants";
import { cosine, dist, distBetween, locate, speed, triangulate } from "./math";

type FlagName = keyof typeof flags;


export interface Instance {
    dist: number;
    angle: number;
}

interface RawPlayer extends Instance {
    team: TeamName | null;
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

function getCoords(me: Coordinate, object: Instance | undefined, visibleFlags: RawFlag[]): Object | undefined {
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
    private readonly teamName: TeamName;
    private ballSpeedCounter: number;
    private ballSpeedState?: Object;
    private ballSpeedFlags: RawFlag[];
    private ballSpeedAngle: number;
    private me: Coordinate | undefined;
    private validAngle: number;

    time: number;
    signals: Signal[];
    team: Player[];
    enemy: Player[];
    prot?: Object;
    goal?: Object;
    ball?: Object;
    ballSpeed?: Object;

    constructor(side: Position, team: TeamName) {
        this.side = side;
        this.teamName = team;
        this.ballSpeedCounter = 0;
        this.ballSpeedFlags = [];
        this.ballSpeedAngle = 0;
        this.validAngle = 0;

        this.time = 0;
        this.signals = [];
        this.team = [];
        this.enemy = [];
    }

    update(input: any, angle: number): boolean | null {
        this.time = input[0];
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
        if (visible.seenFlags.length < 2) return false;
        const me = locate(visible.seenFlags[0], visible.seenFlags[1], visible.seenFlags[2] ? visible.seenFlags[2] : null);
        if (!me) return false;

        let prot: Object | undefined, goal: Object | undefined;
        visible.seenFlags.forEach(flag => {
            if (flag.name == "gr") {
                if (this.side == "r") prot = getCoords(me, flag, visible.seenFlags);
                else goal = getCoords(me, flag, visible.seenFlags);
            } else if (flag.name == "gl") {
                if (this.side == "l") prot = getCoords(me, flag, visible.seenFlags);
                else goal = getCoords(me, flag, visible.seenFlags);
            }
        });
        if (prot) this.prot = prot;
        else if (this.prot) this.prot = predictPosition(this.prot, this.me!!, me, angle - this.validAngle);
        if (goal) this.goal = goal;
        else if (this.goal) this.goal = predictPosition(this.goal, this.me!!, me, angle - this.validAngle);

        const ball = getCoords(me, visible.seenBall, visible.seenFlags);
        this.ballSpeedCounter++;
        if (ball && (this.ballSpeedCounter == SPEED_TIME)) {
            if (this.ballSpeedState && this.me) {
                const anchor = this.ballSpeedFlags.find(flag => visible.seenFlags.find(seen => flag.name == seen.name))?.name;
                if (anchor) {
                    const currAnchor = visible.seenFlags.find(flag => flag.name == anchor);
                    const prevAnchor = this.ballSpeedFlags.find(flag => flag.name == anchor);
                    prevAnchor!!.angle -= angle - this.ballSpeedAngle;
                    this.ballSpeed = speed(prevAnchor!!, currAnchor!!, this.ballSpeedState, ball, SPEED_TIME);
                }
            }
            this.ballSpeedState = ball;
            this.ballSpeedFlags = visible.seenFlags;
            this.ballSpeedAngle = angle;
            this.ballSpeedCounter = 0;
        }
        if (ball) this.ball = ball;
        else this.ball = undefined;

        this.team = visible.seenPlayers.filter(player => player.team == this.teamName).map(player => getPlayerCoords(me, player, visible.seenFlags));
        this.enemy = visible.seenPlayers.filter(player => player.team != this.teamName).map(player => getPlayerCoords(me, player, visible.seenFlags));

        this.validAngle = angle;
        this.me = me;
        return true;
    }

    private updateHear(input: any) {
        this.signals.push(input[2]);
        // TODO: check
    }

    ballPos(predict: number): Object | undefined {
        if (this.ball && this.ballSpeed) return {
            x: this.ball.x + this.ballSpeed.x * predict,
            y: this.ball.y + this.ballSpeed.y * predict,
            dist: this.ball.dist + this.ballSpeed.dist * predict,
            angle: this.ball.angle + this.ballSpeed.angle * predict
        }; else if (this.ball && (predict == 0)) return this.ball;
        else return undefined;
    }
}
