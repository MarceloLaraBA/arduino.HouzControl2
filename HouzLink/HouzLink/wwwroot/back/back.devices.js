const device = {

    _device_template:
        "<li device_id=\"dev{id}\">" +
            "<h3 class= \"{stat}\">" +
                "<span class=\"name\">{name}</span>" +
                "<span class=\"value\">{value}</span>" +
            "</h3 >" +
            "<ul role=\"toolbar\">" +
                "<li class=\"device_query\">query</li>" +
                "<li class=\"device_setvalue nobtn\"><input type=\"text\" value=\"{value}\" placeholder=\"payload\" name=\"payload\" /></li>"+
                "<li class=\"device_set\">set</li>"+
            "</ul>" +
        "</li >",

    queryall: () => {
        ui.getData("device").then((x) => {
            const ul = document.getElementById("device_list");
            for (let dev of x) {
                const li = ui.parse(
                    ui
                        .render(dev, device._device_template)
                    .replaceAll("{stat}", enums.deviceStatus[dev.status])
                );
                li.querySelector(".device_query").addEventListener("click", (x) => {
                    device.query(dev.id);
                });
                li.querySelector(".device_set").addEventListener("click", (x) => {
                    device.set_value(
                        dev.id,
                        li.querySelector("input[name='payload']").value);

                });
                ul.appendChild(li);
            }
        });
    },

    query: (x) => {
        ui.getData("device/" + x + "/query")
            .then((b) => {
                console.log(b);
            });
    },

    set_value: (dev, val) => {
        console.log("dev:", dev, "|payload:", val);
    }

};
