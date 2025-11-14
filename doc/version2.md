# Implementacion de version 2

Instrucciones para Claude:

1. quiero agregarle una conexion a un wifi AP para poder seleccionar la red wifi a usar,
2. para hacer esto necesito guardar el wifi ultimo válido en eeprom,
3. si no se puede conectar al wifi luego de varios intentos, que borre la wifi de eeprom,
4. al iniciar por primera vez el sistema y no tener nada en eeprom, que se quede esperando configuracion,
5. la pagina web del webserver AP tiene que pedir configuracion, buscar una wifi y mostrar una para seleccionar, y un input box para ingresar la password.
6. si se conecta a la red wifi, se graba en eeprom.
7. todo loguearlo en Serial.
8. todo lo que sea wifi y web server colocarlo en C:\Users\javie\OneDrive\Documentos\Proyectos\Iset57\RelojGiganteMax7219\src\miWebServer.cpp
(no cambies codigo, pregunta primero)

