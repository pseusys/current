import { Coordinate, Instance, RawFlag } from "./locator";
import * as flags from "./flags.json";
import { isExistCoord } from "./constants";


interface Flag extends Coordinate {
    d: number;
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

function asin(sin: number): number {
    return radians_to_degrees(Math.asin(sin));
}

function acos(cos: number): number {
    return radians_to_degrees(Math.acos(cos));
}


export function distBetween(f1: Instance, f2: Instance): number {
    return Math.sqrt(f1.dist ** 2 + f2.dist ** 2 - 2 * f1.dist * f2.dist * Math.cos(Math.abs(f1.angle - f2.angle)));
}

export function dist(f1: Coordinate, f2: Coordinate): number {
    return Math.sqrt(Math.pow(f1.x - f2.x, 2) + Math.pow(f1.y - f2.y, 2));
}


export function cosine(prev: number, curr: number, len: number): number {
    return acos((Math.pow(len, 2) + Math.pow(curr, 2) - Math.pow(prev, 2)) / (2 * len * curr));
}


export function locate(f1: RawFlag, f2: RawFlag, f3: RawFlag | null = null): Coordinate {
    const flag1 = { x: flags[f1.name].x, y: flags[f1.name].y, d: f1.dist };
    const flag2 = { x: flags[f2.name].x, y: flags[f2.name].y, d: f2.dist };
    const flag3 = (f3) ? { x: flags[f3.name].x, y: flags[f3.name].y, d: f3.dist } : null;
    return triangulate(flag1, flag2, flag3);
}

export function triangulate(f1: Flag, f2: Flag, f3: Flag | null = null): Coordinate {
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
