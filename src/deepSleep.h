#include <arduino.h>
#include <loraConfig.h>
#include <EEPROM.h>

void PrintRuntime()
{
    long seconds = millis() / 1000;
    uint16_t eepAddr = 0;
    EEPROM.put(eepAddr, seconds);
    eepAddr += sizeof(seconds);
    EEPROM.commit();

    Serial.print("Runtime: ");
    Serial.print(seconds);
    Serial.println(" seconds");
}

void SaveLMICToRTC(int deepsleep_sec)
{
    Serial.println(F("Save LMIC to RTC"));
    RTC_LMIC = LMIC;

    // ESP32 can't track millis during DeepSleep and no option to advanced millis after DeepSleep.
    // Therefore reset DutyCyles

    unsigned long now = millis();

    // EU Like Bands
#if defined(CFG_LMIC_EU_like)
    Serial.println(F("Reset CFG_LMIC_EU_like band avail"));
    for (int i = 0; i < MAX_BANDS; i++)
    {
        ostime_t correctedAvail = RTC_LMIC.bands[i].avail - ((now / 1000.0 + deepsleep_sec) * OSTICKS_PER_SEC);
        if (correctedAvail < 0)
        {
            correctedAvail = 0;
        }
        RTC_LMIC.bands[i].avail = correctedAvail;
    }

    RTC_LMIC.globalDutyAvail = RTC_LMIC.globalDutyAvail - ((now / 1000.0 + deepsleep_sec) * OSTICKS_PER_SEC);
    if (RTC_LMIC.globalDutyAvail < 0)
    {
        RTC_LMIC.globalDutyAvail = 0;
    }
#else
    Serial.println(F("No DutyCycle recalculation function!"));
#endif
}

void LoadLMICFromRTC()
{
    Serial.println(F("Load LMIC from RTC"));
    LMIC = RTC_LMIC;
}

void GoDeepSleep()
{
    Serial.println(F("Go DeepSleep"));
    PrintRuntime();
    Serial.flush();
    esp_sleep_enable_timer_wakeup(TX_INTERVAL * 1000000);
    esp_deep_sleep_start();
}