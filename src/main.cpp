#include <main.hpp>
//#define FIRSTTIME_CONFIG_EPPROM 1 // SI ES LA PRIMERA VEZ QUE SE USA ESTE DISPOSITIVO DECLARAR ESTA LINEA SOLO LA PRIMERA VEZ

void setup()
{
    Serial.begin(115200);
    EEPROM.begin(1024);
    Serial.println(F("Starting DeepSleep WaterCounter Gesinen V1.0.0"));
    // PrintLMICVersion();

#ifdef FIRSTTIME_CONFIG_EPPROM
    saveConfig();
#endif
    readSavedData();
    Serial.print("Interval (seconds): ");
    Serial.println(TX_INTERVAL);
    // LMIC init
    os_init();

    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    if (RTC_LMIC.seqnoUp != 0)
    {
        LoadLMICFromRTC();
    }

    // LoraWANDebug(LMIC);
    //do_send(&sendjob, mydata, 6);

    whenWakeUp();
}

void loop()
{
    static unsigned long lastPrintTime = 0;

    os_runloop_once();
    const bool timeCriticalJobs = os_queryTimeCriticalJobs(ms2osticksRound((TX_INTERVAL * 1000)));
    if (!timeCriticalJobs && GOTO_DEEPSLEEP == true && !(LMIC.opmode & OP_TXRXPEND))
    {
        Serial.print(F("Can go sleep "));
        LoraWANPrintLMICOpmode();
        SaveLMICToRTC(TX_INTERVAL);
        GoDeepSleep(TX_INTERVAL);
    }
    else if (lastPrintTime + 2000 < millis())
    {
        Serial.print(F("Cannot sleep "));
        Serial.print(F("TimeCriticalJobs: "));
        Serial.print(timeCriticalJobs);
        Serial.print(" ");

        LoraWANPrintLMICOpmode();
        PrintRuntime();
        lastPrintTime = millis();
    }
}
