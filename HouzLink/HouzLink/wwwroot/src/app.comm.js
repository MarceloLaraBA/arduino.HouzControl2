"use strict";
const comm = {

    //TODO: handle socket/comm/server
    status: "",
    ready: false,
    elem: undefined,
    check:() => {
        comm.render("connecting to server", "awaiting");
        return ui.getData("comm").then(comm.update);
        },
    update: (cs) => {
        //ready to send/receive..
        comm.ready = (cs.open && cs.link === 2);
        if (comm.ready) return comm.render("online");

        //show error
        switch (cs.link) {
            case 0: return comm.render("com port closed", "error");
            case 1: return comm.render("server booting", "warn");
            case 3: return comm.render("radio offline", "error");
        }
        return comm.render("unknown error", "error");
    },
    render: (st, cls) => {
        if (comm.elem === undefined) {comm.elem = document.getElementById("comm");};
        const el = comm.elem;
        comm.status = st;
        el.innerHTML = st;
        el.className = cls || st;
    }
}