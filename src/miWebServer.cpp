#include "miWebServer.h"
#include <time.h>

// Inicia la conexión a las redes WiFi
void wifiConnect()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Conectando a WiFi");
    int cont = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if (cont++ > 60) // espero 30 segundos
        {
            Serial.println("\nNo se pudo conectar a la red WiFi. ABORT!");
            esp_restart();
        }
    }

    Serial.println("\nConectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
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

        // Si ya se sincronizó hace menos de una hora Y la hora está seteada, no reintentar
        if (lasTimeAtHourOk != 0 && (millis() - lasTimeAtHourOk) < 3600000 && isTimeSet())
        {
            return true; // Ya sincronizado recientemente
        }

        Serial.println("Iniciando hora con NTP...");
        configTime(-3 * 3600, 0, "time.google.com", "time.cloudflare.com", "ar.pool.ntp.org");

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
                lasTimeAtHourOk = millis(); // Actualizar SOLO cuando la sincronización fue exitosa
                return true;
            }
            delay(100); // Evitar saturar el watchdog durante la espera de NTP
        }
        Serial.println("No se pudo sincronizar la hora (timeout)");
    }
    return false;
}
