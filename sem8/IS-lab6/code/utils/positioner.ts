import * as team from "../layout.json"
import { Agent, Role } from "../agent";
import { Coordinate } from "./locator";
import { Position } from "./constants";


export function create_players(teamName: string, pos: Position) {
    const artefacts: number[] = [];
    team.forEach((player, idx) => {
        setTimeout(() => {
            if (artefacts.includes(player.artefact)) throw Error(`Artefact ${player.artefact} is not unique!`);
            else artefacts.push(player.artefact);
            return new Agent(teamName, player.artefact, pos, player.position, player.role as Role);
        }, idx * 100);
    });
}

export function reset_player(agent: Agent): Coordinate {
    return team.find((player) => { return player.artefact == agent.artefact })!!.position;
}
