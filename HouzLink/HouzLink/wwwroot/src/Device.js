function Device(args) {
    this.id = args.id;
    this.name = args.name;
    this.cmd = args.cmd;
    this.payload = args.payload;
    this.value = args.value;
    this.valueDate = args.valueDate;
    this.status = undefined;
    this.deviceType = args.deviceType;
    this.valueStr = args.valueStr;
    this.node = args.node;
    this.includeInUpdate = args.includeInUpdate;
    this.includeInEnviroment = args.includeInEnviroment;

    //client fx
    this.elem = document.querySelector("*[device=\"" + this.id + "\"]");
    if (this.elem !== null)
        this
            .bindAction()
            .setStatus(args.status, true)
            .setValue(args.value, true);
    return this;
};

Device.prototype.bindAction = function () {
    this.elem.classList.add(enums.deviceType[this.deviceType]);
    switch (this.deviceType) {
    case 0: break; //Unknown
        case 2: //Light on/off
            this.elem.addEventListener("click", (function() {
                this.sendCommand(2, this.value === 0 ? 1 : 0);
            }).bind(this), false);
            break; 
        case 3: break;//LightArray

        case 10: //Humidity
        case 11: //Pressure
        case 4:  //Temp
            var sp = document.createElement("span");
            sp.innerHTML = this.value / 100;
            this.elem.appendChild(sp);

            this.elem.addEventListener("click", (function () {
                this.sendCommand(0);
            }).bind(this), false);
            break;   

        case 5: break;   //LightSensor
        case 6: break;   //IrAppliance
        case 7: break;   //AcSwitch
        case 8: break;   //AcTemp
        case 9: break;   //Analog
        case 12: break;  //LedIndicator
        case 13: break;  //Switch
        case 14: break;  //IrReceiver
    }
    return this;
};

Device.prototype.sendCommand = function (command, payload) {
    this.setStatus(2);
    switch (command) {
        case 0: //query
            ui.getText("device/" + this.id + "/query");
            break;
        case 2: //set
            ui.getText("device/" + this.id + "/set/" + payload);
    default:
    }

    return this;
};

Device.prototype.update = function (newVal) {
    return this
            .setStatus(newVal.status)
            .setValue(newVal.value);
};

Device.prototype.setStatus = function (newStatus, redraw) {
    if ((!redraw) && newStatus === this.status) return this;
    this.status = newStatus;
    const st = enums.deviceStatus[this.status];
    if (this.elem.classList.contains(st)) return this;
    for (let cls of enums.deviceStatus) this.elem.classList.remove(cls.toLowerCase());
    this.elem.classList.add(st);
    return this;
};

Device.prototype.setValue = function (newValue, redraw) {
    if ((!redraw) && newValue === this.value) return this;
    this.payload = this.value = newValue;
    switch (this.deviceType) {
        case 2: //light
            console.log("device.setValue [light]:", this.value);
            for (let cls of ["lightOn", "lightOff"]) this.elem.classList.remove(cls);
            this.elem.classList.add(this.value === 0 ? "lightOff" : "lightOn");
                break;
        case 10: //Humidity
        case 11: //Pressure
        case 4:  //Temp
            console.log("device.setValue [env]:", this.value);
            this.elem.querySelector("span").innerText = this.value;
    }
}
