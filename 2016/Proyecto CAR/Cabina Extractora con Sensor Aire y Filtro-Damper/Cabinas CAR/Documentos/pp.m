%% Aproximación de curva de sensor de temperatura PT-100
%% Ing. Judapies
clc,clear

y=[80  60  50  40   30   20  15   10  8    7    6   ];
V=[0.4 0.5 0.6 0.73 0.92 1.3 1.65 2.3 2.75 2.98 3.13];


for i=1:length(V)
  x(i)=V(i)*(1023/5);  
  

end

p1=polyfit(x,y,1);    %aproximaciòn por polinomio de segundo orden
p2=polyfit(x,y,2);    %aproximaciòn por polinomio de tercer orden
p3=polyfit(x,y,3);    %aproximaciòn por polinomio de tercer orden
p4=polyfit(x,y,9);    %aproximaciòn por polinomio de tercer orden


x2=x;           %genera un vector que va desde 1 hasta 5 en incrementos de .1
y1=polyval(p1,x2);   %evalua el polinomio de segundo orden en los puntos x2
y2=polyval(p2,x2);
y3=polyval(p3,x2);
y4=polyval(p4,x2);

 plot(x,y,'m',x2,y2,'r',x2,y3,'g',x2,y4,'b');
 grid on;|
% legend('datos','polinomio de primer orden','polinomio de segundo orden');

adc=613;
Polinomio1=(adc*p1(1))+p1(2);
Polinomio2=(adc^2*p2(1))+(adc*p2(2))+p2(3);
Polinomio3=(adc^3*p3(1))+(adc^2*p3(2))+(adc*p3(3))+p3(4);
#Polinomio4=(adc^9*p4(1))+(adc^8*p4(2))+(adc^7*p4(3))+(adc^6*p4(4))+(adc^5*p4(5))...
#        +(adc^4*p4(6))+(adc^3*p4(7))+(adc^2*p4(8))+(adc*p4(9))+p4(10);
