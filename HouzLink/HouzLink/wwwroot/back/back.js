var comm = {
    refresh: () => {
        ui.getData("comm").then((x) => {
            ui.set("comm_port", x.port);
            ui.set("comm_baudrate", x.bitrate);
            ui.set("comm_status", x.open ? "open" : "closed");
            ui.set("comm_link", comm.statusEnm[x.link]);
        });
    },
    connect: () => {
        ui
            .getText("comm", "POST")
            .then(comm.refresh);
    },
    disconnect: () => {
        ui
            .getText("comm", "DELETE")
            .then(comm.refresh);
    },
    statusEnm: ["booting", "online", "offline"],

    sendraw: (x) => {
        ui.getText("comm/send", "POST", document.getElementById("commclient_sendtext").value)
            .then(log.refresh);
    }
};

var log = {
    refresh: () => {
        ui.getData("log").then((x) => {
            const ul = document.getElementById("log");
            ul.innerHTML = "";
            for (let item of x) {
                const li = document.createElement("li");
                li.innerHTML = item;
                ul.appendChild(li);
            }
        });
    }

};

const ui = {
    // FAKE API ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    apiUri: function (api) { return window.location.origin + "/api/" + (api || ""); },
    getText: function (url, method, data) {
        return new Promise(function (resolve, reject) {
            const xhr = new XMLHttpRequest();
            const d = data ? JSON.stringify(data) : "";
            xhr.open(method ? method : "GET", ui.apiUri(url));
            xhr.setRequestHeader("Content-Type", "application/json");
            xhr.onload = function () { if (xhr.status !== 404) resolve(xhr.responseText); else reject(xhr); };
            xhr.onerror = function () { reject(xhr) };
            xhr.send(d);
        });
    },
    getData: function (url, method, data) {
        return this.getText(url, method, data).then(JSON.parse);
    },
    parseElem: function (htmlString) {
        var elem = document.createElement('div');
        elem.innerHTML = htmlString;
        return elem.firstChild;
    },
    set: function(elemId, str) {
        const elem = document.getElementById(elemId);
        if (elem === undefined || elem ===null) return;
        elem.innerHTML = str;
    },
    bind: function(elemId, fx) {
        const elem = document.getElementById(elemId);
        if (elem === undefined || elem === null) return;
        elem.addEventListener("click", fx);
    }
};


(function () {

    //comm
    ui.bind("comm_connect", comm.connect);
    ui.bind("comm_disconnect", comm.disconnect);
    ui.bind("comm_refresh", comm.refresh);

    ui.bind("commclient_sendbutton", comm.sendraw);

    //log
    ui.bind("log_refresh", log.refresh);

})();
