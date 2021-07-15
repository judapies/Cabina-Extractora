////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2018 JP Bioingenieria SAS                     ////
////         Funciones utilizadas en CEGH con F y D                     ////
////////////////////////////////////////////////////////////////////////////

void mensajes(int8 x,y){// Funcion para imprimir mensajes de Menu Principal.
    
   if(x==1)
   {if(estadoalarma==0)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma          OFF");}
   if(estadoalarma==1)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma           ON");}
   }
   
   if(x==2)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Filtro             ");}
   
   if(x==3)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo Filtro      ");}
   
   if(x==4)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Cambio Contraseña  ");}
   
   if(x==5)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo de Purga    ");}
   
   if(x==6)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo Post-Purga  ");}
   
   if(x==7)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Servicio           ");}
   
   if(x==8)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Mover Vidrio       ");}
}

short SolicitaContrasena(int8 MenuAct,int8 MenuAnt,int8 MenuPos,cont0,cont1,cont2,cont3){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Ingrese        ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"    Contraseña      ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    CLAVE=%u%u%u%u  ",clave[0],clave[1],clave[2],clave[3]);
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0){
            clave[unidad-11]++;Flanco = 1;delay_ms(30);t_latencia=0;
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo  
         delay_ms(20);
         if(DOWN){// && Flanco2 == 0){
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);t_latencia=0;
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si Oprime Derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0){
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si Oprime izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0){
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");t_latencia=0;
         }
      }else{
         Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
         clave[unidad-11]=9;
      if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
         clave[unidad-11]=0;
      if(unidad<11){             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
         unidad=11;
         Menu=MenuAnt;
         paso=0;Flecha=2;Flecha2=2;unidad=11;
      }
      
      lcd_gotoxy(unidad,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      
      if(unidad>11&&unidad<14){
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
      }
      
      if(unidad>14){             // Si a Terminado de ingresar la clave, verifica si es correcta o no.
         if(clave[0]==3&&clave[1]==8&&clave[2]==9&&clave[3]==2&&MenuAct==0){ // Si Ingresa clave para reset general del sistema.
             write_eeprom(100,0);delay_ms(20);write_eeprom(101,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
             write_eeprom(102,0);delay_ms(20);write_eeprom(103,0);delay_ms(20);
             reset_cpu();
         }
            
         if(clave[0]==cont0 && clave[1]==cont1 && clave[2]==cont2 && clave[3]==cont3){ // Si las claves coinciden pasa a Menu Principal.
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);
            Menu=MenuPos;unidad=11;
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            return 1;
         }else{                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
            lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta    ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            return 0;
         }
      }     
}

void MenuPrincipal(){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"---MENU PRINCIPAL---");
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0){
            Flecha2--;Flecha--;Flecha1=Flecha+1;Flanco = 1;delay_ms(30);
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0){
            Flecha2++;Flecha++;Flecha1=Flecha-1;Flanco2 = 1;delay_ms(30);
         }
      }else{
         Flanco2 = 0;
      }
   
        
      if(Flecha2>nMenuH)
      {paso++;nMenuH=Flecha2;nMenuL=nMenuH-2;Flecha=4;}
        
      if(Flecha2<nMenuL)
      {paso--;nMenuL=Flecha2;nMenuH=nMenuL+2;Flecha=2;}
        
      if(Flecha2>n_opcionH)
      {Flecha2=n_opcionL;Flecha=2;paso=0;nMenuL=Flecha2;nMenuH=nMenuL+2;}
      
      if(Flecha2<n_opcionL)
      {Flecha2=n_opcionH;Flecha=4;paso=n_opcionH-4;nMenuH=Flecha2;nMenuL=nMenuH-2;}  
         
      if(paso<0)
         paso=0;
      
      mensajes(1+paso,2);
      mensajes(2+paso,3);
      mensajes(3+paso,4);

      lcd_gotoxy(1,Flecha);// Para mostrar la flecha de seleccion
      lcd_putc(t[0]);

      if(Flecha==2){
         lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }
        
      if(Flecha==4){
         lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }
        
      if(Flecha==3){
         lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
         lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
         lcd_putc(t[1]);
      }

      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            Flanco1 = 1;Menu=Flecha2;Flecha=3;delay_ms(500);printf(lcd_putc,"\f");
            if(Menu==6){
               clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            }
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            /*Flanco3 = 1;*/delay_ms(500);Menu=20;printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
}

