const enums = {
    statusEnm: ["comClosed", "booting", "online", "offline"],

    //from server
    "deviceStatus": ["unknown", "ok", "awaiting", "error", "nodedown"],
    "command": ["QUERY", "VALUE", "SET", "EVENT", "STATUS"],
    "deviceType": [
        "Unknown", "node", "light", "lightarray", "temperature", "lightsensor", "irappliance", "acswitch", "actemp", "analog",
        "humidity", "pressure", "ledindicator", "switch", "irreceiver"
    ]
};