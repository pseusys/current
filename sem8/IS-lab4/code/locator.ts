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

export function findFlag(p: any, name: FlagName): Flag | undefined {
    return getVisible(p).seenFlags.find((flag) => flag.name == name );
}

export function getVisible(p: any): { seenFlags: Flag[], seenPlayers: Player[], seenBall: Instance | undefined } {
    const seenFlags: Flag[] = [];
    const seenPlayers: Player[] = [];
    let seenBall: Instance | undefined = undefined;

    // Пройдемся по res.p из app.ts
    for (let i = 1; i < p.length; ++i) {
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
