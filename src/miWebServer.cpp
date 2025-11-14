#include "miWebServer.h"
#include <time.h>

// Variables globales
Preferences preferences;
AsyncWebServer* server = nullptr;
bool configPortalActive = false;

// Inicia la conexión a las redes WiFi
void wifiConnect()
{
    String ssid, password;

    // Intentar cargar credenciales desde EEPROM
    if (loadWiFiCredentials(ssid, password))
    {
        Serial.println("Usando credenciales guardadas en EEPROM");
        Serial.print("Conectando a: ");
        Serial.println(ssid);
    }
    else
    {
        // Si no hay credenciales guardadas, usar las hardcodeadas como fallback
        Serial.println("No hay credenciales en EEPROM, usando configuración por defecto");
        ssid = WIFI_SSID;
        password = WIFI_PASS;
    }

    // Configurar modo Station
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    Serial.print("Conectando a WiFi");
    int cont = 0;
    int maxAttempts = 60; // 30 segundos (60 x 500ms)

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        cont++;

        if (cont >= maxAttempts)
        {
            Serial.println("\nNo se pudo conectar a la red WiFi después de varios intentos");

            // Si las credenciales venían de EEPROM, borrarlas porque son inválidas
            if (hasStoredCredentials())
            {
                Serial.println("Borrando credenciales inválidas...");
                clearWiFiCredentials();
            }

            // NO reiniciar - retornar para que el caller maneje la situación
            Serial.println("Conexión WiFi FALLÓ - se requiere configuración");
            WiFi.disconnect();
            return;
        }
    }

    Serial.println("\n¡Conectado exitosamente!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
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

// ============================================================================
// Funciones de gestión de credenciales WiFi en EEPROM (Preferences)
// ============================================================================

void saveWiFiCredentials(const char* ssid, const char* password)
{
    Serial.println("Guardando credenciales WiFi en EEPROM...");
    preferences.begin("wifi", false); // false = modo lectura/escritura
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();
    Serial.print("SSID guardado: ");
    Serial.println(ssid);
    Serial.println("Password guardado correctamente");
}

bool loadWiFiCredentials(String &ssid, String &password)
{
    Serial.println("Cargando credenciales WiFi desde EEPROM...");
    preferences.begin("wifi", true); // true = solo lectura
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    preferences.end();

    if (ssid.length() > 0)
    {
        Serial.print("SSID cargado: ");
        Serial.println(ssid);
        Serial.println("Password cargado correctamente");
        return true;
    }
    else
    {
        Serial.println("No se encontraron credenciales guardadas");
        return false;
    }
}

void clearWiFiCredentials()
{
    Serial.println("Borrando credenciales WiFi de EEPROM...");
    preferences.begin("wifi", false);
    preferences.clear(); // Borra todas las claves del namespace "wifi"
    preferences.end();
    Serial.println("Credenciales borradas exitosamente");
}

bool hasStoredCredentials()
{
    preferences.begin("wifi", true);
    String ssid = preferences.getString("ssid", "");
    preferences.end();
    bool hasCredentials = (ssid.length() > 0);

    Serial.print("¿Tiene credenciales guardadas? ");
    Serial.println(hasCredentials ? "SI" : "NO");

    return hasCredentials;
}

// ============================================================================
// Funciones de Access Point y Portal de Configuración
// ============================================================================

// Página HTML para el portal de configuración WiFi
const char HTML_CONFIG_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Reloj ISET 57 - Configuración WiFi</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 15px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.3);
            padding: 30px;
            max-width: 500px;
            width: 100%;
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 10px;
            font-size: 24px;
        }
        h2 {
            color: #667eea;
            text-align: center;
            margin-bottom: 30px;
            font-size: 16px;
            font-weight: normal;
        }
        .status {
            background: #e3f2fd;
            border-left: 4px solid #2196f3;
            padding: 15px;
            margin-bottom: 20px;
            border-radius: 5px;
        }
        .status.success { background: #e8f5e9; border-left-color: #4caf50; }
        .status.error { background: #ffebee; border-left-color: #f44336; }
        label {
            display: block;
            margin-bottom: 8px;
            color: #555;
            font-weight: bold;
        }
        select, input {
            width: 100%;
            padding: 12px;
            margin-bottom: 20px;
            border: 2px solid #ddd;
            border-radius: 8px;
            font-size: 16px;
            transition: border-color 0.3s;
        }
        select:focus, input:focus {
            outline: none;
            border-color: #667eea;
        }
        button {
            width: 100%;
            padding: 15px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: transform 0.2s;
        }
        button:hover { transform: translateY(-2px); }
        button:active { transform: translateY(0); }
        button:disabled {
            background: #ccc;
            cursor: not-allowed;
            transform: none;
        }
        .refresh-btn {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            margin-bottom: 20px;
        }
        .loading {
            text-align: center;
            color: #667eea;
            margin: 20px 0;
        }
        .spinner {
            border: 3px solid #f3f3f3;
            border-top: 3px solid #667eea;
            border-radius: 50%;
            width: 30px;
            height: 30px;
            animation: spin 1s linear infinite;
            margin: 10px auto;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Reloj Gigante ISET 57</h1>
        <h2>Configuración WiFi</h2>

        <div id="status" class="status" style="display:none;"></div>

        <button class="refresh-btn" onclick="scanNetworks()">Escanear Redes WiFi</button>

        <div id="loading" class="loading" style="display:none;">
            <div class="spinner"></div>
            <p>Escaneando redes...</p>
        </div>

        <form id="wifiForm">
            <label for="ssid">Red WiFi:</label>
            <select id="ssid" name="ssid" required>
                <option value="">Seleccione una red...</option>
            </select>

            <label for="password">Contraseña:</label>
            <input id="password" name="password" placeholder="Ingrese la contraseña" required>

            <button type="submit" id="connectBtn">Conectar</button>
        </form>
    </div>

    <script>
        function showStatus(message, type = 'info') {
            const status = document.getElementById('status');
            status.textContent = message;
            status.className = 'status ' + type;
            status.style.display = 'block';
        }

        async function scanNetworks() {
            const loading = document.getElementById('loading');
            const select = document.getElementById('ssid');

            loading.style.display = 'block';
            showStatus('Escaneando redes WiFi...', 'info');

            try {
                const response = await fetch('/scan');
                const networks = await response.json();

                select.innerHTML = '<option value="">Seleccione una red...</option>';

                networks.forEach(network => {
                    const option = document.createElement('option');
                    option.value = network.ssid;
                    option.textContent = `${network.ssid} (${network.rssi} dBm) ${network.encryption ? '🔒' : ''}`;
                    select.appendChild(option);
                });

                showStatus(`${networks.length} redes encontradas`, 'success');
            } catch (error) {
                showStatus('Error al escanear redes: ' + error.message, 'error');
            } finally {
                loading.style.display = 'none';
            }
        }

        document.getElementById('wifiForm').addEventListener('submit', async (e) => {
            e.preventDefault();

            const ssid = document.getElementById('ssid').value;
            const password = document.getElementById('password').value;
            const connectBtn = document.getElementById('connectBtn');

            if (!ssid) {
                showStatus('Por favor seleccione una red WiFi', 'error');
                return;
            }

            connectBtn.disabled = true;
            connectBtn.textContent = 'Conectando...';
            showStatus('Intentando conectar a ' + ssid + '...', 'info');

            try {
                const response = await fetch('/connect', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ ssid, password })
                });

                const result = await response.json();

                if (result.success) {
                    // Ocultar formulario
                    document.getElementById('wifiForm').style.display = 'none';
                    document.querySelector('.refresh-btn').style.display = 'none';

                    // Mostrar mensaje de éxito grande
                    const container = document.querySelector('.container');
                    container.innerHTML = `
                        <h1 style="color: #4caf50; font-size: 48px; margin-bottom: 20px;">✓</h1>
                        <h2 style="color: #4caf50; font-size: 32px; margin-bottom: 30px;">¡OK, Listo!</h2>
                        <div class="status success" style="display: block; font-size: 18px;">
                            <p><strong>Conectado exitosamente a: ${ssid}</strong></p>
                            <p style="margin-top: 10px;">IP asignada: ${result.ip}</p>
                            <p style="margin-top: 20px;">El reloj se reiniciará en modo normal en unos segundos...</p>
                            <p style="margin-top: 10px; font-size: 16px;">Ya puede cerrar esta página.</p>
                        </div>
                    `;
                } else {
                    showStatus('Error al conectar: ' + result.message, 'error');
                    connectBtn.disabled = false;
                    connectBtn.textContent = 'Conectar';
                }
            } catch (error) {
                showStatus('Error de comunicación: ' + error.message, 'error');
                connectBtn.disabled = false;
                connectBtn.textContent = 'Conectar';
            }
        });

        // Escanear automáticamente al cargar la página
        window.addEventListener('load', () => {
            scanNetworks();
        });
    </script>
