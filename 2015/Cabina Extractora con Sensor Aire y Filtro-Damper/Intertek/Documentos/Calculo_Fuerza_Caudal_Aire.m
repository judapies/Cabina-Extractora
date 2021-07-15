%% Ing. Judapies 
%% Calculo De Fuerza de Damper
clc,clear
Viscosidad=1.95e-5;%Newton*Segundo sobre metro cuadrado
Densidad=1.09;
Caudal=1.53;%Metros cubicos por segundo
Longitud=30;%Metro
r=0.254/2;
AreaDucto=pi*r^2;
Velocidad=Caudal/AreaDucto;
% Fuerza=Caudal*Viscosidad/Longitud;%Newton
Fuerza=Densidad*Caudal*Velocidad;
Distancia=0.127;%Metro
Torque=Fuerza*Distancia;%Newton*Metro
Diferencia=5/Torque
TorqueDamper=5;
FuerzaDamper=TorqueDamper/Distancia;