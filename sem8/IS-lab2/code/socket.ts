import { createSocket, Socket } from "dgram"; // Модуль для работы с UDP

declare module "dgram" {
    interface Socket {
        sendMsg(msg: any): void;
    }
}
Socket.prototype.sendMsg = function(msg: any) {
    //Отправка сообщения серверу
    this.send(Buffer.from(msg), 6000, 'localhost', (err, _) => {
        if (err) throw err;
    });
}

export function create(teamName: string, onMessage: (msg: Buffer) => void, version: number = 7): Socket {
    //Создание сокета
    // в качестве TeamName могут использоваться только два имени (названия первой и второй команд).
    // версия = 7 (не обязательна, но тогда взаимодействие будет осуществ. по максимально доступной)

    const socket = createSocket({ type: 'udp4', reuseAddr: true });
    socket.on("message", (msg, _) => onMessage(msg));

    // Инициализация игрока на сервере (без параметра goalie)
    socket.sendMsg(`(init ${teamName} (version ${version}))`); //подключение игрока к серверу
    return socket;
}
