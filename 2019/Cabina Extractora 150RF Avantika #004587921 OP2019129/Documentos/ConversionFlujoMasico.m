%% 
%% Conversion Flujo Masico
clc,clear
FlujoMasico=0:450;
Densidad=1.29;
Area=(9e-3*9e-3);
Velocidad=(FlujoMasico/(Densidad*Area))/3600;
FlujoVolumetrico=FlujoMasico/Densidad;
Velocidad2=(FlujoVolumetrico/Area)/3600;

Velocidad2=Velocidad2*0.3;
Masico=[0 0    50   100 150  200  250];
Voltaje=[0 1.0 2.25 2.7 3.25 3.55 3.75];
ADC=(Voltaje*1024)/5;
plot(Masico,ADC)
grid;