void Envio_I2C(direccion, posicion, dato){

   i2c_start();            // Comienzo comunicaci?n
   i2c_write(direccion);   // Direcci?n del esclavo en el bus I2C
   i2c_write(posicion);    // Posici?n donde se guardara el dato transmitido
   i2c_write(dato);        // Dato a transmitir
   i2c_stop();             // Fin comunicaci?n
 }

void Lectura_I2C (byte direccion, byte posicion, byte &dato) {

   i2c_start();            // Comienzo de la comunicaci?n
   i2c_write(direccion);   // Direcci?n del esclavo en el bus I2C
   i2c_write(posicion);    // Posici?n de donde se leer? el dato en el esclavo
   i2c_start();            // Reinicio
   i2c_write(direccion+1); // Direcci?n del esclavo en modo lectura
   dato=i2c_read(0);       // Lectura del dato
   i2c_stop();             // Fin comunicaci?n
}

void Carga_Vector(void){
   SlaveA0Tx[0]=instruccion;
   SlaveA0Tx[1]=address;
   SlaveA0Tx[2]=regl;
   SlaveA0Tx[3]=regh;
   SlaveA0Tx[4]=pwm1l;
   SlaveA0Tx[5]=pwm1h;
   SlaveA0Tx[6]=pwm2l;
   SlaveA0Tx[7]=pwm2h;
   SlaveA0Tx[8]=txbuf[0];
   SlaveA0Tx[9]=txbuf[19];
}

void Lee_Vector(void){
   
}

void Envio_Esclavos(void){
   Carga_Vector();   
   for(i=0;i<10;i++)
   {
      direccion=0xB0;
      Envio_I2C(direccion,i,SlaveA0Tx[i]);
   } 
}
void Lectura_Esclavos(void){
   Lectura_I2C(0xB0, 0, dato);    //Lectura por I2C
   txbuf[0]=(int8)dato;
   Lectura_I2C(0xB0, 19, dato);    //Lectura por I2C
   txbuf[19]=(int8)dato;
   if(txbuf[0]==10 && txbuf[19]==128){
      for(i=1;i<20;i++){
         direccion=0xB0;                        //Direcci?n en el bus I2c
         posicion=i;                         //Posici?n de memoria a leer
         Lectura_I2C(direccion, posicion, dato);    //Lectura por I2C
         txbuf[i]=(int8)dato;
      }
      Lee_Vector();   
   }
}


void readHolding(){
   instruccion=3;
   address=1;
   regl=0;
   regh=0;
   Envio_Esclavos();
   delay_ms(200);
   Lectura_Esclavos(); 
}

float leeTemperatura(int8 media){
   float ADC=0,V=0,T=0,Tempera=0;
   ADC=sensores(2);
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
   Tpurgap[0]=read_eeprom(4);
   delay_ms(10);
   Tpurgap[1]=read_eeprom(5);
   delay_ms(10);
   Tpurgap[2]=read_eeprom(6);
   delay_ms(10);
   Tpurgap[3]=read_eeprom(7);
   delay_ms(10);
   Tppurgap[0]=read_eeprom(8);
   delay_ms(10);
   Tppurgap[1]=read_eeprom(9);
   delay_ms(10);
   Tppurgap[2]=read_eeprom(10);
   delay_ms(10);
   Tppurgap[3]=read_eeprom(11);
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
   Tempouvp[0]=read_eeprom(21);
   delay_ms(10);
   Tempouvp[1]=read_eeprom(22);
   delay_ms(10);
   Tempouvp[2]=read_eeprom(23);
   delay_ms(10);
   Tempouvp[3]=read_eeprom(24);
   
   delay_ms(10);
   Entero=read_eeprom(25);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(26);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Ajuste1=Entero+Temporal;
   
   
   Entero=read_eeprom(27);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(28);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Ajuste2=Entero+Temporal;
   
   zero_actual=make16(read_eeprom(30),read_eeprom(29));
   delay_ms(10);
   minutos_trabajo=make16(read_eeprom(34),read_eeprom(33));
   delay_ms(10);
   minutos_uv=read_eeprom(31);
   delay_ms(10);
   VelMotor=read_eeprom(32);
   delay_ms(10);
   
   Diferencia=make16(read_eeprom(61),read_eeprom(60));
   delay_ms(10);
   G=make16(read_eeprom(63),read_eeprom(62));
   delay_ms(10);
   negativo=read_eeprom(50);
   delay_ms(10);

}

