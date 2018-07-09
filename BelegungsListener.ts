import { Belegung } from "./Belegung";

export interface BelegungsListener {
    belegungChangedListener(belegung:Belegung):void;
}