%% Aproximación de curva de sensor de temperatura PT-100
%% Ing. Judapies
clc,clear
V=[314 290 257 241 229 222 219 216 214 212 208];
y=[2.6 2.3 1.5 1.3 1.0 0.8 0.7 0.6 0.5 0.4 0.0];


for i=1:length(V)
  x(i)=V(i);  
  

end

p1=polyfit(x,y,1);    %aproximaciòn por polinomio de segundo orden
p2=polyfit(x,y,2);    %aproximaciòn por polinomio de tercer orden
p3=polyfit(x,y,3);    %aproximaciòn por polinomio de tercer orden
p4=polyfit(x,y,5);    %aproximaciòn por polinomio de tercer orden


x2=x;           %genera un vector que va desde 1 hasta 5 en incrementos de .1
y1=polyval(p1,x2);   %evalua el polinomio de segundo orden en los puntos x2
y2=polyval(p2,x2);
y3=polyval(p3,x2);
y4=polyval(p4,x2);

 plot(x,y,'m',x2,y2,'r',x2,y3,'g',x2,y4,'b');
 grid on;
 %legend('datos','polinomio de primer orden','polinomio de segundo orden');

a=0.0000000217825761185273;b=-0.0000265435346051623;c= 0.0129037445818485;d= -3.12874942435261;
t=378.463968402560;f=-18275.0402762787;
 
 adc=280;
Polinomio1=(adc*p1(1))+p1(2);
Polinomio2=(adc^2*p2(1))+(adc*p2(2))+p2(3);
Polinomio3=(adc^3*p3(1))+(adc^2*p3(2))+(adc*p3(3))+p3(4);
Polinomio4=(adc^5*p4(1))+(adc^4*p4(2))+(adc^3*p4(3))+(adc^2*p4(4))+(adc*p4(5))...
        +p4(6);
Poli5=(adc^5*a)+(adc^4*b)+(adc^3*c)+(adc^2*d)+(adc*t)...
        +f; 
