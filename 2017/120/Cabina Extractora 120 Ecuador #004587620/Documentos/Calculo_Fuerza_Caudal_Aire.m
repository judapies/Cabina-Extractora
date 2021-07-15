%%
%%
clc,clear
Viscosidad=1.95e-5;%Newton*Segundo sobre metro cuadrado
Caudal=1.038284;%Metros cubicos por segundo
Longitud=7;%Metro
Fuerza=Caudal*Viscosidad/Longitud;%Newton
Distancia=0.127;%Metro
Torque=Fuerza*Distancia;%Newton*Metro
Diferencia=5/Torques