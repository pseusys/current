import * as flags from "./flags.json";
type FlagName = keyof typeof flags;

export interface Player {
    distance: number;
    angle: number;
}

export interface Flag {
    name: FlagName;
    distance: number;
    angle: number;
}

interface Point {
    d: number;
    x: number;
    y: number;
}

export function distance (p1: Point, p2: Point) {
    return Math.sqrt((p1.x - p2.x)**2 + (p1.y - p2.y)**2)
}

function isExistCoord(p: Point) {
    return p.x <= 54 && p.x >= -54 && p.y <= 32 && p.y >= -32;
}

export function biangulate(f1: Point, f2: Point, f3: Point | null = null): Point {
    let possiblePoints: Point[] = [];

    if (f1.x == f2.x) {
        const y = (f2.y**2 - f1.y**2 + f1.d**2 - f2.d**2) / (2 * (f2.y - f1.y));
        possiblePoints.push({ x: f1.x + Math.sqrt(f1.d ** 2 - (y - f1.y) ** 2) || 0, y: y, d: 0 });
        possiblePoints.push({ x: f1.x - Math.sqrt(f1.d ** 2 - (y - f1.y) ** 2) || 0, y: y, d: 0 });

    } else if (f1.y == f2.y) {
        const x = (f2.x**2 - f1.x**2 + f1.d**2 - f2.d**2) / (2 * (f2.x - f1.x));
        possiblePoints.push({ x: x, y: f1.y + Math.sqrt(f1.d ** 2 - (x - f1.x) ** 2) || 0, d: 0 });
        possiblePoints.push({ x: x, y: f1.y - Math.sqrt(f1.d ** 2 - (x - f1.x) ** 2) || 0, d: 0 });

    } else {
        const alpha = (f1.y - f2.y) / (f2.x - f1.x);
        const beta = (f2.y**2 - f1.y**2 + f2.x**2 - f1.x**2 + f1.d**2 - f2.d**2) / (2 * (f2.x - f1.x));

        const a = alpha ** 2 + 1;
        const b = -2 * (alpha * (f1.x - beta) + f1.y);
        const c = (f1.x - beta) ** 2 + f1.y ** 2 - f1.d ** 2;

        const resY1 = (-b + Math.sqrt(b ** 2 - 4 * a * c) || 0) / (2 * a);
        const resY2 = (-b - Math.sqrt(b ** 2 - 4 * a * c) || 0) / (2 * a);

        possiblePoints.push({ x: f1.x + Math.sqrt(f1.d ** 2 - (resY1 - f1.y) ** 2) || 0, y: resY1, d: 0 });
        possiblePoints.push({ x: f1.x + Math.sqrt(f1.d ** 2 - (resY2 - f1.y) ** 2) || 0, y: resY2, d: 0 });
        possiblePoints.push({ x: f1.x - Math.sqrt(f1.d ** 2 - (resY1 - f1.y) ** 2) || 0, y: resY1, d: 0 });
        possiblePoints.push({ x: f1.x - Math.sqrt(f1.d ** 2 - (resY2 - f1.y) ** 2) || 0, y: resY2, d: 0 });
    }

    if (f3 != null) {
        possiblePoints = possiblePoints.sort((a, b) => {
            const lossA = Math.abs((a.x - f3.x) ** 2 + (a.y - f3.y) ** 2 - f3.d ** 2)
            const lossB = Math.abs((b.x - f3.x) ** 2 + (b.y - f3.y) ** 2 - f3.d ** 2)
            return lossA - lossB;
        });
    }
    return possiblePoints.find(point => {
        return isExistCoord(point);
    })!!;
}

export function toPoint(flag: Flag): Point {
    return { x: flags[flag.name].x, y: flags[flag.name].y, d: flag.distance }
}

export function distBetween(f1: Point, a1: number, f2: Point, a2: number): number {
    return Math.sqrt(f1.d ** 2 + f2.d ** 2 - 2 * f1.d * f2.d * Math.cos(Math.abs(a1 - a2)));
}
