%% Aproximación de curva de sensor de temperatura PT-100
%% Ing. Judapies
clc,clear
y=[-.935 0   .3  .4  .6  .8  .9  1.2];
x=[ 420  429 434 437 442 448 455 460];

p1=polyfit(x,y,1);    %aproximaciòn por polinomio de segundo orden
p2=polyfit(x,y,2);    %aproximaciòn por polinomio de tercer orden
p3=polyfit(x,y,3)    %aproximaciòn por polinomio de tercer orden


x2=x;           %genera un vector que va desde 1 hasta 5 en incrementos de .1
y2=polyval(p1,x2);   %evalua el polinomio de segundo orden en los puntos x2
y3=polyval(p2,x2);

plot(x,y,'.',x2,y2,'r',x2,y3,'g');
grid on;
legend('datos','polinomio de primer orden','polinomio de segundo orden');

adc=483;
tempadc=701;
Polinomio1=(adc*p1(1))+p1(2);
Polinomio2=(adc^2*p2(1))+(adc*p2(2))+p2(3);
Polinomio3=(adc^3*p3(1))+(adc^2*p3(2))+(adc*p3(3))+p3(4)

% 
% % ***************** grafica comparativa
% X=420:480;
% Y=(X.^3*p3(1))+(X.^2*p3(2))+(X*p3(3))+p3(4);  % Orden 3
% % Y=(X.^2*p2(1))+(X*p2(2))+p2(3);  % Orden 2
% plot(X,Y,x,y);grid;
% % *****************
% 
% Vactual = Polinomio3;
% Vcorregido = Polinomio3*(656/tempadc);