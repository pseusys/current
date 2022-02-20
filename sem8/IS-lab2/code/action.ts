export class ActionFlow {
    actions: any[];
    current: number;

    constructor(actions: any[]) {
        this.actions = actions;
        this.current = 0;
    }

    now(): any {
        return this.actions[this.current];
    }
}
