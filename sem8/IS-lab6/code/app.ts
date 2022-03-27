import { create_players } from "./utils/positioner";

export type TeamName = "team1" | "team2";

let teamName = "team1" as TeamName;
let teamEnemyName = "team2"as TeamName;

create_players(teamName, "l");
setTimeout(() => {
    create_players(teamEnemyName, "r");
}, 2000);
