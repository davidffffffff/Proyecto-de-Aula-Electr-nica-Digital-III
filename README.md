# Proyecto-de-Aula-Electr-nica-Digital-III

Idea de proyecto final:

Se busca realizar un carrito que pueda transportar un paquete dese un punto A hasta un punto B. Para dicha operación el carrito contará con un sensor de ultrasonido con el cual va a evitar chocar con los diferentes obstáculos que se le presenten en su recorrido. Para mapear los puntos A y B, se tomará el punto A como el origen (inicio del recorrido), y a partir de un control se le enviará al carrito la distancia que debe recorrer hacia adelante, y hacia la izquierda ó derecha para poder llegar al punto B. En un escenario ideal (sin obstáculos) el carrito solo avanzará la distancia hacia a delante, y hacia la derecha ó izquierda, según donde se haya indicado que está el punto B. 

Para evitar los obstáculos el carrito tendrá en cuenta en qué parte de su recorrido tuvo que cambiar de dirección, y según la distancia que recorra en este nuevo sentido, recalculará cuantos centímetros debe recorrer hacia las diferentes direcciones (adelante, atrás, izquierda y derecha) para poder llegar al punto B. una vez el carrito llegue a su destino se detendrá.


Requisitos funcionales

•	Para comunicarle al dispositivo la distancia AB, se desarrollará un control que envíe de forma inalámbrica esta información al dispositivo.
•	El control debe estar diseñado con un teclado matricial 4*4, con el que se ingresará las distancia hacia adelante, derecha ó izquierda, y además con un botón del mismo teclado se indicará que ya se puede empezar el recorrido. Además, el mismo control debe tener 4 dispalys de 7 segmentos en los que se verán las distancias que se le están enviando al carrito.
•	El dispositivo debe tener la capacidad de diferenciar la dirección en la que se está moviendo (adelante, atrás, derecha, izquierda). Este no tendrá la capacidad de moverse en diagonal.
•	Para que el dispositivo llegue al punto B de forma correcta, debe tener en cuenta la distancia que se mueve en cada dirección
•	Cuando el carrito llegue al destino, este se detendrá y depositará el paquete. 
•	Una vez se deposite el paquete, hará un giro en 180° y retornará al punto A, esquivando los obstáculos que se puedan presentar nuevamente en su camino.

Requisitos no funcionales

•	Principalmente, el dispositivo debe ser capaz de proporcionarle a los usuarios la comodidad de no tener que moverse del lugar en el que están para poder intercambiar objetos.
•	El dispositivo debe ser capaz de transportar objetos cuyo peso no sea mayor a 1kg
•	Como se desea que el dispositivo sea capaz de transportar objetos en un entorno pequeño (hogar, oficina, etc…) el error en la distancia AB que se manejará rondará los 10 cm.
•	El dispositivo debe ser capaz de detectar un obstáculo a una distancia de alrededor 15 cm para que pueda cambiar su dirección si riesgos de chocar.
•	La interfaz de usuario en el control es intuitiva para que el usuario pueda ingresar las distancias y enviar el carrito sin ningún problema.

Escenario de pruebas

El escenario de pruebas para el dispositivo debe ser un lugar cuyo espacio sea similar al de un salón de clases, donde se encuentren algunos obstáculos (sillas, mesas, zapatos, bolsos, etc) que el dispositivo pueda superar para poder entregarle al destinatario el paquete enviado por el remitente. Para que la necesidad que el dispositivo quiere cumplir tenga sentido, también se necesitan los usuarios (remitente y destinatario) y el objeto que se desea enviar. dicho objeto no debe pesar más de un 1kg y debe ser compacto y sus dimensiones no deben superar los 15 cm^3.

Costos

dispaly 7 segmentos x3 $4000
3 botones $3000
Circuito impreso para el control $15000
Raspberry pi pico * 2 $34000 cu
Base del auto con llantas $40000
Sensor ultrasonido $8000
4 motorreductores $40000
Baterías del carrito $36000
total $214000

