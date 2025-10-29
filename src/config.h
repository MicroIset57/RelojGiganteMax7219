#ifndef CONFIG_H
#define CONFIG_H

// WiFi AP configuration
#define AP_SSID "Reloj-ISET57"
#define AP_PASSWORD "12345678"
#define AP_CHANNEL 1
#define AP_MAX_CONNECTIONS 1
#define AP_HIDDEN false

// WiFi Station configuration
#define WIFI_SSID "ISET57CLARO"
#define WIFI_PASS "GONZALO1981"

// Time configuration
#define GMT_OFFSET_SEC (-3 * 3600) // Argentina UTC-3
#define DAYLIGHT_OFFSET_SEC 0
#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.nist.gov"

// WiFi reconnection intervals
#define WIFI_CHECK_INTERVAL 300000 // Check WiFi every 5 minutes
#define NTP_SYNC_INTERVAL 3600000  // Sync time every hour if possible

#endif // CONFIG_H