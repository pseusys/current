import { Socket } from "dgram";

export type CommandName = "move" | "init" | "turn" | "dash" | "kick";

export class Command {
    public name: CommandName;
    public value: any;

    constructor(name: CommandName, value: any) {
        this.name = name;
        this.value = value;
    }

    send(socket: Socket) {
        socket.sendMsg(`(${this.name} ${this.value})`);
    }
}