</body>
</html>
)rawliteral";

bool isConfigPortalActive()
{
    return configPortalActive;
}

void stopConfigPortal()
{
    if (!configPortalActive)
    {
        Serial.println("Portal de configuración no está activo");
        return;
    }

    Serial.println("Deteniendo portal de configuración...");

    // Detener servidor web
    if (server != nullptr)
    {
        server->end();
        delete server;
        server = nullptr;
        Serial.println("Servidor web detenido");
    }

    // Apagar modo AP
    WiFi.softAPdisconnect(true);
    Serial.println("Modo Access Point desactivado");

    configPortalActive = false;
    Serial.println("Portal de configuración detenido exitosamente");
}

void startConfigPortal()
{
    if (configPortalActive)
    {
        Serial.println("Portal de configuración ya está activo");
        return;
    }

    Serial.println("===========================================");
    Serial.println("Iniciando portal de configuración WiFi...");
    Serial.println("===========================================");

    // Configurar WiFi en modo Access Point
    WiFi.mode(WIFI_AP);

    // Configurar IP estática del AP
    IPAddress local_ip(AP_IP_ADDR);
    IPAddress gateway(AP_GATEWAY);
    IPAddress subnet(AP_SUBNET);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    // Iniciar Access Point
    bool apStarted = WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, 0, AP_MAX_CONNECTIONS);

    if (!apStarted)
    {
        Serial.println("ERROR: No se pudo iniciar el Access Point");
        return;
    }

    Serial.println("Access Point iniciado exitosamente");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("Contraseña: (ninguna)");

    // Crear servidor web
    server = new AsyncWebServer(80);

    // ========== Endpoint: Página principal (/) ==========
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("HTTP GET / - Sirviendo página de configuración");
        request->send_P(200, "text/html", HTML_CONFIG_PAGE);
    });

    // ========== Endpoint: Escanear redes WiFi (/scan) ==========
    server->on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("HTTP GET /scan - Escaneando redes WiFi...");

        int n = WiFi.scanNetworks();
        Serial.print("Redes encontradas: ");
        Serial.println(n);

        JsonDocument doc;
        JsonArray networks = doc.to<JsonArray>();

        for (int i = 0; i < n; i++)
        {
            JsonObject network = networks.add<JsonObject>();
            network["ssid"] = WiFi.SSID(i);
            network["rssi"] = WiFi.RSSI(i);
            network["encryption"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);

            Serial.print("  - ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.println(" dBm)");
        }

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);

        WiFi.scanDelete(); // Liberar memoria del escaneo
    });

    // ========== Endpoint: Conectar a WiFi (/connect) ==========
    server->on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

        Serial.println("HTTP POST /connect - Recibiendo credenciales WiFi");

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error)
        {
            Serial.print("Error al parsear JSON: ");
            Serial.println(error.c_str());
            request->send(400, "application/json", "{\"success\":false,\"message\":\"JSON inválido\"}");
            return;
        }

        const char* ssid = doc["ssid"];
        const char* password = doc["password"];

        if (!ssid || strlen(ssid) == 0)
        {
            Serial.println("Error: SSID vacío");
            request->send(400, "application/json", "{\"success\":false,\"message\":\"SSID vacío\"}");
            return;
        }

        Serial.print("Intentando conectar a: ");
        Serial.println(ssid);
        Serial.println("Password recibido");

        // Intentar conectar a la red WiFi
        WiFi.mode(WIFI_AP_STA); // Modo AP + Station simultáneo
        WiFi.begin(ssid, password);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20)
        {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("¡Conexión exitosa!");
            Serial.print("IP asignada: ");
            Serial.println(WiFi.localIP());

            // Guardar credenciales en EEPROM
            saveWiFiCredentials(ssid, password);

            // Responder al cliente
            String response = "{\"success\":true,\"message\":\"Conectado\",\"ip\":\"";
            response += WiFi.localIP().toString();
            response += "\"}";
            request->send(200, "application/json", response);

            // Programar reinicio del ESP32 en 3 segundos
            Serial.println("Reiniciando en 3 segundos para aplicar configuración...");
            delay(3000);
            esp_restart();
        }
        else
        {
            Serial.println("Error: No se pudo conectar a la red WiFi");
            WiFi.disconnect();
            WiFi.mode(WIFI_AP); // Volver a modo AP solamente

            request->send(200, "application/json",
                "{\"success\":false,\"message\":\"No se pudo conectar. Verifique la contraseña.\"}");
        }
    });

    // ========== Endpoint: Estado de conexión (/status) ==========
    server->on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("HTTP GET /status - Consultando estado");

        JsonDocument doc;
        doc["configPortalActive"] = configPortalActive;
        doc["wifiConnected"] = (WiFi.status() == WL_CONNECTED);
        doc["apClients"] = WiFi.softAPgetStationNum();

        if (WiFi.status() == WL_CONNECTED)
        {
            doc["ip"] = WiFi.localIP().toString();
            doc["ssid"] = WiFi.SSID();
        }

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // Iniciar servidor
    server->begin();
    Serial.println("Servidor web iniciado en http://192.168.4.1");
    Serial.println("===========================================");

    configPortalActive = true;
}
