/*
ЗАДАЧА
подключиться к серверу игры и передать начальные координаты игрока;
проанализировать ответы сервера;
вывести в консоль вычисленные координаты игрока.
*/
import { create_players } from "./positioner"; //Импорт агента

export type TeamName = "team1" | "team2";

let teamName = "team1" as TeamName; //Имя команды
let teamEnemyName = "team2"as TeamName; //Имя команды
create_players(teamName, teamEnemyName);  //Создание экземпляров
