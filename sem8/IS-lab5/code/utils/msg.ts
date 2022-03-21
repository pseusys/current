interface Index {
    idx: number;
}

export function parseMsg(msg: string): any {
    //Разбор сообщения
    if (msg.endsWith("\u0000")) msg = msg.substring(0, msg.length - "\u0000".length); //Удаление символа в конце
    //Разбор сообщения
    let array = msg.match(/(\(|[-\d.]+|[\\"\w]+|\))/g);
    let res = { msg, p: [] };
    if (array == null) return;

    // Анализировать с индекса 0, результат в res
    parse(array, { idx: 0 }, res);
    makeCmd(res); // Выделить команду
    return res;
}

function parse(array: RegExpMatchArray, index: Index, res: any) {
    // Разбор сообщения в скобках
    // Всегда с открывающей скобки
    if (array[index.idx] != "(") return;
    index.idx++;
    // Разбор внутри скобок
    parseInner(array, index, res);
}

function parseInner(array: RegExpMatchArray, index: Index, res: any) {
    // Пока не встретится закрывающая скобка
    while (array[index.idx] != ")") {
        // Если внутри еще одна скобка
        if (array[index.idx] == "(") {
            let r = { p: [] };
            // Рекурсивный вызов с index
            parse(array, index, r);
            res.p.push(r);
        } else {
            // Одиночный параметр
            let num = parseFloat(array[index.idx]);
            res.p.push(isNaN(num) ? array[index.idx] : num);
            index.idx++;
        }
    }
    index.idx++;
}

function makeCmd(res: any) {
    // Выделение команды
    if (res.p && res.p.length > 0) {
        // Первый параметр - команда
        res.cmd = res.p.shift();
        // Выделить команды у параметров
        for (let value of res.p) makeCmd(value);
    }
}
