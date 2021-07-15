%% Aproximaci�n de curva de sensor de temperatura PT-100
%% Ing. Judapies
clc,clear
%y=[0.5 0.7 1.5 2.7 4.4];
%y=100:0.1:134;
##x=[101.3 101.7 102.1 102.4 102.8 103.2 ...
##103.6 104 104.3 104.7 105.1 105.4 105.8 106.2 106.6 ...
##106.9 107.3 107.7 108.1 108.4 108.8 109.2 ...
##109.6 110 110.7 111.1 111.5 111.9 112.3 112.7 ...
##113.1 113.5 114 114.3 114.7 115.1 115.6 116 116.3 ...
##116.7 117.1 117.5 117.9 118.6 119.3 120 120 120.4 120.7 ...
##121.3 122 122 122.7 122.7 123.4 124.1 124.1 124.7 125.1 ...
##125.5 126 126.2 126.8 127.2 127.7 128.1 128.5 129 129.6 ...
##129.9 130.4 130.8 131.3 131.7 132.2 132.6 133.1 133.5 134 ...
##134.4 134.9 135.3 135.8 136.2 136.7 137.1 137.6 138.1 138.5 139 ...
##139.5 140 140.5 140.9 141.4 142 142.4 142.9 143.3 143.9 ...
##144.3 144.8 145.3 145.8 146.2 146.7 147.2 147.7 148.2 148.6 ...
##149.6 149.6 150.3 151 151 151.7 152.2 152.7 153.2 153.8 154.3 ...
##154.8 155.3 155.8 156.3 156.8 157.3 157.9 158.4 158.9 159.4 ...
##159.9 160 160.4 161.5 162.1 162.6 163.1 163.7 164.2 164.8 165.3 165.9 ...
##166.4 167 167.5 168 168.6 169.1 169.7 170.2 170.8 171.3 171.8 172.4 ...
##173.1 173.6 174.1 174.7 175.3 175.9 176.4 177 177.6 178.2 178.7 179.3 ...
##180 180.5 181.1 181.6 182.2 182.8 183.4 184 184.5 185.1 185.7 186.3 186.9 ...
##187.5 188.2 188.8 189.4 190 190.6 191.2 191.8 192.4 193 193.7 194.3 194.9 195.5 ...
##196.1 196.7 197.3 197.9 198.5 199.2 199.8 200.5 201.1 201.8 ...
##202.4 203.1 203.7 204.4 205 205.7 206.3 207 207.6 208.3 208.9 209.6 ...
##210.2 210.8 211.5 212.1 212.8 213.5 214.2 214.8 215.2 216.2 216.9 217.6 218.3 ...
##218.9 219.6 220.3 221 221.7 222.4 223.1 223.7 224.4 225.1 225.8 226.5 227.2 227.9 ...
##228.6 229.3 230 230.7 231.5 232.2 232.9 233.6 234.4 235.1 235.8 236.5 237.3 238 ...
##238.7 239.4 240.2 240.9 241.6 242.3 243.1 243.8 244.5 245.3 246 246.8 247.6 248.3 ...
##249.1 249.9 250.6 251.4 252.2 252.9 253.7 254.5 255.2 256 256.8 257.5 258.3 259.1 ...
##259.8 260.3 261.4 262.2 263 263.8 264.6 265.4 266.2 267 267.8 268.6 269.4 270.3 271.1 ...
##271.9 272.7 273.5 274.3 275.1 275.9 276.7 277.6 278.3 279.1 280 280.9 281.7 282.6 283.4 ...
##284.3 285.1 286 286.8 287.7 288.5 289.4 290.2 291.1 291.9 292.8 293.6 294.5 295.4 296.2 297.1 ...
##297.9 298.8 299.7 300.6 301.5 302.4 303.3 304.2];
y=100:134;
x=[101.3 105.1 108.8 112.7 116.7 120.7 125.1 129.6 134.0 138.5 143.3 148.2 153.2 ...
158.4 163.7 169.1 174.7 180.5 186.3 192.4 198.5 205 211.5 218.3 225.1 232.2 239.4 ...
246.8 254.5 262.2 270.3 278.3 286.8 295.4 304.2];

p1=polyfit(x,y,1);    %aproximaci�n por polinomio de segundo orden
p2=polyfit(x,y,2);    %aproximaci�n por polinomio de tercer orden
p3=polyfit(x,y,3);    %aproximaci�n por polinomio de tercer orden


x2=x;           %genera un vector que va desde 1 hasta 5 en incrementos de .1
y2=polyval(p1,x2);   %evalua el polinomio de segundo orden en los puntos x2
y3=polyval(p2,x2);

% plot(x,y,'.',x2,y2,'r',x2,y3,'g');
% grid on;
% legend('datos','polinomio de primer orden','polinomio de segundo orden');

adc=206.3;
Polinomio1=(adc*p1(1))+p1(2);
Polinomio2=(adc^2*p2(1))+(adc*p2(2))+p2(3);
Polinomio3=(adc^3*p3(1))+(adc^2*p3(2))+(adc*p3(3))+p3(4);

