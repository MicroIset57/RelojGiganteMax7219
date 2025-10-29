#include "miWebServer.h"
#include <time.h>

// WebServer server(80);
// WiFiClient client;
// HTTPClient http;
WiFiMulti wifiMulti;

const char *htmlPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Configuración del Reloj</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; text-align: center; margin: 0px auto; padding: 20px; }
        .input-group { margin: 10px 0; }
        input[type="datetime-local"] { padding: 5px; font-size: 16px; }
        button { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; 
                border-radius: 4px; cursor: pointer; font-size: 16px; }
        button:hover { background-color: #45a049; }
    </style>
</head>
<body>
    <h1>Configuración del Reloj</h1>
    <form id="timeForm">
        <div class="input-group">
            <label for="datetime">Fecha y Hora:</label><br>
            <input type="datetime-local" id="datetime" name="datetime" required>
        </div>
        <button type="submit">Actualizar</button>
    </form>
    <p id="status"></p>

    <script>
        document.getElementById('timeForm').onsubmit = function(e) {
            e.preventDefault();
            var datetime = document.getElementById('datetime').value;
            var timestamp = Math.floor(new Date(datetime).getTime() / 1000);
            
            fetch('/setTime?timestamp=' + timestamp, {
                method: 'GET'
            })
            .then(response => response.text())
            .then(data => {
                document.getElementById('status').innerHTML = 'Hora actualizada con éxito';
            })
            .catch(error => {
                document.getElementById('status').innerHTML = 'Error al actualizar la hora';
            });
        };
    </script>
</body>
</html>
)";

// Inicia la conexión a las redes WiFi
void wifiConnect()
{
    Serial.println("Conectando al wifi");
    Serial.print('.');

    while (wifiMulti.run(10000) != WL_CONNECTED)
    {
        Serial.print('.');
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("WiFi OK: ");
        Serial.println(WiFi.SSID().c_str());
    }
    else
    {
        Serial.println("ERROR EN EL WIFI !!!");
    }
}

void handleRoot()
{
    Serial.println("Solicitud HTTP en / recibida");
    server.send(200, "text/html", htmlPage);
}

void handleTimeUpdate()
{
    Serial.println("Solicitud HTTP en /setTime recibida");

    if (server.hasArg("timestamp"))
    {
        String timestampStr = server.arg("timestamp");
        time_t timestamp = timestampStr.toInt();
        struct timeval tv;
        tv.tv_sec = timestamp;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        server.send(200, "text/plain", "OK");
    }
    else
    {
        server.send(400, "text/plain", "Missing timestamp parameter");
    }
}

void handleNotFound()
{
    server.send(404, "text/plain", "Página no encontrada");
}

void initWebServer()
{
    // Configurar rutas del servidor
    /*server.on("/", HTTP_GET, handleRoot);
    server.on("/setTime", HTTP_GET, handleTimeUpdate);
    server.onNotFound(handleNotFound);

    // Iniciar servidor
    server.begin();*/
    Serial.println("Servidor HTTP iniciado");

    // agrego estos wifis para conectarme automaticamente....
    wifiMulti.addAP("iPhone de Javi", "Efficast1"); // javier
    wifiMulti.addAP("ISET57CLARO", "GONZALO1981");  // iset
    wifiMulti.addAP("ISET57", "12345678");          // otro que hay que compartir

    wifiConnect();
}

bool isTimeSet()
{
    time_t now;
    time(&now);
    return (now > 1759968000); // verifico si es mayor a 9/10/2025
}

bool syncTimeWithNTP()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        static unsigned long lasTimeAtHourOk = 0;
        if (millis() - lasTimeAtHourOk < 3600000 && isTimeSet())
            return true; // si ya se sincronizo hace menos de una hora, no lo hago otra vez.
        lasTimeAtHourOk = millis();

        Serial.println("Iniciando hora con NTP...");
        configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2);

        unsigned long start = millis();
        time_t now = 0;

        while ((millis() - start) < 10000)
        {
            time(&now);
            if (now > 1000000)
            {
                struct tm timeinfo;
                localtime_r(&now, &timeinfo);
                char buf[64];
                strftime(buf, sizeof(buf), "%c", &timeinfo);
                Serial.print("Hora sincronizada: ");
                Serial.println(buf);
                return true;
            }
        }
        Serial.println("No se pudo sincronizar la hora (timeout)");
    }
    return false;
}
