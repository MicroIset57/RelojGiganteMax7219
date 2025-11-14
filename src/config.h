#ifndef CONFIG_H
#define CONFIG_H

// WiFi Station configuration (credenciales por defecto - ahora se usan desde EEPROM)
#define WIFI_SSID "ISET57CLARO"
#define WIFI_PASS "GONZALO1981"

// Access Point configuration (para modo de configuración)
#define AP_SSID "RelojISet57-Config"
#define AP_PASSWORD ""  // Sin password para facilitar acceso inicial
#define AP_CHANNEL 6    // Canal WiFi del AP
#define AP_MAX_CONNECTIONS 4  // Máximo de clientes conectados simultáneamente

// IP configuration del Access Point
#define AP_IP_ADDR 192, 168, 4, 1
#define AP_GATEWAY 192, 168, 4, 1
#define AP_SUBNET 255, 255, 255, 0

#endif // CONFIG_H
