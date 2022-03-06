import * as team from "./layout.json"
import { Agent, Role } from "./agent";
import { TeamName } from "./app";


export function create_players(rightTeam: TeamName, leftTeam: TeamName): Agent[] {
    return team.map((player) => {
        if ((player.team != rightTeam) && (player.team != leftTeam)) throw Error(`Player team name (${player.team}) does not match declared teams (${rightTeam} and ${leftTeam})!`);
        return new Agent(player.team, player.team == rightTeam ? "r" : "l", player.position, player.role as Role);
    });
}
