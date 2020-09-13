window.onload = function() {
    const username = localStorage.getItem("tetris.username");
    if (username !== null) window.location = "tetris.html";

    document.getElementById("submit").onclick = function () {
        const username = document.getElementById("input").value;
        if (username) {
            localStorage.setItem("tetris.username", username);
            window.location = "tetris.html";
        }
    }
}
