import { Component } from '@angular/core';
import { NavController} from 'ionic-angular';
import { BLE } from '@ionic-native/ble';
import { NgZone } from '@angular/core';

import {PaginaOpciones} from '../opciones/opciones';

@Component({
  selector: 'pagina-inicio',
  templateUrl: 'inicio.html'
})
export class PaginaInicio {
  public UUID_UART_SERVICE = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";

  private dispositivos:Array<any>;
  private zone:any;
  private buscador;

  constructor(private navCtrl: NavController, private ble: BLE) {
    this.dispositivos = [];
    this.zone = new NgZone({ enableLongStackTrace: false });
    this.start();
    this.buscador = setInterval(this.start,1500);
  }

  private start(){
    this.dispositivos = [];
    var scan = this.ble.scan([this.UUID_UART_SERVICE],1000);
    if(typeof scan !== 'undefined'){
      scan.subscribe((dispositivo)=>{
        this.zone.run(()=>{
          this.dispositivos.push(dispositivo);
          console.info(JSON.stringify(dispositivo));
        });
      });
    }
  }

  public conectar(dispositivo:any){
    clearInterval(this.buscador);
    this.navCtrl.push(PaginaOpciones, {
      dispositivo: dispositivo
    });
  }

}
