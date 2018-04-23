// setup
(function () {
    ui.collapsableNodes();
    comm
        .check()
        .then(socket.connect)
        .then(devices.getDevices);
})();

