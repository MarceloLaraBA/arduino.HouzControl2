const socket = {
    _socket: undefined,
    connect: () => {
        socket._socket = new Socket({
            onStatusChanged: socket.onStatusChanged,
            onReceive: socket.onReceive,
            onClose: socket.onClose
        });
        socket._socket.connect()
            .then((x) => {
                ui.set("#socket_status", "open");
            });
    },
    onStatusChanged: (x) => {


        console.log("ss.onStatusChanged: ", x);
    },
    onClose: (x) => {
        ui.set("#socket_status", "closed");
        console.log("ss.onClose: ", x);
        setTimeout(socket.connect, 5000);
    },
    onReceive: (x) => {
        const data = JSON.parse(x.data);

        console.log("ss.onReceive: ", data);
        switch (data.Type) {
            case 0: //comm status
                comm.render(JSON.parse(data.Data));
                break;
            case 1: //comm log
                ui.addLi(document.getElementById("comm_log"), data.Data);
                break;
            case 2: //device update
                var dev = JSON.parse(data.Data);
                ui.set("#device_list li[device_id='dev"+dev.Id+"'] .value", dev.Value);
                break;
            default:
                console.log("type:", data.Type);
            }

    }

}


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



(function () {
    ui.bind("device_getall", device.queryall);

    //comm
    ui.bind("comm_connect", comm.connect);
    ui.bind("comm_disconnect", comm.disconnect);
    ui.bind("comm_refresh", comm.refresh);
    ui.bind("commclient_sendbutton", comm.sendraw);

    //log
    ui.bind("log_refresh", log.refresh);


    //socket connect
    socket.connect();

})();
