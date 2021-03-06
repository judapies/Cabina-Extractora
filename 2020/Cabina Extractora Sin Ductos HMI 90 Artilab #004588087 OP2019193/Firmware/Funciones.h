void BorraBuffer(void){
RX_Buffer[0]=0;
RX_Buffer[1]=0;
RX_Buffer[2]=0;
RX_Buffer[3]=0;
RX_Buffer[4]=0;
RX_Buffer[5]=0;
RX_Buffer[6]=0;
RX_Buffer[7]=0;
RX_Buffer[8]=0;
RX_Buffer[9]=0;

RX_Buffer2[0]=0;
RX_Buffer2[1]=0;
RX_Buffer2[2]=0;
RX_Buffer2[3]=0;
RX_Buffer2[4]=0;
RX_Buffer2[5]=0;
RX_Buffer2[6]=0;
RX_Buffer2[7]=0;
RX_Buffer2[8]=0;
RX_Buffer2[9]=0;

}

char bgetc(void){
   char c;
   while(RX_Counter==0)
      ;
   c=Rx_Buffer2[RX_Rd_Index];
   if(++RX_Rd_Index>RX_BUFFER_SIZE)
      RX_Rd_Index=0;
   if(RX_Counter)
      RX_Counter--;
   return c;
}

void bputc(char c){
   char restart=0;
   while(TX_Counter> (TX_BUFFER_SIZE-1))
      ;
   if(TX_Counter==0)
      restart=1;
   TX_Buffer[TX_Wr_Index++]=c;
   if(TX_Wr_Index>TX_BUFFER_SIZE)
      TX_Wr_Index=0;
   
   TX_Counter++;
   
   if(restart==1)
      enable_interrupts(int_tbe);
}

void SendDataDisplay(void){
delay_us(10);
putc(0xFF);
delay_us(10);
putc(0xFF);
delay_us(10);
putc(0xFF);
delay_us(10);
}

