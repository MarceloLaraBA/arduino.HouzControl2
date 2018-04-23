const devices = {
    _devices: undefined,
    getDevices: () => {
        return ui.getData("device")
            .then((x) => {
                devices._devices = [];
                for (let dev of x)
                    devices._devices.push(new Device(dev));
            });
    },
    update: function(x) {
        var dev = devices._devices.find(d => d.id === x.id);
        if (dev !== undefined && dev !== null) dev.update(x);
    }
}