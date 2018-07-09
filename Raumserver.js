"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var mqtt = require("mqtt");
var Belegung_1 = require("./Belegung");
var _1 = require(".");
var Raumserver = /** @class */ (function () {
    function Raumserver() {
        var _this = this;
        this.belegung = new Array();
        this.mqttClient = mqtt.connect("mqtt://" + _1.MyServer.config.mqttServer);
        this.mqttClient.on('connect', function () {
            console.log("connect to MQTT Server:" + _this.mqttClient.connected);
            _this.mqttClient.subscribe('mmbbs/#');
        });
        this.mqttClient.on("message", function (topic, message) {
            console.log("topic:" + topic + " message:" + message);
            console.log("belegung=" + _this.belegung.length);
            var obj = JSON.parse(message.toString());
            var b = new Belegung_1.Belegung(obj.KNAME, obj.Raum, obj.Fach, obj.TEACHER_ID);
            if (_this.belegung["/" + topic] != undefined && !_this.belegung["/" + topic].equals(b)) {
                console.log("neue Belegung");
                _this.belegung["/" + topic] = b;
                if (_this.listener) {
                    _this.listener.belegungChangedListener(b);
                }
            }
            else if (_this.belegung["/" + topic] == undefined) {
                _this.belegung["/" + topic] = b;
                if (_this.listener) {
                    _this.listener.belegungChangedListener(b);
                }
            }
        });
        console.log("Raumserver erzeugt!");
    }
    Raumserver.prototype.setListener = function (l) {
        this.listener = l;
    };
    Raumserver.prototype.getBelegung = function (topic) {
        var out = [];
        for (var key in this.belegung) {
            if (key.indexOf(topic) != -1) {
                out.push(this.belegung[key]);
            }
        }
        return out;
    };
    return Raumserver;
}());
exports.Raumserver = Raumserver;
//# sourceMappingURL=Raumserver.js.map