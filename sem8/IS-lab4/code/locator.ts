import * as flags from "./flags.json";
import { TeamName } from "./app";

export type FlagName = keyof typeof flags;

export interface Instance {
    distance: number;
    angle: number;
}

export interface Player extends Instance {
    team: TeamName | null;
    id: number | null;
    goalie: boolean | null;
}

export interface Flag extends Instance {
    name: FlagName;
}

export interface Coordinate {
    x: number;
    y: number;
}

function degrees_to_radians(degrees: number): number {
    return degrees * (Math.PI / 180);
}

function radians_to_degrees(radians: number): number {
    return radians * (180 / Math.PI);
}

function sin(ang: number): number {
    return Math.sin(degrees_to_radians(ang));
}

function cos(ang: number): number {
    return Math.cos(degrees_to_radians(ang));
}

function asin(ang: number): number {
    return radians_to_degrees(Math.asin(ang));
}

export function speed(selfPrev: Instance, selfCurr: Instance, semiPrev: Instance, curr: Instance, time: number): Instance {
    //console.log("----- NEW MEASUREMENT -----");
    //console.log(`Self Prev: ${JSON.stringify(selfPrev)}`);
    //console.log(`Self Curr: ${JSON.stringify(selfCurr)}`);
    const b = -2 * selfPrev.distance * cos(selfPrev.angle);
    const D = Math.pow(b, 2) - 4 * (Math.pow(selfPrev.distance, 2) - Math.pow(selfCurr.distance, 2));
    const self = [(-b + Math.sqrt(D)) / 2, (-b - Math.sqrt(D)) / 2];
    //console.log(`Self: ${JSON.stringify(self)}`);
    const real = self.filter(x => x > 0).sort((a, b) => a - b)[0];
    const prevDist = Math.sqrt(Math.pow(semiPrev.distance, 2) + Math.pow(real!!, 2) - 2 * semiPrev.distance * real!! * cos(semiPrev.angle));
    const prev: Instance = { distance: prevDist, angle: asin((semiPrev.distance / prevDist) * sin(semiPrev.angle)) };
    //console.log(`Semi-Prev: ${JSON.stringify(semiPrev)}`);
    //console.log(`Prev: ${JSON.stringify(prev)}`);
    //console.log(`Curr: ${JSON.stringify(curr)}`);
    const diff: Instance = { distance: curr.distance - prev.distance, angle: curr.angle - prev.angle };
    //console.log(`Diff: ${JSON.stringify(diff)}`);
    const th = curr.angle;
    const unit: Instance = { distance: cos(th) + sin(th), angle: cos(th) - sin(th) };
    //console.log(`Unit: ${JSON.stringify(unit)}`);
    const res = { distance: diff.distance * unit.distance, angle: diff.angle * unit.angle };
    //console.log(`Result: ${JSON.stringify(res)}`);
    return { distance: res.distance / time, angle: res.angle / time };
}

export function findFlag(p: any, name: FlagName): Flag | undefined {
    return getVisible(p).seenFlags.find((flag) => flag.name == name );
}

export function getVisible(p: any): { seenFlags: Flag[], seenPlayers: Player[], seenBall: Instance | undefined } {
    const seenFlags: Flag[] = [];
    const seenPlayers: Player[] = [];
    let seenBall: Instance | undefined = undefined;

    // Пройдемся по res.p из app.ts
    if (p) for (let i = 1; i < p.length; ++i) {
        // Если есть "cmd": { "р": [.............] } и есть угол и расстояние
        if (p[i].cmd.p.length >= 0 && p[i].p.length >= 2) {
            // Если игрок видит флаг
            // Запоминаем флаги в формате {название, расстояние, угол}
            if (p[i].cmd.p[0] == "f" || p[i].cmd.p[0] == "g") seenFlags.push({ name: p[i].cmd.p.join(""), distance: p[i].p[0], angle: p[i].p[1] });
            // Если игрок видит игрока и есть угол и расстояние
            else if (p[i].cmd.p[0] == "p" && p[i].cmd.p[1] != undefined) {
                seenPlayers.push({ team: p[i].cmd.p[1].slice(1, -1), id: p[i].cmd.p[2], goalie: p[i].cmd.p[3], distance: p[i].p[0], angle: p[i].p[1] });
            } else if (p[i].cmd.p[0] == "b") seenBall = { distance: p[i].p[0], angle: p[i].p[1] }
        }
    }
    return { seenFlags, seenPlayers, seenBall };
}
