"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Belegung = /** @class */ (function () {
    function Belegung(kname, raum, fach, teacher_id) {
        this.KNAME = "";
        this.Raum = "";
        this.Fach = "";
        this.TEACHER_ID = "";
        this.KNAME = kname;
        this.Raum = raum;
        this.Fach = fach;
        this.TEACHER_ID = teacher_id;
    }
    Belegung.prototype.toString = function () {
        var id = this.Raum.replace(".", "");
        var belegtclass = "belegt";
        if (this.KNAME == "frei") {
            belegtclass = "frei";
        }
        return '<div id="c' + id + '" class="col-sm-3 ' + belegtclass + '"><div id="r' + id + '" class="RAUM">Raum ' + this.Raum + '</div>' +
            '<div id="k' + id + '" class="KNAME">' + this.KNAME + '</div>' +
            '<div id="f' + id + '" class="FACH">' + this.Fach + '</div>' +
            '<div id="l' + id + '" class="TEACHER_ID">' + this.TEACHER_ID + '</div></div>\r\n';
    };
    Belegung.prototype.equals = function (b) {
        if (b.Fach == this.Fach && b.KNAME == this.KNAME && b.Raum == this.Raum && b.TEACHER_ID == this.TEACHER_ID) {
            return true;
        }
        return false;
    };
    return Belegung;
}());
exports.Belegung = Belegung;
//# sourceMappingURL=Belegung.js.map