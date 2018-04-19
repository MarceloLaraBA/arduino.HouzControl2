const comm = {
    refresh: () => {
        ui.getData("comm").then(comm.render);
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
    },
    render: (x) => {
        ui.set("#comm_port", x.port);
        ui.set("#comm_baudrate", x.bitrate);
        ui.set("#comm_status", x.open ? "open" : "closed");
        ui.set("#comm_link", comm.statusEnm[x.link]);
    }
};
