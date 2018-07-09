"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var http = require("http");
var fs = require("fs");
var WebSocket = require("ws");
var Raumserver_1 = require("./Raumserver");
var MyServer = /** @class */ (function () {
    function MyServer() {
        var _this = this;
        MyServer.config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
        this.raumServer = new Raumserver_1.Raumserver();
        this.raumServer.setListener(this);
        console.log("raumserver: " + this.raumServer);
        var server = http.createServer(function (request, response) {
            var header = { 'Content-Type': 'text/html' };
            response.writeHead(200, header);
            console.log("request:", request.url);
            if (request.url == "/") {
                var contents = fs.readFileSync('index.html', 'utf8');
                response.end(contents);
            }
            else {
                var belegung = _this.raumServer.getBelegung(request.url);
                var out = fs.readFileSync('head.html', 'utf8');
                out = out.replace("ws://127.0.0.1:3000", MyServer.config.server + ":" + MyServer.config.port);
                var cell = 1;
                out = out + '<div class="row">';
                for (var _i = 0, belegung_1 = belegung; _i < belegung_1.length; _i++) {
                    var bel = belegung_1[_i];
                    out = out + bel.toString();
                    if (cell % 4 == 0) {
                        out = out + '</div><div class="row">';
                    }
                    cell++;
                }
                out = out + "</div></bodys></html>";
                response.end(out);
            }
        });
        this.wss = new WebSocket.Server({ server: server });
        this.wss.on('connection', function (ws, req) {
            //connection is up, let's add a simple simple event
            ws.on('message', function (message) {
                //log the received message and send it back to the client
                console.log('received: %s', message);
                var belegung = _this.raumServer.getBelegung(message);
                ws.send(JSON.stringify(belegung));
            });
            ws.on('error', function () { return console.log('errored'); });
            console.log("WS Connection:" + req.url);
            //send immediatly a feedback to the incoming connection    
            ws.send(JSON.stringify(_this.raumServer.getBelegung(req.url)));
        });
        this.wss.on('close', function (reasonCode, description) {
            console.log((new Date()) + ' Peer ' + _this.wss.remoteAddress + ' disconnected.');
        });
        server.listen(3000);
        console.log("Web Server starting..");
    }
    MyServer.prototype.belegungChangedListener = function (belegung) {
        console.log("belegungChangedListener");
        var bel = new Array();
        bel.push(belegung);
        // Broadcast to all.
        this.wss.clients.forEach(function (client) {
            if (client.readyState === WebSocket.OPEN) {
                client.send(JSON.stringify(bel));
            }
        });
    };
    return MyServer;
}());
exports.MyServer = MyServer;
var myServer = new MyServer();
//# sourceMappingURL=index.js.map