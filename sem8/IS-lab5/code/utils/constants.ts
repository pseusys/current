import { Coordinate } from "../automaton/locator";

export const FIELD_X = 54;
export const FIELD_Y = 32;

export const SPEED_TIME = 10;

export type Signal = "hear" | "see";
export type Position = "l" | "r";


export function isExistCoord(p: Coordinate) {
    return p.x <= FIELD_X && p.x >= -FIELD_X && p.y <= FIELD_Y && p.y >= -FIELD_Y;
}