void MenuFiltro(){
   lcd_gotoxy(1,1);
      printf(lcd_putc,"  Duracion Actual   ");                          
      lcd_gotoxy(1,2);                                                  
      printf(lcd_putc," Tiempo= %02u:%02u:%02u ",horas,minutos,segundos);      
      
      if(flag_filtro==1){
         lcd_gotoxy(1,3);                                                 
         printf(lcd_putc,"DESACTIVAR= Oprima >");                        
      }else{
         lcd_gotoxy(1,3);                                                 
         printf(lcd_putc," ACTIVAR= Oprima >  ");                        
      }
   
      lcd_gotoxy(1,4);                                                 
      printf(lcd_putc," RESET= Oprima ^    ");                                                                                   
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP){
            printf(lcd_putc,"\f");
            lcd_gotoxy(1,2);
            printf(lcd_putc," Reset de tiempo ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Exitoso     ");
            write_eeprom(70,0);delay_ms(20);
            write_eeprom(71,0);delay_ms(20);
            write_eeprom(72,0);delay_ms(20);
            delay_ms(700);
            segundos=0;minutos=0;horas=0;
            delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         }
      }
      
      if(RIGHT){
         delay_ms(20);
         if(RIGHT){
            flag_filtro=!flag_filtro;write_eeprom(73,flag_filtro);delay_ms(20);
            delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         }
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            Flanco3 = 1;delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
}