void LimitaValores(void){
   if(Password[0]>9 || Password[0]<0){
      Password[0]=0;write_eeprom(0,0);
   }
   if(Password[1]>9 || Password[1]<0){
      Password[1]=0;write_eeprom(1,0);
   }
   if(Password[2]>9 || Password[2]<0){
      Password[2]=0;write_eeprom(2,0);
   }
   if(Password[3]>9 || Password[3]<0){
      Password[3]=0;write_eeprom(3,0);
   }
   if(Tpurgap[0]>9 || Tpurgap[0]<0){
      Tpurgap[0]=0;write_eeprom(4,0);
   }
   if(Tpurgap[1]>9 || Tpurgap[1]<0){
      Tpurgap[1]=0;write_eeprom(5,0);
   }
   if(Tpurgap[2]>9 || Tpurgap[2]<0){
      Tpurgap[2]=2;write_eeprom(6,2);
   }
   if(Tpurgap[3]>9 || Tpurgap[3]<0){
      Tpurgap[3]=0;write_eeprom(7,0);
   }
   if(Tppurgap[0]>9 || Tppurgap[0]<0){
      Tppurgap[0]=0;write_eeprom(8,0);
   }
   if(Tppurgap[1]>9 || Tppurgap[1]<0){
      Tppurgap[1]=0;write_eeprom(9,0);
   }
   if(Tppurgap[2]>9 || Tppurgap[2]<0){
      Tppurgap[2]=2;write_eeprom(10,2);
   }
   if(Tppurgap[3]>9 || Tppurgap[3]<0){
      Tppurgap[3]=0;write_eeprom(11,0);
   }
   if(Tuv[0]>9 || Tuv[0]<0){
      Tuv[0]=0;write_eeprom(12,0);
   }
   if(Tuv[1]>9 || Tuv[1]<0){
      Tuv[1]=0;write_eeprom(13,0);
   }
   if(Tuv[2]>9 || Tuv[2]<0){
      Tuv[2]=0;write_eeprom(14,0);
   }
   if(Tuv[3]>9 || Tuv[3]<0){
      Tuv[3]=0;write_eeprom(15,0);
   }
   if(Ttrabajo[0]>9 || Ttrabajo[0]<0){
      Ttrabajo[0]=0;write_eeprom(16,0);
   }
   if(Ttrabajo[1]>9 || Ttrabajo[1]<0){
      Ttrabajo[1]=0;write_eeprom(17,0);
   }
   if(Ttrabajo[2]>9 || Ttrabajo[2]<0){
      Ttrabajo[2]=0;write_eeprom(18,0);
   }
   if(Ttrabajo[3]>9 || Ttrabajo[3]<0){
      Ttrabajo[3]=0;write_eeprom(19,0);
   }
   if(Tempouvp[0]>9 || Tempouvp[0]<0){
      Tempouvp[0]=0;write_eeprom(21,0);
   }
   if(Tempouvp[1]>9 || Tempouvp[1]<0){
      Tempouvp[1]=0;write_eeprom(22,0);
   }
   if(Tempouvp[2]>9 || Tempouvp[2]<0){
      Tempouvp[2]=0;write_eeprom(23,0);
   }
   if(Tempouvp[3]>9 || Tempouvp[3]<0){
      Tempouvp[3]=1;write_eeprom(24,1);
   }
   if(Ajuste1>25.0){
      Ajuste1=1.5;write_eeprom(25,1);write_eeprom(26,5);
   }
   if(Ajuste2>25.0){
      Ajuste2=1.0;write_eeprom(27,1);write_eeprom(28,0);
   }
   if(zero_actual>300){
      zero_actual=203;write_eeprom(29,203);write_eeprom(30,0);
   }
   if(minutos_uv>60){
      minutos_uv=0;write_eeprom(31,0);
   }
   if(VelMotor>100 || VelMotor==0){
      VelMotor=50;write_eeprom(32,50);
   }   
   if(minutos_trabajo>3600){
      minutos_trabajo=0;write_eeprom(33,0);write_eeprom(34,0);
   }
   //if(Relacion>2.0){
      //Relacion=0.57;write_eeprom(35,0);write_eeprom(36,57);
   //}
   if(Diferencia>200){
      Diferencia=0;write_eeprom(60,0);write_eeprom(61,0);
   }
   if(G>900){
      G=240;write_eeprom(62,240);write_eeprom(63,0);
   }
   if(negativo>20){
      negativo=10;write_eeprom(50,10);
   }
}

