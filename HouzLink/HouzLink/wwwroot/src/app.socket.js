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
                console.log("socket.connect:", x)
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
        var data;
        try {data = JSON.parse(x.data)} catch (e) {data = x.data}; 
        switch (x.messageType) {
        case 3: //comm log
            console.log("comm:", x.data);
            break;

        case 4: //comm status
            console.log("socket.receive.comm", data);
            comm.update(data);
            break;

        case 5: //device update
            devices.update(data);
            break;
        default:
                console.log("unhandled MessageType:", x.messageType);
        }

    }

}