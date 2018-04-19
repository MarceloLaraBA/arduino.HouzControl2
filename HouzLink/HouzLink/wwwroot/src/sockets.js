var Socket = function (args) {

    //webSocket url
    this.uri = args.uri;
    if (this.uri === undefined) this.uri = "/ws";
    this._wsUri = (document.location.protocol === "https:" ? "wss" : "ws") + "://" +
        document.location.hostname + (document.location.port ? (`:${document.location.port}`) : "") +
        this.uri;

    //events
    this.onReceive = args.onReceive;
    this.onStatusChanged = args.onStatusChanged;
    this.onClose = args.onClose;

    //todo: make private
    this._ws = undefined;

};


Socket.prototype = {

    statusChange: function () {
        //console.log("socket.state> ", this._ws.readyState);
    },

    connect: function () {
        var o = this;
        return new Promise(function (resolve, reject) {

            //skip connection if already connected
            if (o._ws !== undefined && o._ws.readyState === WebSocket.OPEN) {
                resolve(o._ws);
                return;
            };

            //create socket
            //console.log("socket.connect> connecting to", o._wsUri, "..");
            o._ws = new WebSocket(o._wsUri);

            //bind events
            o._ws.onclose = (ev) => { if (typeof o.onClose === "function") o.onClose(); };
            o._ws.onmessage = (ev) => {
                //console.log("socket.receive>", ev.data);
                try {
                    if (typeof o.onReceive === "function") 
                        o.onReceive(JSON.parse(ev.data));
                } catch (e) {
                    console.log("socket.receive> [unknown]", ev.data);
                }
            };

            //handle promise
            o._ws.onopen = () => { resolve(o._ws) };
            o._ws.onerror = (err) => { reject(err) };
        });
    },

    close: function () {
        if (this._ws === undefined || this._ws.readyState !== WebSocket.OPEN) return;
        this._ws.close(1000, "close from client");
    },

    send: function (data) {
        if (typeof data !== "object") {
            console.log("!ERROR: not an object! socket.send> ", data);
            return null;
        }
        return this.connect()
            .then((x) => {
                console.log("socket.send> ", data);
                x.send(JSON.stringify(data));
                return x;
            });
    }




};