short esBisiesto(int8 year) {
     return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

void LimitaDia(){
   if(MesTx>12)
      MesTx=10;
   if(DiaTx>31)
      DiaTx=16;
   if(YearTx>99)
      YearTx=18;   
   if(HoraTx>24)
      HoraTx=11;      
   if(MinutoTx>59)
      MinutoTx=30;      
   
      if(MesTx==2){
         if(esBisiesto(YearTx)){
            if(DiaTx>29){
               DiaTx=29;
            }
         }else{
            if(DiaTx>28){
               DiaTx=28;
            }
         }
      }else{
         if(MesTx<=7){
            if(MesTx % 2 ==0){
               if(DiaTx>30){
                  DiaTx=30;                   
               }
            }
         }else{
            if(MesTx % 2 ==1){
               if(DiaTx>30){
                  DiaTx=30; 
               }
            }
         }
      }
}

void LeeDisplay(void){
   if(Dato_Exitoso==5){
         RX_Buffer[0]=RX_Buffer2[0];
         RX_Buffer[1]=RX_Buffer2[1];
         RX_Buffer[2]=RX_Buffer2[2];
         RX_Buffer[3]=RX_Buffer2[3];
         RX_Buffer[4]=RX_Buffer2[4];
         RX_Buffer[5]=RX_Buffer2[5];
         RX_Buffer[6]=RX_Buffer2[6];
         RX_Buffer[7]=RX_Buffer2[7];
         RX_Buffer[8]=RX_Buffer2[8];
         RX_Buffer[9]=RX_Buffer2[9];
         
      if(RX_Buffer[3]==0x01){// Pregunta por la pagina en la que esta el display,01 es Contrase?a de Acceso
         Menu=0;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x02){//02 es Menu Principal 
         Menu=1;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x03){//03 es Tiempo de Purga
         Menu=2;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x04){//04 es Tiempo de Post-Purga
         Menu=3;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x05){//05 es Tiempo de Purga Corriendo
         Menu=4;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x06){//06 es Pantalla Principal
         Menu=5;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x07){//07 es Tiempo de Post-Purga Corriendo
         Menu=6;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x08){//08 es Fecha y hora
         Menu=7;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x09){//09 es off
         Menu=8;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x0a){//0a es TiempoUV
         Menu=9;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x0f){//0f es Recibe caracteres de contrase?a desde display
         
      }else if(RX_Buffer[3]==0x1a){//1a es Mantenimiento
         Menu=10;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x1b){//1b es Menu de clave correcta
         Menu=15;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x1c){//1c es Menu de clave incorrecta
         Menu=16;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x2c){//2c es Menu de Configuracion de Parametros
         Menu=100;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0xcc){//cc es Menu de Bienvenida
         Menu=240;
         if(MenuAntt!=240)
            MenuAntt=Menu;
            //reset_cpu();
      }
   }else{
      for(z=0;z<RX_BUFFER_SIZE;z++){
            //Rx_Buffer[z]=0;
            //Rx_Buffer2[z]=0;
         }
   }
}

Float LeerFlujo(int8 media,int8 canal){
    Flujo[canal].adc=sensores(canal);
      
    if(Flujo[canal].negativo==10){
      x_uno=Flujo[canal].adc+Flujo[canal].Diferencia;
    }
    if(Flujo[canal].negativo==20){
      x_uno=Flujo[canal].adc-Flujo[canal].Diferencia;
    }
    x_cuadrado=x_uno*x_uno;
    x_cubo=x_uno*x_cuadrado;
    Flujo[canal].Velocidad=(x_cubo*a)+(x_cuadrado*b)+(x_uno*c)+d; 
    //Velocidad=Velocidad*correccion;//Ajuste de Temperatura
    Flujo[canal].Velocidad=Flujo[canal].Velocidad-0.08;
    if(Flujo[canal].Velocidad<0.0)
      Flujo[canal].Velocidad=0.0;
     
     if(Flujo[canal].l>media-1)
       {Flujo[canal].l=0;}
     
     Flujo[canal].promedio[Flujo[canal].l]=Flujo[canal].Velocidad;
     Flujo[canal].l++;
     Flujo[canal].Flujo=0;
     
     for(q=0;q<=(media-1);q++){
        Flujo[canal].Flujo+=Flujo[canal].promedio[q];
     } 
     Flujo[canal].Flujo=(Flujo[canal].Flujo/media)*Flujo[canal].Ajuste; 
     return Flujo[canal].Flujo;
}

float D6FW101(int8 media,canal){
   static float a=-0.054947,b=0.56321,c=-0.45502;
   float flow=0.0,promedio=0.0,adc=0.0,Adn=0.47970,AdnV=0.63180,Qdn=0,flowV=0.0;//Areas en m2
   int8 pos=0;
   
   promedio=0.0;
   Qdn=0.0;
   adc=sensores(canal);
   adc=(adc/1023)*5.0;
   flow=(adc*adc*a)+(adc*b)+c;
   flow=flow-0.04;
   if(flow<0.0)
      flow=0.0;
      
   Qdn=Adn*flow;   
   flowV=Qdn/AdnV;
   
   if(pos>media)
      pos=0;
   
   prom[pos]=flowV;
   pos++;
   
   for(i=0;i<=(media-1);i++){
      promedio+=prom[h];
   } 
   promedio=promedio/media;
   return promedio*Flujo[canal].Ajuste;
   //return flowV;

}

float Leer_Sensor_Presion(int8 media,int8 canal){
   Presion[canal].promediopresion=0.0;
   
   Presion[canal].adc=sensores(canal);
   Presion[canal].V1=Presion[canal].adc; 
   Presion[canal].Presion=(Presion[canal].V1/Presion[canal].G)-1.0;// Presion=(Voltaje/Ganancia)-1
   //V1 = (x_uno*5.0)/1023.0;   //Lectura de Divisor de Voltaje de PT100 con resistencia de 1k (+-10%)
   //if(canal==4){
   //   Presion[canal].Presion=(Presion[canal].V1/Gdn)-1.0;// Presion=(Voltaje/Ganancia)-1
   //}else{
   //}
   
   if(Presion[canal].Presion<0.0){
      Presion[canal].Presion=0.0;
   }

   if(Presion[canal].r>media-1)
   {Presion[canal].r=0;}
   Presion[canal].PromPresion[Presion[canal].r]=Presion[canal].Presion*4.02;Presion[canal].r++;
         
   for(q=0;q<=(media-1);q++)
   {
      Presion[canal].promediopresion+=Presion[canal].PromPresion[q];
   } 
   Presion[canal].promediopresion=Presion[canal].promediopresion/media;   
   
   return Presion[canal].promediopresion*Presion[canal].Ajuste;
   
   //if(canal==4){//return Presion[canal].promediopresion*Presion[canal].Ajuste;
   //   return Presion[canal].promediopresion*Ajustedn;
   //}else{
      
   //}
}

float leeTemperatura(int8 media){
   float ADC=0,V=0,T=0,Tempera=0;
   ADC=sensores(0);
   V=5*ADC/1023;
   T=V/0.01;
   if(l2>media)
      l2=0;
      
   promedio2[l2]=T;l2++;
   Tempera=0;
     
     for(h2=0;h2<=(media-1);h2++){
        Tempera+=promedio2[h2];
     }      
   return Tempera/media;
}

void LeeEEPROM(void){
   Password[0]=read_eeprom(0);
   delay_ms(10);
   Password[1]=read_eeprom(1);
   delay_ms(10);
   Password[2]=read_eeprom(2);
   delay_ms(10);
   Password[3]=read_eeprom(3);
   delay_ms(10);
   mPurgap=read_eeprom(4);
   delay_ms(10);
   sPurgap=read_eeprom(5);
   delay_ms(10);
   mPPurgap=read_eeprom(6);
   delay_ms(10);
   sPPurgap=read_eeprom(7);
   delay_ms(10);
   mUVp=read_eeprom(8);
   delay_ms(10);
   sUVp=read_eeprom(9);
   delay_ms(10);
   
   Tuv[0]=read_eeprom(12);
   delay_ms(10);
   Tuv[1]=read_eeprom(13);
   delay_ms(10);
   Tuv[2]=read_eeprom(14);
   delay_ms(10);
   Tuv[3]=read_eeprom(15);
   delay_ms(10);
   Ttrabajo[0]=read_eeprom(16);
   delay_ms(10);
   Ttrabajo[1]=read_eeprom(17);
   delay_ms(10);
   Ttrabajo[2]=read_eeprom(18);
   delay_ms(10);
   Ttrabajo[3]=read_eeprom(19);
   delay_ms(10);
   //flag_alarma=read_eeprom(20);
   //delay_ms(10);
   
   delay_ms(10);
   Entero=read_eeprom(25);
   delay_ms(10);
   Temporal=read_eeprom(26);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Flujo[1].Ajuste=Entero+Temporal;
   
   
   Entero=read_eeprom(27);
   delay_ms(10);
   Temporal=read_eeprom(28);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Flujo[2].Ajuste=Entero+Temporal;
   
   delay_ms(10);
   Entero=read_eeprom(29);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(30);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Presion[4].Ajuste=Entero+Temporal;
   //Ajustedn=Entero+Temporal;
   
   
   Entero=read_eeprom(31);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(32);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Presion[3].Ajuste=Entero+Temporal;
   
   Flujo[1].zero_actual=make16(read_eeprom(34),read_eeprom(33));
   delay_ms(10);
   Flujo[2].zero_actual=make16(read_eeprom(36),read_eeprom(35));
   delay_ms(10);
   
   minutos_trabajo=make16(read_eeprom(38),read_eeprom(37));
   delay_ms(10);
   minutos_uv=read_eeprom(39);
   delay_ms(10);
   Vel1=read_eeprom(40);
   delay_ms(10);
   Vel2=read_eeprom(41);
   delay_ms(10);
   
   Flujo[1].Diferencia=make16(read_eeprom(43),read_eeprom(42));
   delay_ms(10);
   Flujo[2].Diferencia=make16(read_eeprom(45),read_eeprom(44));
   delay_ms(10);
   Presion[4].G=make16(read_eeprom(47),read_eeprom(46));
   //Gdn=make16(read_eeprom(47),read_eeprom(46));
   delay_ms(10);
   Presion[3].G=make16(read_eeprom(49),read_eeprom(48));
   delay_ms(10);
   Flujo[1].negativo=read_eeprom(50);
   delay_ms(10);
   Flujo[2].negativo=read_eeprom(51);
   delay_ms(10);

   Entero=read_eeprom(52);
   delay_ms(10);
   Temporal=read_eeprom(53);
   delay_ms(10);
   Temporal=Temporal/100.0;
   Relacion=Entero+Temporal;
   codigoSubir=read_eeprom(54);
   delay_ms(10);
   codigoBajar=read_eeprom(55);
   delay_ms(10);
   codigoDetener=read_eeprom(56);
   delay_ms(10);
   mediaMovil=read_eeprom(57);
   delay_ms(10);
}

void LimitaValores(void){
   if(Password[0]>9 || Password[0]<0){
      Password[0]=0;write_eeprom(0,0);delay_ms(10);
   }
   if(Password[1]>9 || Password[1]<0){
      Password[1]=0;write_eeprom(1,0);delay_ms(10);
   }
   if(Password[2]>9 || Password[2]<0){
      Password[2]=0;write_eeprom(2,0);delay_ms(10);
   }
   if(Password[3]>9 || Password[3]<0){
      Password[3]=0;write_eeprom(3,0);delay_ms(10);
   }
   if(mPurgap>99){
      mPurgaP=1;write_eeprom(4,1);delay_ms(10);
   }
   if(sPurgap>59){
      sPurgaP=0;write_eeprom(5,0);delay_ms(10);
   }
   if(mPPurgap>99){
      mPPurgaP=1;write_eeprom(6,1);delay_ms(10);
   }
   if(sPPurgap>59){
      sPPurgaP=0;write_eeprom(7,0);delay_ms(10);
   }
   if(mUVp>99){
      mUVP=1;write_eeprom(8,1);delay_ms(10);
   }
   if(sUVp>59){
      sUVP=0;write_eeprom(9,0);delay_ms(10);
   }
   if(Tuv[0]>9 || Tuv[0]<0){
      Tuv[0]=0;write_eeprom(12,0);delay_ms(10);
   }
   if(Tuv[1]>9 || Tuv[1]<0){
      Tuv[1]=0;write_eeprom(13,0);delay_ms(10);
   }
   if(Tuv[2]>9 || Tuv[2]<0){
      Tuv[2]=0;write_eeprom(14,0);delay_ms(10);
   }
   if(Tuv[3]>9 || Tuv[3]<0){
      Tuv[3]=0;write_eeprom(15,0);delay_ms(10);
   }
   if(Ttrabajo[0]>9 || Ttrabajo[0]<0){
      Ttrabajo[0]=0;write_eeprom(16,0);delay_ms(10);
   }
   if(Ttrabajo[1]>9 || Ttrabajo[1]<0){
      Ttrabajo[1]=0;write_eeprom(17,0);delay_ms(10);
   }
   if(Ttrabajo[2]>9 || Ttrabajo[2]<0){
      Ttrabajo[2]=0;write_eeprom(18,0);delay_ms(10);
   }
   if(Ttrabajo[3]>9 || Ttrabajo[3]<0){
      Ttrabajo[3]=0;write_eeprom(19,0);delay_ms(10);
   }
   if(Flujo[1].Ajuste>10.0 || Flujo[1].Ajuste==0.0){
      Flujo[1].Ajuste=1.1;write_eeprom(25,1);delay_ms(10);write_eeprom(26,5);delay_ms(10);
   }
   if(Flujo[2].Ajuste>10.0 || Flujo[2].Ajuste==0.0){
      Flujo[2].Ajuste=1.1;write_eeprom(27,1);delay_ms(10);write_eeprom(28,5);delay_ms(10);
   }
   if(Presion[4].Ajuste>2.0 || Presion[4].Ajuste==0.0){
      Presion[4].Ajuste=1.0;write_eeprom(29,1);delay_ms(10);write_eeprom(30,0);delay_ms(10);
   }
   if(Presion[3].Ajuste>2.0 || Presion[3].Ajuste==0.0){
      Presion[3].Ajuste=1.0;write_eeprom(31,1);delay_ms(10);write_eeprom(32,0);delay_ms(10);
   }
   if(Flujo[1].zero_actual>260 ||Flujo[1].zero_actual<200 ){
      Flujo[1].zero_actual=203;write_eeprom(33,203);delay_ms(10);write_eeprom(34,0);delay_ms(10);
   }
   if(Flujo[2].zero_actual>260 ||Flujo[2].zero_actual<200){
      Flujo[2].zero_actual=203;write_eeprom(35,203);delay_ms(10);write_eeprom(36,0);delay_ms(10);
   }
   if(minutos_trabajo>3600){
      minutos_trabajo=0;write_eeprom(37,0);delay_ms(10);write_eeprom(38,0);delay_ms(10);
   }
   if(minutos_uv>60){
      minutos_uv=0;write_eeprom(39,0);delay_ms(10);
   }
   if(Vel1>6){
      Vel1=2;write_eeprom(40,2);delay_ms(10);
   }   
   if(Vel2>6){
      Vel2=2;write_eeprom(41,2);delay_ms(10);
   }   
   
   if(Flujo[1].Diferencia>80){
      Flujo[1].Diferencia=0;write_eeprom(42,0);delay_ms(10);write_eeprom(43,0);delay_ms(10);
   }
   if(Flujo[2].Diferencia>80){
      Flujo[2].Diferencia=0;write_eeprom(44,0);delay_ms(10);write_eeprom(45,0);delay_ms(10);
   }
   if(Presion[4].G>280 || Presion[4].G<200){
      Presion[4].G=220;write_eeprom(46,220);delay_ms(10);write_eeprom(47,0);delay_ms(10);
   }
   if(Presion[3].G>280 || Presion[3].G<200){
      Presion[3].G=220;write_eeprom(48,220);delay_ms(10);write_eeprom(49,0);delay_ms(10);
   }
   if(Flujo[1].negativo>20){
      Flujo[1].negativo=10;write_eeprom(50,10);delay_ms(10);
   }
   if(Flujo[2].negativo>20){
      Flujo[2].negativo=10;write_eeprom(51,10);delay_ms(10);
   }
   if(Relacion>1.0){
      Relacion=0.57;write_eeprom(52,0);delay_ms(10);write_eeprom(53,57);delay_ms(10);
   }
   if(mediaMovil>80){
      mediaMovil=40;write_eeprom(57,60);delay_ms(10);
   }
}

void ApagaSalidas(){
   Alarma_off;
   LuzBlanca_off;
   LuzUV_off;
   Aux_off;
   Toma_off;
   Motor_off;
   Motor2_off;
}

void dibujaBarraDn(){
   if(barra>=0 && barra<=25){
         printf("j0.val=%u",(int8)((barra*100)/25));
         SendDataDisplay();
         printf("j1.val=0");
         SendDataDisplay();
         printf("j2.val=0");
         SendDataDisplay();
         printf("j3.val=0");
         SendDataDisplay();
      }else if(barra>25 && barra<=50){
         printf("j1.val=%u",(int8)(((barra*100)/25)-100));
         SendDataDisplay();
         printf("j0.val=100");
         SendDataDisplay();
         printf("j2.val=0");
         SendDataDisplay();
         printf("j3.val=0");
         SendDataDisplay();
      }else if(barra>50 && barra<=75){
         printf("j2.val=%u",(int8)(((barra*100)/25)-200));
         SendDataDisplay();
         printf("j0.val=100");
         SendDataDisplay();
         printf("j1.val=100");
         SendDataDisplay();
         printf("j3.val=0");
         SendDataDisplay();
      }else if(barra>75 && barra<=100){
         printf("j3.val=%u",(int8)(((barra*100)/25)-300));
         SendDataDisplay();
         printf("j0.val=100");
         SendDataDisplay();
         printf("j1.val=100");
         SendDataDisplay();
         printf("j2.val=100");
         SendDataDisplay();
      }
}

void dibujaBarraIn(){
   if(barra>=0 && barra<=25){
         printf("j4.val=%u",(int8)((barra*100)/25));
         SendDataDisplay();
         printf("j5.val=0");
         SendDataDisplay();
         printf("j6.val=0");
         SendDataDisplay();
         printf("j7.val=0");
         SendDataDisplay();
      }else if(barra>25 && barra<=50){
         printf("j5.val=%u",(int8)(((barra*100)/25)-100));
         SendDataDisplay();
         printf("j4.val=100");
         SendDataDisplay();
         printf("j6.val=0");
         SendDataDisplay();
         printf("j7.val=0");
         SendDataDisplay();
      }else if(barra>50 && barra<=75){
         printf("j6.val=%u",(int8)(((barra*100)/25)-200));
         SendDataDisplay();
         printf("j4.val=100");
         SendDataDisplay();
         printf("j5.val=100");
         SendDataDisplay();
         printf("j7.val=0");
         SendDataDisplay();
      }else if(barra>75 && barra<=100){
         printf("j7.val=%u",(int8)(((barra*100)/25)-100));
         SendDataDisplay();
         printf("j4.val=100");
         SendDataDisplay();
         printf("j5.val=100");
         SendDataDisplay();
         printf("j6.val=100");
         SendDataDisplay();
      }
}  

void cambiaColor(int16 col){
   printf("Principal.bco=%Lu",col);
   SendDataDisplay(); 
   printf("fecha.bco=%Lu",col);
   SendDataDisplay(); 
   printf("estado.bco=%Lu",col);
   SendDataDisplay(); 
   printf("fex.bco=%Lu",col);
   SendDataDisplay(); 
   printf("fdown.bco=%Lu",col);
   SendDataDisplay(); 
   printf("t3.bco=%Lu",col);
   SendDataDisplay(); 
   printf("t2.bco=%Lu",col);
   SendDataDisplay(); 
   printf("pdown.bco=%Lu",col);
   SendDataDisplay(); 
   printf("pex.bco=%Lu",col);
   SendDataDisplay(); 
}

void bajarVidrio(){
   pulsoSubir=OFF;
   if(VIDRIOUUP || !VIDRIODN){
      O1_on;O2_off;
   }else{
      O1_off;O2_off;
      pulsoBajar=OFF;
   }
}

void subirVidrio(){
   pulsoBajar=OFF;
   if(!VIDRIOUUP || VIDRIODN){
      O1_off;O2_on;
   }else{
      O1_off;O2_off;
      pulsoSubir=OFF;
   }
}

void detenerVidrio(){
   O1_off;O2_off;
}

void leeControlRemoto(){
   if(NUEVO_DATO==1){
      NUEVO_DATO=0;
      BYTE_IR[0]=BIT_BYTE(BITS[0],BITS[1],BITS[2],BITS[3],BITS[4],BITS[5],BITS[6],BITS[7]);
      BYTE_IR[1]=BIT_BYTE(BITS[8],BITS[9],BITS[10],BITS[11],BITS[12],BITS[13],BITS[14],BITS[15]);
      BYTE_IR[2]=BIT_BYTE(BITS[16],BITS[17],BITS[18],BITS[19],BITS[20],BITS[21],BITS[22],BITS[23]);
      BYTE_IR[3]=BIT_BYTE(BITS[24],BITS[25],BITS[26],BITS[27],BITS[28],BITS[29],BITS[30],BITS[31]);
   
      if(BYTE_IR[3]==codigoBajar){ //Bajar Vidrio  //AD
         pulsoBajar=ON;
         RX_Buffer[4]=0x00;
         RX_Buffer2[4]=0x00;
      }
      
      if(BYTE_IR[3]==codigoSubir){ // Subir Vidrio //E7
         pulsoSubir=ON;
         RX_Buffer[4]=0x00;
         RX_Buffer2[4]=0x00;
      }
     
      if(BYTE_IR[3]==codigoDetener){ //F7 E3 A5
         O1_off;O2_off;
         pulsoSubir=OFF;
         pulsoBajar=OFF;
         RX_Buffer[4]=0x00;
         RX_Buffer2[4]=0x00;
      }
   }
   
   if(pulsoBajar)
      bajarVidrio();
      
   if(pulsoSubir)
      subirVidrio();    
     
}

void ubicaVidrio20cm(){  
      if(VIDRIOUUP){
          flag_arriba=ON;
          bajarVidrio(); // Baja Vidrio         
      }
        
      if(flag_arriba){
         bajarVidrio(); // Baja Vidrio         
         
         if(VIDRIOUP || VIDRIODN)
            flag_arriba=OFF;
        
         if(VIDRIOUP && !VIDRIODN)   
            flag_arriba=OFF;
            
         if(VIDRIOUP && VIDRIODN)
            flag_arriba=OFF;
            
         if(!VIDRIOUP && VIDRIODN)
            flag_arriba=OFF;
      }else{
         if(!VIDRIOUP && !VIDRIODN){
            subirVidrio();
         } // Sube Vidrio
        
         if(VIDRIOUP && !VIDRIODN){
            detenerVidrio();
         }
        
         if(VIDRIOUP && VIDRIODN){
            detenerVidrio();
         } // Vidrio quieto
            
         if(!VIDRIOUP && VIDRIODN){
             subirVidrio();
         } // Baja Vidrio
      }
}
