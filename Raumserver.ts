import * as mqtt from "mqtt"
import { MqttClient } from "mqtt";
import { Belegung } from "./Belegung";
import { BelegungsListener } from "./BelegungsListener";
import { MyServer } from ".";

export class Raumserver {
    private mqttClient: MqttClient;
    private listener: BelegungsListener;
    private belegung: Belegung[];


    constructor() {
        this.belegung = new Array();
        this.mqttClient = mqtt.connect("mqtt://"+MyServer.config.mqttServer);
        this.mqttClient.on('connect', () => {
            console.log("connect to MQTT Server:" + this.mqttClient.connected);
            this.mqttClient.subscribe('mmbbs/#', )
        });

        this.mqttClient.on("message", (topic, message) => {
            console.log("topic:" + topic + " message:" + message);
            console.log("belegung=" + this.belegung.length);
    
            var obj = JSON.parse(message.toString())
            let b: Belegung = new Belegung(obj.KNAME, obj.Raum, obj.Fach, obj.TEACHER_ID);
            if (this.belegung["/" + topic] != undefined && !this.belegung["/" + topic].equals(b)) {
                console.log("neue Belegung");
    
                this.belegung["/" + topic] = b;
                if (this.listener) {
                    this.listener.belegungChangedListener(b);
                }
            }
            else if (this.belegung["/" + topic] == undefined) {
                this.belegung["/" + topic] = b;
                if (this.listener) {
                    this.listener.belegungChangedListener(b);
                }
            }    
        });
        console.log("Raumserver erzeugt!");
    }

    setListener(l: BelegungsListener): void {
        this.listener = l;
    }


    getBelegung(topic: string): Belegung[] {
        let out: Belegung[] = [];
        for (var key in this.belegung) {

            if (key.indexOf(topic) != -1) {
                out.push(this.belegung[key]);
            }
        }
        return out;
    }
}