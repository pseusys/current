const opener = $("#opener");
const dialog = $("#dialog");
const user = dialog.attr("user");

let socket;
let pivot_time;
let ticking_timer = undefined;
let countdown = undefined;



opener.button();

dialog.dialog({
    autoOpen: false,
    show: {
        effect: "blind",
        duration: 100
    },
    hide: {
        effect: "blind",
        duration: 100
    },
    open: () => {
        part_user(true, user);
        socket = io();
        init_socket();
        socket.emit("join");
    },
    close: (event, ui) => {
        clearTimeout(ticking_timer);
        socket.disconnect();
        socket = undefined;
    },
    width: "800",
    height: "450"
});



function create_log_entry (string, color = undefined) {
    const elem = $("<p></p>").text(string);
    if (!!color) elem.css("color", color);
    $("#log").append(elem);
    console.log(string);
}

function init_socket () {
    socket.on("time", (time) => {
        pivot_time = Date.parse(time);
        if (pivot_time > new Date()) {
            $("#pre-trades").css("display", "flex");
            $("#trades").css("display", "none");
        }
        ticking_timer = setInterval(() => {
            $(".ticking").text(Math.trunc(Math.abs(pivot_time - new Date()) / 1000));
            if (!!countdown) {
                $("#timeout").text(countdown);
                countdown--;
                if (countdown === 0) countdown = undefined;
            } else $("#timeout").text("");
        }, 1000);
    });

    socket.on("start", () => {
        $("#pre-trades").css("display", "none");
        $("#trades").css("display", "flex");
        create_log_entry("Trades started!", "green");
    });

    socket.on("message", (data) => {
        const p = JSON.parse(data);
        create_log_entry(p.sender + ": " + p.msg);
    });

    socket.on("new_book", (book) => {
        const p = JSON.parse(book);
        console.log(p);
        create_log_entry("New book: " + p.name + " by " + p.author, "brown");
        $("#trading_book_name").text(p.name);
    });

    socket.on("trading", (data) => {
        const p = JSON.parse(data);
        create_log_entry("Trading begins, starting price: " + p.price +
            ", minimal step: " + p.min_step +
            ", maximal step: " + p.max_step + "!");
        $("#bet_input").val(p.price);
    });

    socket.on("countdown", (interval) => {
        countdown = Number.parseInt(interval);
    });

    socket.on("new_price", (data) => {
        const p = JSON.parse(data);
        create_log_entry(p.user + " suggested " + p.money + " moneys", "red");
    });

    socket.on("over", () => {
        create_log_entry("Trades ended!", "red");
        $("#post-trades").css("display", "flex");
        $("#trades").css("display", "none");
    });
}



$("#bet_button").click(() => {
    const money = $("#bet_input").val();
    socket.emit("message", JSON.stringify({ sender: user, msg: "betting " + money }));
    create_log_entry("Your bet: " + money);
    socket.emit("bet", JSON.stringify({ user: user, money: money }));
});

$("#message_button").click(() => {
    const msg = $("#message_input").val();
    socket.emit("message", JSON.stringify({ sender: user, msg: msg }));
    create_log_entry(user + ": " + msg);
});

opener.click(() => { dialog.dialog("open"); });
