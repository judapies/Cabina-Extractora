%% Aproximaci?n de curva de sensor de temperatura PT-100
%% Ing. Judapies
clc,clear
%y=[0    0.25 0.3 0.35 0.44 0.45 0.5 0.55];
%x=[1100 880  740 660  570  330  260 0];

y=[0   312 587 825 975 1037 1187];  %Caudal
yy=y.*0.9;
x=[2.4 2.2 1.9 1.5 1.2 1.1  0.9];     %Presion

p1=polyfit(x,yy,1);    %aproximaci?n por polinomio de segundo orden
p2=polyfit(x,yy,2);    %aproximaci?n por polinomio de tercer orden
p3=polyfit(x,yy,3);    %aproximaci?n por polinomio de tercer orden


x2=x;           %genera un vector que va desde 1 hasta 5 en incrementos de .1
y1=polyval(p1,x2);
y2=polyval(p2,x2);   %evalua el polinomio de segundo orden en los puntos x2
y3=polyval(p3,x2);

 plot(x,yy,'.',x2,y2,'r',x2,y2,'g',x2,y2,'m');
 grid on;
 %legend('datos','polinomio de primer orden','polinomio de segundo orden','polinomio de Tercer orden');

adc=0.1;%inH2O
Polinomio1=(adc*p1(1))+p1(2);
Polinomio2=(adc^2*p2(1))+(adc*p2(2))+p2(3)
Polinomio3=(adc^3*p3(1))+(adc^2*p3(2))+(adc*p3(3))+p3(4)