void VelocidadMotor(int8 Vel){
   float tmp;
   if(Vel==0){
      Motor_off;
      Alarmas=OFF;
      flagError=OFF;
      conteoError=0;
   }else{
      #ifdef RS485
      Motor_on;
      tmp=((float)Vel/(1.6666666))*10.0;
      readHolding(); 
      reg=(int16)tmp;
      if(reg!=make16(txbuf[3],txbuf[4])){
      //if(tmp!=inst){ 
         instruccion=5;
         address=100;
         //reg=(int16)tmp;
         regl=make8(reg,0);
         regh=make8(reg,1);
         Envio_Esclavos();
         conteoError++;
         flagError=ON;
         if(conteoError>10)
            Alarmas=ON;
      //}
      }else{
         if(flagError){
            flagError=OFF;
            conteoError=0;
            Alarmas=OFF;
            instruccion=0;
            address=0;
            regl=0;
            regh=0;
            Envio_Esclavos();
         }
         Display_on;
      }
      #else
         Motor_on;
         tmp=((float)Vel*1023.0)/100.0;
         pwm1=1023-((int16)tmp);
         pwm2=pwm1;
         pwm1l=make8(pwm1,0);
         pwm1h=make8(pwm1,1);
         pwm2l=make8(pwm2,0);
         pwm2h=make8(pwm2,1);
         Envio_Esclavos();
      #endif
   }
}

