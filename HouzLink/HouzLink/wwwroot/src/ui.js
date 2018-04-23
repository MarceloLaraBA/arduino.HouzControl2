"use strict";
const ui = {
    apiUri: function (api) { return window.location.origin + "/api/" + (api || ""); },
    getText: function (url, method, data) {
        return new Promise(function (resolve, reject) {
            const xhr = new XMLHttpRequest();
            const d = data ? typeof data==="string"?data:JSON.stringify(data) : "";
            xhr.open(method ? method : "GET", ui.apiUri(url));
            xhr.setRequestHeader("Content-Type", "application/json");
            xhr.onload = function () { if (xhr.status !== 404) resolve(xhr.responseText); else reject(xhr); };
            xhr.onerror = function () { reject(xhr) };
            xhr.send(d);
        });
    },
    getData: function (url, method, data) {
        return this.getText(url, method, data).then(JSON.parse);
    },
    parse: function (htmlString) {
        const elem = document.createElement("div");
        elem.innerHTML = htmlString;
        return elem.firstChild;
    },
    set: function (elemId, str) {
        const elem = document.querySelector(elemId);
        if (elem === undefined || elem === null) return;
        elem.innerHTML = str;
    },
    bind: function (elemId, fx) {
        const elem = document.getElementById(elemId);
        if (elem === undefined || elem === null) return;
        elem.addEventListener("click", fx);
    },
    addLi: function (ul, str) {
        const li = document.createElement("li");
        li.innerHTML = str;
        ul.appendChild(li);
		return li;
    },
    render: function (obj, html) {
        if (obj === undefined || obj === null) return "";
        for (let property in obj) {if (obj.hasOwnProperty(property)) {
            html = html.replace(new RegExp((`{${property}}`), 'g'), obj[property]);
            }
        };
        return html;
    },

    //ux
    collapsableNodes: () => {
        for (const nodeElem of document.querySelectorAll("div.node>h2")) {
            nodeElem.addEventListener("click",
                function () {
                    nodeElem.parentNode.classList.toggle("collapsed");
                });
        }
    }

};

//
String.prototype.replaceAll = function (search, replacement) {
    var target = this;
    return target.replace(new RegExp(search, 'g'), replacement);
};

