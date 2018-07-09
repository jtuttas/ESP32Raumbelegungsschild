import * as http from "http"
import * as fs from "fs"
import * as WebSocket from 'ws';
import { Raumserver } from "./Raumserver";
import { Belegung } from "./Belegung";
import { BelegungsListener } from "./BelegungsListener";

export class MyServer implements BelegungsListener {
    
    private raumServer: Raumserver;
    private wss: WebSocket.Server;
    public static config;
    constructor() {
        MyServer.config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
        this.raumServer = new Raumserver();
        this.raumServer.setListener(this);
        console.log("raumserver: " + this.raumServer);
        var server: http.Server = http.createServer((request: http.ServerRequest, response: http.ServerResponse) => {
            let header: http.OutgoingHttpHeaders = { 'Content-Type': 'text/html' };
            response.writeHead(200, header);
            console.log("request:", request.url);
            if (request.url == "/") {
                let contents = fs.readFileSync('index.html', 'utf8');
                response.end(contents);
            }
            else {
                let belegung: Belegung[] = this.raumServer.getBelegung(request.url);
                let out = fs.readFileSync('head.html', 'utf8');
                out=out.replace("ws://127.0.0.1:3000",MyServer.config.server+":"+MyServer.config.port)
                let cell: number = 1;
                out = out + '<div class="row">';
                for (const bel of belegung) {
                    out = out + bel.toString();
                    if (cell % 4 == 0) {
                        out = out + '</div><div class="row">';
                    }
                    cell++;
                }
                out = out + "</div></bodys></html>"
                response.end(out);
            }
        });
        this.wss = new WebSocket.Server({ server });

        this.wss.on('connection', (ws: WebSocket, req: Request) => {

            //connection is up, let's add a simple simple event
            ws.on('message', (message: string) => {
                //log the received message and send it back to the client
                console.log('received: %s', message);
                let belegung: Belegung[] = this.raumServer.getBelegung(message);
                ws.send(JSON.stringify(belegung));
            });

            ws.on('error', () => console.log('errored'));

            console.log("WS Connection:" + req.url);

            //send immediatly a feedback to the incoming connection    
            ws.send(JSON.stringify(this.raumServer.getBelegung(req.url)));
        });

        this.wss.on('close', (reasonCode, description) => {
            console.log((new Date()) + ' Peer ' + this.wss.remoteAddress + ' disconnected.');
        });
        server.listen(3000);
        console.log("Web Server starting..");
    }

    belegungChangedListener(belegung: Belegung): void {
        console.log("belegungChangedListener");
        let bel: Belegung[] = new Array();
        bel.push(belegung);
        // Broadcast to all.

        this.wss.clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(JSON.stringify(bel));
            }
        });


    }




}
var myServer = new MyServer();