short PidePassword(int8 MenuAnt, int8 MenuActual, int8 MenuPost, int8 clave0,int8 clave1, int8 clave2,int8 clave3){
      if(n==0)
      {
         displayContrasena(Contrasena[0],20,30,1);displayContrasena(Contrasena[1],40,30,0);
         displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
         strcpy(Menus.Letra,"Clave");
         displayMenu(Menus.Letra,20,0,1,3);
         n=1;
      }
      
      if(tup>=30){
         flag_latencia=0;t_latencia=0;
         if(Flanco == 0){
            Flanco = 1;delay_ms(5);
            for(i=1;i<=4;i++){
               if(unidad==i){
                  Contrasena[i-1]++;
                  if(Contrasena[i-1]>9){
                     Contrasena[i-1]=0;
                  }
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }else{
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }
         }
      }else{
         Flanco = 0;
      }
            
      if(tdown>=30){
         flag_latencia=0;t_latencia=0;
         if(Flanco2 == 0){
            Flanco2 = 1;delay_ms(5);
            for(i=1;i<=4;i++){
               if(unidad==i){
                  Contrasena[i-1]--;
                  if(Contrasena[i-1]<0){
                     Contrasena[i-1]=9;
                  }
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }else{
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }      
         }     
      }else{
         Flanco2 = 0;
      }
            
      if(tright>=30){
         flag_latencia=0;t_latencia=0;
         if(Flanco1 == 0){
            Flanco1 = 1;delay_ms(5);unidad++;
            for(i=1;i<=4;i++){
               if(unidad==i){
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }else{
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }
         }
      }else{
         Flanco1 = 0;
      }
      
      if(tleft>=30){
         delay_ms(100);
         //if(LEFT){
            Menu=MenuAnt;glcd_fillScreen(OFF);n=0;Opcion=1;unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            glcd_fillScreen(OFF);glcd_update();
            return 1;
         //}
      }
    
      if(unidad>4){
         glcd_fillScreen(OFF);unidad=1;flag_latencia=0;t_latencia=0;
         
         if(Contrasena[0]==3&&Contrasena[1]==8&&Contrasena[2]==9&&Contrasena[3]==2 && MenuActual==0){ // Si Ingresa clave para reset general del sistema.
            write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contrase?a de Fabrica y reinicia Programa.
            write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
            reset_cpu();
         }
      
         if((Contrasena[0]==clave0)&&(Contrasena[1]==clave1)&&(Contrasena[2]==clave2)&&(Contrasena[3]==clave3)){
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Correcta");
            displayMenu(Menus.Letra,15,30,0,2);
            Menu=MenuPost;glcd_update();delay_ms(1000);glcd_fillScreen(OFF);
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            if(MenuActual==0)tiempo_purga=1;n=0;
            return 1;
         }else{
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            unidad=1;glcd_update();delay_ms(1000);Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;glcd_fillScreen(OFF);
            glcd_rect(0, 0, 127, 25, YES, ON);strcpy(Menus.Letra,"Clave");glcd_text57(25, 1, Menus.Letra, 3, OFF);displayContrasena(Contrasena[0],20,30,1);
            displayContrasena(Contrasena[1],40,30,0);displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
            Menu=MenuAnt;glcd_fillScreen(OFF);n=0;
            return 0;
         }
      }
}

void glcd_imagen(int8 x)
{
   char i,j;
   signed char k; 
     
   if(x==3){
   for( i = 0 ; i < 64 ; i ++ )
   {  for( j = 0 ; j < 16 ; j ++)
      {  for(k=7;k>-1;k--)
         {      
            if( bit_test(Bombillo[i][j] ,7-k )) 
               glcd_pixel( j*8+k,i, ON );
         }  
      }
   }
   }
   
   if(x==4){
   for( i = 0 ; i < 64 ; i ++ )
   {  for( j = 0 ; j < 16 ; j ++)
      {  for(k=7;k>-1;k--)
         {      
            if( bit_test(Tomai[i][j] ,7-k )) 
               glcd_pixel( j*8+k,i, ON );
         }  
      }
   }
   }
   
   if(x==5){
   for( i = 0 ; i < 64 ; i ++ )
   {  for( j = 0 ; j < 16 ; j ++)
      {  for(k=7;k>-1;k--)
         {      
            if( bit_test(Ventilador[i][j] ,7-k )) 
               glcd_pixel( j*8+k,i, ON );
         }  
      }
   }
   }
}

Float LeerFlujoInFlow(int media)
{
    InFlow_adc=sensores(1);
      
    if(negativo==10)
    {
      x_uno=Inflow_adc+Diferencia;
    }
    if(negativo==20)
    {
      x_uno=Inflow_adc-Diferencia;
    }
    Velocidad=(pow(x_uno,3)*a)+(pow(x_uno,2)*b)+(x_uno*c)+(d);
    //Velocidad=Velocidad*correccion;//Ajuste de Temperatura
    
    if(Velocidad<0.0)
     {Velocidad=0.0;}
     
     if(l>=media)
       {l=0;}
     
     promedio[l]=Velocidad;l++;
     Flujo=0;
        for(h=0;h<=(media-1);h++)
        {
           Flujo+=promedio[h];
        } 
           Flujo=(Flujo/media)*Ajuste1; 
           return Flujo;
}

