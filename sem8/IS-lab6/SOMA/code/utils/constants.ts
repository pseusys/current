import { Coordinate } from "./locator";

export const FIELD_X = 54;
export const FIELD_Y = 32;

export type Signal = "hear" | "see";
export type Position = "l" | "r";


export function isExistCoord(p: Coordinate) {
    return p.x <= FIELD_X && p.x >= -FIELD_X && p.y <= FIELD_Y && p.y >= -FIELD_Y;
}
