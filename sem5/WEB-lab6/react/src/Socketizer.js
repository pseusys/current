import io from "socket.io-client";

let user;
let actions;
let socket;
let ongoing = false;

const callbacks = {};

function getCookie(cname) {
    const name = cname + "=";
    const ca = decodeURIComponent(document.cookie).split(';');
    for (let i = 0; i < ca.length; i++) {
        let c = ca[i];
        while (c.charAt(0) === ' ') c = c.substring(1);
        if (c.indexOf(name) === 0) return c.substring(name.length, c.length);
    }
    return "";
}

export async function update () {
    let user_response = await fetch("http://localhost:8081/user?user=" + getCookie("username"));
    user = await user_response.json();
    let actions_response = await fetch("http://localhost:8081/actions");
    actions = await actions_response.json();
    for (const callname in callbacks) callbacks[callname](ongoing, user, actions);
}

export function init () {
    socket = io(":8081");
    socket.on("message", (data) => {
        const p = JSON.parse(data);
        console.log(p.sender + ": " + p.msg);
        if (p.display) alert(p.msg);
    });
    socket.on("update", (data) => {
        const p = JSON.parse(data);
        console.log("Торги " + (p.ongoing ? "" : "не ") + "идут");
        ongoing = p.ongoing;
        update().then(() => { console.log("Состояние обновлено"); });
    });
    socket.on("resolve", () => { update().then(() => { console.log("Обновление завершено") }); });
    socket.emit("join");
}

export function buy (action, quant) {
    socket.emit("buy", JSON.stringify({ user: user.name, action: action, quantity: quant }));
}

export function sell (action, quant) {
    socket.emit("sell", JSON.stringify({ user: user.name, action: action, quantity: quant }));
}

export function set_listener (name, listener) { callbacks[name] = listener; }
