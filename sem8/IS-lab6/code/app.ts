import { create_players } from "./utils/positioner";

create_players("SOMA", "l");

setTimeout(() => {
    // Init right team, something like:
    create_players("non-SOMA", "r");
}, 2000);
