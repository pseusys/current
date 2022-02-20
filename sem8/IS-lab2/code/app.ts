/*
ЗАДАЧА
подключиться к серверу игры и передать начальные координаты игрока;
проанализировать ответы сервера;
вывести в консоль вычисленные координаты игрока.
*/

import { Agent } from "./agent"; //Импорт агента

let teamName =  "RUSSKIE"; //Имя команды
let teamEnemyName =  "enemy"; //Имя команды
let agent = new Agent(teamName);  //Создание экземпляра агента
let agentEnemy = new Agent(teamEnemyName);  //Создание экземпляра агента

//agent.socketSend("move", '-15 0') // Размещение игрока на поле
//agentEnemy.socketSend("move", `-15 0`) // Размещение игрока на поле

//setInterval(() => {
//    agent.socketSend("turn", 5);
//}, 100);
