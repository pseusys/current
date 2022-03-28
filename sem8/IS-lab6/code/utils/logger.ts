export class Logger {
    private readonly id: number;
    private readonly team: string;

    constructor(id: number, team: string) {
        this.id = id;
        this.team = team;
    }

    log(message: any) {
        console.log(`(Player ${this.id} of team ${this.team}): ${message}`);
    }
}