void MenuSetpoint(){
      lcd_gotoxy(1,2);
      printf(lcd_putc," Tiempo de Filtro");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"       %03u   ",setpoint);
   
      if(setpoint<10)
         setpoint=10;
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0) {
            setpoint+=1;Flanco = 1;
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0) {
            setpoint-=1;Flanco2 = 1;
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT || LEFT){// Si oprime derecha
         delay_ms(20);
         if((RIGHT || LEFT)&& Flanco1 == 0){
            /*Flanco1 = 1;*/
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            /*Temporal=setpoint;
            Entero=(int)setpoint;
            Temporal=Temporal-Entero;
            Temporal2=Temporal*10.0;
            Decimal2=(int8)Temporal2;*/
            write_eeprom(40,setpoint);delay_ms(20);
            //write_eeprom(41,Decimal2);delay_ms(20);//Guardar valor de Setpoint en eeprom
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
}

void MenuAlarma(){
   estadoalarma=!estadoalarma;Flanco1 = 1;Menu=1; paso=0;
                      
            if(estadoalarma==1){
               estadoalarma=1;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"        Activo      ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"        Alarma      ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
               
            if(estadoalarma==0){
               estadoalarma=0;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"      Desactivo     ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"       Alarma       ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");
            }
            write_eeprom(5,estadoalarma);delay_ms(1000);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
}

void MenuMedia(){
   lcd_gotoxy(1,2);
      printf(lcd_putc,"     Media Movil    ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"       %02i   ",MediaMovil);
      
      if(MediaMovil>12)
         MediaMovil=12;
   
      if(MediaMovil<1)
         MediaMovil=1;
   
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0) {
            MediaMovil+=1;Flanco = 1;delay_ms(30);
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0){
            MediaMovil-=1;Flanco2 = 1;delay_ms(30);
         }
      }else{
         Flanco2 = 0;
      }
   
      if(LEFT){// Si oprime derecha
         delay_ms(20);
         if(LEFT && Flanco1 == 0) {
            /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            write_eeprom(42,MediaMovil);delay_ms(20);//Guardar valor de Setpoint en eeprom
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
}

void MenuPuntoCero(){
   lcd_gotoxy(1,1);
      printf(lcd_putc,"  Zero Point Config ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"InFlow= %2.1f  (m/s) ",Flujo);
      lcd_gotoxy(1,3);
      printf(lcd_putc,"ZF=%2.0f ADC=%2.0f",zero_fabrica,sensores(0));
      lcd_gotoxy(1,4);
      printf(lcd_putc," Diferencia=%2.0f",Diferencia);
    
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            Diferencia=zero_fabrica-sensores(0);
            
            if(Diferencia>=0){
               negativo=10;write_eeprom(50,negativo);delay_ms(20);
            }
            if(Diferencia<0){
               negativo=20;write_eeprom(50,negativo);delay_ms(20);
            }
            Diferencia=abs(Diferencia);
            Dif16=(int16)Diferencia;
            
            write_eeprom(60,make8(Dif16,0));delay_ms(20);
            write_eeprom(61,make8(Dif16,1));delay_ms(20);//Guardar valor de Setpoint en eeprom
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            /*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
}

void MenuMantenimiento(){
   lcd_gotoxy(1,1);
      printf(lcd_putc," MANT.  V=%2.0f  ",sensores(0));//T= %2.0f",sensores(1));//T= %2.1f",TC);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Damp=%2.0f%%  T=%2.0f ",(100*((float)tmp/255)),sensores(1));
      lcd_gotoxy(1,3);
      printf(lcd_putc,"IF= %2.1f(m/s) Dam=%2.0f",Flujo,(100*(sensores(2)/1023)));
      lcd_gotoxy(1,4);
      printf(lcd_putc,"Ajuste1= %2.1f ",Ajuste1);
      
      if(Ajuste1>30.0)
         Ajuste1=0.0;
         
      if(Ajuste1<0.0)
         {Ajuste1=0.0;}
      
      if(UP){//Si oprime hacia arriba
         delay_ms(50);
         if(UP){
            Ajuste1+=0.1;
         }
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(50);
         if(DOWN){
            Ajuste1-=0.1;
         }
      }
      
      if(LEFT){// Si oprime derecha
         delay_ms(20);
         if(LEFT && Flanco1 == 0) {
            /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(30);printf(lcd_putc,"\f");
            Temporal=Ajuste1;
            Entero=(int)Ajuste1;
            Temporal=Temporal-Entero;
            Temporal2=Temporal*10.0;
            Decimal1=(int8)Temporal2;
            write_eeprom(25,Entero);delay_ms(20);
            write_eeprom(26,Decimal1);delay_ms(20);
            MensajeGuardado();
         }
      }else{
         Flanco1 = 0;
      }
            
}

void MenuModo(){
   lcd_gotoxy(1,1);
   printf(lcd_putc," MODO   Damp=%2.0f%% ",(100*((float)tmp/255)));

   lcd_gotoxy(1,4);
   printf(lcd_putc,"IF= %2.1f(m/s) Dam=%2.0f",Flujo,(100*(sensores(2)/1023)));
      
         if(UP && DOWN){
            delay_ms(20);
            if(UP && DOWN){
               ModoAuto=0;
               ModoManual=1;
               ModoSemi=0;
               Automa=10;
               Semi=10;
               Manual=20;
            }
         }
         
         if(DOWN && RIGHT){
            delay_ms(20);
            if(DOWN && RIGHT){
               ModoAuto=1;
               ModoManual=0;
               ModoSemi=0;
               Automa=20;
               Semi=10;
               Manual=10;
            }
         }
         
         if(UP && RIGHT){
            delay_ms(20);
            if(UP && RIGHT){
               ModoAuto=0;
               ModoManual=0;
               ModoSemi=1;
               Automa=10;
               Semi=20;
               Manual=10;
            }
         }
      
      if(ModoManual == 1 || Manual==20)
      {
         ModoAuto=0;ModoSemi=0;Automa=10;Semi=10;
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   Modo Manual  ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    Seleccionado    ");
         if(LEFT)
         {
            delay_ms(20);
            if(LEFT){
               ModoSemi=0;ModoAuto=0;ModoManual=0;Manual=20;Automa=10;Semi=10;delay_ms(500);printf(lcd_putc,"\f");ajustar_damper(254);tmp=254; 
               write_eeprom(10,Manual);delay_ms(20);
               write_eeprom(12,Semi);delay_ms(20);
               write_eeprom(11,Automa);delay_ms(20);
               write_eeprom(13,tmp);delay_ms(20);
               MensajeGuardado();
               Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
            }
         } 
      }
        
      if(ModoAuto == 1 || Automa==20)
      {
         ModoManual=0;ModoSemi=0;Semi=10;Manual=10;
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   Modo Auto   ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    Seleccionado    ");
         if(LEFT)
         {
            delay_ms(20);
            if(LEFT){
               ModoSemi=0;ModoAuto=0;ModoManual=0;Automa=20;Semi=10;Manual=10;delay_ms(500);printf(lcd_putc,"\f");
               write_eeprom(10,Manual);delay_ms(20);
               write_eeprom(12,Semi);delay_ms(20);
               write_eeprom(11,Automa);delay_ms(20);
               write_eeprom(13,tmp);delay_ms(20);
               MensajeGuardado();
               Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
            }
         } 
      }
      
      if(ModoSemi == 1 || Semi==20)
      {
         ModoManual=0;ModoAuto=0;Automa=10;Manual=10;
         lcd_gotoxy(1,2);
         printf(lcd_putc,"   Modo Semi   ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Ajuste Valor    ");
         if(UP){
            delay_ms(20);
            if(UP)
               tmp++;
         }
         
         if(DOWN){
            delay_ms(20);
            if(DOWN)
               tmp--;
         }
         
         ValorSemi=tmp;
         ajustar_damper(tmp);
         if(LEFT)
         {
            delay_ms(20);
            if(LEFT){
               write_eeprom(13,tmp);delay_ms(20);ModoSemi=0;ModoAuto=0;ModoManual=0;Semi=20;Automa=10;Manual=10;delay_ms(500);printf(lcd_putc,"\f");
               write_eeprom(12,Semi);delay_ms(20);
               write_eeprom(11,Automa);delay_ms(20);
               write_eeprom(10,Manual);delay_ms(20);
               MensajeGuardado();
               Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;
            }
         } 
      } 
}

void MenuTiempoPurga(){
   lcd_gotoxy(1,1);
      printf(lcd_putc,"     Tiempo  de      ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"       Purga         ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%02u:%02u (M:S)  ",purga_minutos_pro,purga_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      
      if(unidad2<1)
         unidad2=1;
      if(unidad2>3)
         unidad2=3;   
      
      if(UP){
         if(unidad2==1){
            if(purga_minutos_pro<61){
               purga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3){
            if(purga_segundos_pro<60){
               purga_segundos_pro++;delay_ms(300);
            }
         }
      }
      
      if(DOWN){
         if(unidad2==1){
            if(purga_minutos_pro>1){
               purga_minutos_pro--;delay_ms(300);
            }
            
            if(purga_minutos_pro>60){
               purga_minutos_pro=60;delay_ms(300);
            }
         }
         
         if(unidad2==3){
            if(purga_segundos_pro>0){
               purga_segundos_pro--;delay_ms(300);
            }
            
            if(purga_segundos_pro>59){
               purga_segundos_pro=59;delay_ms(300);
            }
         }
      }
      
      if(RIGHT){
         if(unidad2==1){
            unidad2=3;
         }else{
            if(unidad2==3){
               unidad2=1;
            }else{
               unidad2=3;
            }
         }
         delay_ms(500);
         printf(lcd_putc,"\f");
      }
      
      if(LEFT)
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(20,purga_minutos_pro);delay_ms(30);
         write_eeprom(21,purga_segundos_pro);delay_ms(30);
         purga_minutos=purga_minutos_pro;purga_segundos=purga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }  
}

void MenuTiempoPostPurga(){
   lcd_gotoxy(1,1);
      printf(lcd_putc,"     Tiempo  de      ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"     Post-Purga      ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%02u:%02u (M:S)  ",ppurga_minutos_pro,ppurga_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      if(unidad2<1)
         unidad2=1;
      if(unidad2>3)
         unidad2=3;   
      
      if(UP)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro<61)
            {
               ppurga_minutos_pro++;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro<60)
            {
               ppurga_segundos_pro++;delay_ms(300);
            }
         }
      }
      
      if(DOWN)
      {
         if(unidad2==1)
         {
            if(ppurga_minutos_pro>1)
            {
               ppurga_minutos_pro--;delay_ms(300);
            }
            
            if(ppurga_minutos_pro>60)
            {
               ppurga_minutos_pro=60;delay_ms(300);
            }
         }
         
         if(unidad2==3)
         {
            if(ppurga_segundos_pro>0)
            {
               ppurga_segundos_pro--;delay_ms(300);
            }
            
            if(ppurga_segundos_pro>59)
            {
               ppurga_segundos_pro=59;delay_ms(300);
            }
         }
      }
      
      if(RIGHT)
      {
         if(unidad2==1)
         {
            unidad2=3;
         }
         else
         {
            if(unidad2==3)
            {
               unidad2=1;
            }
         }
         delay_ms(500);
         printf(lcd_putc,"\f");
      }
      
      if(LEFT)
      {
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(22,ppurga_minutos_pro);delay_ms(20);
         write_eeprom(23,ppurga_segundos_pro);delay_ms(20);
         ppurga_minutos=ppurga_minutos_pro;ppurga_segundos=ppurga_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }  
}

void MenuPostPurga(){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"    !Post-Purga!    ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"                    ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"      %02u:%02u  ",ppurga_minutos,ppurga_segundos); 
      Motor_on;
          
      if(Lectura==1)
      {
         Luz_Blanca_off;
         
         if(ppurga_segundos==0 && ppurga_minutos==0)
         {
            write_eeprom(70,segundos);delay_ms(20);
            write_eeprom(71,minutos);delay_ms(20);
            write_eeprom(72,horas);delay_ms(20);
            printf(lcd_putc,"\f");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Proceso      ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"    Finalizado    ");
            O1_off;O2_off;
            printf(lcd_putc,"\f");
            #ifdef VARIADOR
            ajuste_vel(tmp);
            #endif            
            reset_cpu();
         }
         Lectura=0;
      }
}   

void MenuApagado(){
   lcd_gotoxy(1,1);
   printf(lcd_putc,"   Desea apagar     ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"     la cabina      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"   > Aceptar        ");
   lcd_gotoxy(1,4);
   printf(lcd_putc,"   < Cancelar       ");
  
   
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0){
            Flanco1 = 1;Menu=30;intensidadpantalla(contraste);Alarma_off;printf(lcd_putc,"\f");tiempo_ppurga=1;
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0){
            Flanco3 = 1;Menu=20;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
}

void MenuPurga(){
      if(Lectura2==1){
         lcd_gotoxy(1,1);
         printf(lcd_putc,"      !Purga!       ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"                    ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"      %02u:%02u   ",purga_minutos,purga_segundos); 
         Lectura2=0;
      }
      
      if(!VIDRIOUP || VIDRIODN){
         O1_off;O2_on;
      }else{
         O1_off;O2_off;
      }
      
      if(LEFT){
         delay_ms(1000);
         if(LEFT){
            tiempo_purga=0;
            EnciendeMotor();//Motor_on;
            Menu=20;
            detenerVidrio();
         }
      }
      
      #ifdef DAMPER
      controlar_flujo();
      #endif
      
      #ifdef VARIADOR
      ajuste_vel(tmp);
      #endif
      Motor_on;
      //EnciendeMotor();
      Luz_Blanca_off;
        
      if(Lectura==1){
        Flujo=Leer_Sensor_Flujo(MediaMovil);
      }
}

void MenuCambioClave(){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"     Ingrese        ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"  Contraseña Nueva  ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && Flanco == 0) {
            clave[unidad-11]++;Flanco = 1;delay_ms(30);
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN && Flanco2 == 0) {
            clave[unidad-11]--;Flanco2 = 1;delay_ms(30);
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si oprime Derecha
         delay_ms(20);
         if(RIGHT && Flanco1 == 0) {
            Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");
         }
      }else{
         Flanco1 = 0;
      }
            
      if(LEFT){// Si oprime Izquierda
         delay_ms(20);
         if(LEFT && Flanco3 == 0) {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");
         }
      }else{
         Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
         clave[unidad-11]=9;
      if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
         clave[unidad-11]=0;
         
      if(unidad<11){
         delay_ms(500);Menu=1;paso=0;Flecha=2;Flecha2=2;
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         printf(lcd_putc,"\f");unidad=11;
      }
      
      lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
      lcd_putc(t[2]);
      if(unidad>14){// Si ya ingreso la nueva contraseña.
         lcd_gotoxy(1,1);
         printf(lcd_putc,"                    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"     Contraseña     ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"     Almacenada     ");
         lcd_gotoxy(1,4);
         printf(lcd_putc,"                    ");
         contrasena[0]=clave[0];contrasena[1]=clave[1];contrasena[2]=clave[2];contrasena[3]=clave[3];
         write_eeprom(100,clave[0]);delay_ms(20);write_eeprom(101,clave[1]);delay_ms(20);
         write_eeprom(102,clave[2]);delay_ms(20);write_eeprom(103,clave[3]);delay_ms(20);
         delay_ms(500);Menu=1;paso=0;Flecha=2;Flecha2=2;
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         unidad=11;
      }
      
      if(unidad>11&&unidad<14){
         lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
         lcd_putc(t[1]);
      }
}

void MenuVelocidad(){
      lcd_gotoxy(1,2);
      printf(lcd_putc,"  Velocidad Motor=  ");
      
      vel=((float)tmp/254)*100;
      v=(int8)vel;
      lcd_gotoxy(1,3);
      printf(lcd_putc,"        %i%%        ",v);

      if(tmp<30)
      {tmp=30;}
   
      if(tmp>254)
      {tmp=254;}
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP) {
            tmp+=1;delay_ms(30);
            ajuste_vel(tmp);
         }
      }
      
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN) {
            tmp-=1;delay_ms(30);
            ajuste_vel(tmp);
         }
      }
      
      if(LEFT){//Si oprime hacia abajo
         delay_ms(20);
         if(LEFT) {
            ajuste_vel(tmp);
            write_eeprom(13,tmp);delay_ms(500);
            lcd_gotoxy(1,2);
            delay_ms(500);
            printf(lcd_putc,"\f");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"  Valor almacenado  ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"   Correctamente   ");
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);
         }
      }
}

void MenuVidrio(){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"   Mover   Vidrio    ");
      
      if(UP){
         lcd_gotoxy(1,3);
         printf(lcd_putc,"   !Subiendo!    ");   
          
         if(!VIDRIOUP || VIDRIODN){
            O1_off;O2_on;
         }else{
            O1_off;O2_off;
         }
      }else{
         if(DOWN){
            lcd_gotoxy(1,3);
            printf(lcd_putc,"   !Bajando!    ");   

            if(VIDRIOUP || !VIDRIODN){
               O1_on;O2_off;
            }else{
               O1_off;O2_off;
            }
         }else{
            lcd_gotoxy(1,3);
            printf(lcd_putc,"   !Detenido!   ");   
            O1_off;O2_off;
         }
      }
      
      
      if(LEFT){
         delay_ms(500);
         printf(lcd_putc,"\f");
         Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         O1_off;O2_off;
      }  
}

void MenuServicio(){
      if(Opcion<1)
         Opcion=4;
      if(Opcion>4)
         Opcion=1;
         
      if(Opcion==1){
         lcd_gotoxy(1,1);printf(lcd_putc,">Media Movil        ");
         lcd_gotoxy(1,2);printf(lcd_putc," Punto Cero         ");
         lcd_gotoxy(1,3);printf(lcd_putc," Mantenimiento      ");
         #ifdef DAMPER   
         lcd_gotoxy(1,4);printf(lcd_putc," Modo               ");
         #endif
         
         #ifdef VARIADOR
         lcd_gotoxy(1,4);printf(lcd_putc," Salida Analoga     ");
         #endif  
      }else if(Opcion==2){
         lcd_gotoxy(1,1);printf(lcd_putc," Media Movil        ");
         lcd_gotoxy(1,2);printf(lcd_putc,">Punto Cero         ");
         lcd_gotoxy(1,3);printf(lcd_putc," Mantenimiento      ");
         #ifdef DAMPER   
         lcd_gotoxy(1,4);printf(lcd_putc," Modo               ");
         #endif
         
         #ifdef VARIADOR
         lcd_gotoxy(1,4);printf(lcd_putc," Salida Analoga     ");
         #endif  
      }else if(Opcion==3){
         lcd_gotoxy(1,1);printf(lcd_putc," Media Movil        ");
         lcd_gotoxy(1,2);printf(lcd_putc," Punto Cero         ");
         lcd_gotoxy(1,3);printf(lcd_putc,">Mantenimiento      ");
         #ifdef DAMPER   
         lcd_gotoxy(1,4);printf(lcd_putc," Modo               ");
         #endif
         
         #ifdef VARIADOR
         lcd_gotoxy(1,4);printf(lcd_putc," Salida Analoga     ");
         #endif  
      }else if(Opcion==4){
         lcd_gotoxy(1,1);printf(lcd_putc," Media Movil        ");
         lcd_gotoxy(1,2);printf(lcd_putc," Punto Cero         ");
         lcd_gotoxy(1,3);printf(lcd_putc," Mantenimiento      ");
         #ifdef DAMPER   
         lcd_gotoxy(1,4);printf(lcd_putc,">Modo               ");
         #endif
         
         #ifdef VARIADOR
         lcd_gotoxy(1,4);printf(lcd_putc,">Salida Analoga     ");
         #endif   
      }
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP) {
            delay_ms(30);
            Opcion--;
         }
      }
      
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN) {
            delay_ms(30);            
            Opcion++;
         }
      }
      
      if(RIGHT){//Si oprime hacia abajo
         delay_ms(20);
         if(RIGHT) {
            if(Opcion==1){
               Menu=9;
            }else if(Opcion==2){
               Menu=10;
            }else if(Opcion==3){
               Menu=11;
            }else if(Opcion==4){
               Menu=12;
            }
         }
         printf(lcd_putc,"\f");
      }
      
      if(LEFT){//Si oprime hacia abajo
         delay_ms(20);
         if(LEFT) {
            Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
         }
      }
}
