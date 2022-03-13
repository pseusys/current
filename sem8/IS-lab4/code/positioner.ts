import * as team from "./layout.json"
import { Agent, Role } from "./agent";
import { TeamName } from "./app";
import { Coordinate } from "./locator";


export function create_players(rightTeam: TeamName, leftTeam: TeamName): Agent[] {
    const artefacts: number[] = [];
    return team.map((player) => {
        if (player.artefact in artefacts) throw Error(`Artefact ${player.artefact} is not unique!`);
        else artefacts.push(player.artefact);
        if ((player.team != rightTeam) && (player.team != leftTeam)) throw Error(`Player team name (${player.team}) does not match declared teams (${rightTeam} and ${leftTeam})!`);
        return new Agent(player.team, player.artefact, player.team == rightTeam ? "r" : "l", player.position, player.role as Role);
    });
}

export function reset_player(agent: Agent): Coordinate {
    return team.find((player) => { return player.artefact == agent.artefact })!!.position;
}
