export class Belegung {
    KNAME:string="";
    Raum:string="";
    Fach: string="";
    TEACHER_ID: string="";

    constructor(kname:string,raum:string,fach:string,teacher_id:string) {
        this.KNAME=kname;
        this.Raum=raum;
        this.Fach=fach;
        this.TEACHER_ID=teacher_id;
    }

    toString():string {
        var id = this.Raum.replace(".","");
        var belegtclass="belegt"
        if (this.KNAME=="frei") {
            belegtclass="frei"
        }
        return '<div id="c'+id+'" class="col-sm-3 '+belegtclass+'"><div id="r'+id+'" class="RAUM">Raum '+this.Raum+'</div>'+
                    '<div id="k'+id+'" class="KNAME">'+this.KNAME+'</div>'+
                    '<div id="f'+id+'" class="FACH">'+this.Fach+'</div>'+
                    '<div id="l'+id+'" class="TEACHER_ID">'+this.TEACHER_ID+'</div></div>\r\n';
    }

    equals(b:Belegung):boolean {
        if (b.Fach==this.Fach && b.KNAME==this.KNAME && b.Raum==this.Raum && b.TEACHER_ID==this.TEACHER_ID) {
            return true;
        }
        return false;
    }
}