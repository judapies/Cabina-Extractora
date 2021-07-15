%% Aproximación de curva de sensor de temperatura PT-100
%% Ing. Judapies
clc,clear
y=[0.5 0.7 1.5 2.7 4.4];
x=[347 355 366 387 404];

p1=polyfit(x,y,1);    %aproximaciòn por polinomio de segundo orden
p2=polyfit(x,y,2);    %aproximaciòn por polinomio de tercer orden
p3=polyfit(x,y,3);    %aproximaciòn por polinomio de tercer orden


x2=x;           %genera un vector que va desde 1 hasta 5 en incrementos de .1
y2=polyval(p1,x2);   %evalua el polinomio de segundo orden en los puntos x2
y3=polyval(p2,x2);

% plot(x,y,'.',x2,y2,'r',x2,y3,'g');
% grid on;
% legend('datos','polinomio de primer orden','polinomio de segundo orden');

adc=335;
Polinomio1=(adc*p1(1))+p1(2);
Polinomio2=(adc^2*p2(1))+(adc*p2(2))+p2(3);
Polinomio3=(adc^3*p3(1))+(adc^2*p3(2))+(adc*p3(3))+p3(4);

