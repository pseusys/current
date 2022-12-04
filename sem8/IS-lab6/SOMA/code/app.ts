import { create_players } from "./utils/positioner";

// Initialize left team:
create_players("SOMA", "l");

setTimeout(() => {
    // Initialize right team:
    // create_players("non-SOMA", "r");
}, 2000);
