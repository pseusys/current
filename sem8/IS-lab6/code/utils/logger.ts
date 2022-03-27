import { TeamName } from "../app";


export class Logger {
    private readonly id: number;
    private readonly team: TeamName;

    constructor(id: number, team: TeamName) {
        this.id = id;
        this.team = team;
    }

    log(message: any) {
        console.log(`(Player ${this.id} of team ${this.team}): ${message}`);
    }
}
