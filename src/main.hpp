#include <arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#define BUCHUTEST
#include <SPI.h>
#include <ttn_credentials.h>
#include <deepSleep.h>
#define RCMDPORT 4

bool GOTO_DEEPSLEEP = false;
uint8_t mydata[13] = {};
// Schedule TX every this many seconds
RTC_DATA_ATTR int TX_INTERVAL = 30;

unsigned long varA = 10;
unsigned long varB = 10;
unsigned long varC = 10;

unsigned long offsetA = 0;
unsigned long offsetB = 0;
unsigned long offsetC = 0;

RTC_DATA_ATTR int bootNumCountA = 0;
RTC_DATA_ATTR int bootNumCountB = 0;
RTC_DATA_ATTR int bootNumCountC = 0;

#define SET_INTERVAL_SECONDS 0xA0
#define SET_INTERVAL_MINUTS 0xA1
#define SET_VAR_A 0xA2
#define SET_VAR_B 0xA3
#define SET_VAR_C 0xA4
#define SET_OFFSET_A 0xA5
#define SET_OFFSET_B 0xA6
#define SET_OFFSET_C 0xA7
/**
 * @brief saveConfig
 *  save the data in eeprom
 */
void saveConfig()
{
    Serial.println("saving config");
    uint16_t addr = 10;

    EEPROM.put(addr, TX_INTERVAL);
    addr += sizeof(TX_INTERVAL);

    EEPROM.put(addr, varA);
    addr += sizeof(varA);
    EEPROM.put(addr, varB);
    addr += sizeof(varB);
    EEPROM.put(addr, varC);
    addr += sizeof(varC);

    EEPROM.put(addr, offsetA);
    addr += sizeof(offsetA);
    EEPROM.put(addr, offsetA);
    addr += sizeof(offsetA);
    EEPROM.put(addr, offsetC);
    addr += sizeof(offsetC);

    EEPROM.put(addr, bootNumCountA);
    addr += sizeof(bootNumCountA);
    EEPROM.put(addr, bootNumCountB);
    addr += sizeof(bootNumCountB);
    EEPROM.put(addr, bootNumCountC);
    addr += sizeof(bootNumCountC);

    EEPROM.commit();
}

/**
 * @brief readSavedData
 *  read the data saved in eeprom
 */
void readSavedData()
{
    Serial.println("reading saved config");

    uint16_t addr = 10;
    EEPROM.get(addr, TX_INTERVAL);
    addr += sizeof(TX_INTERVAL);

    EEPROM.get(addr, varA);
    addr += sizeof(varA);
    EEPROM.get(addr, varB);
    addr += sizeof(varB);
    EEPROM.get(addr, varC);
    addr += sizeof(varC);

    EEPROM.get(addr, offsetA);
    addr += sizeof(offsetA);
    EEPROM.get(addr, offsetA);
    addr += sizeof(offsetA);
    EEPROM.get(addr, offsetC);
    addr += sizeof(offsetC);

    EEPROM.get(addr, bootNumCountA);
    addr += sizeof(bootNumCountA);
    EEPROM.get(addr, bootNumCountB);
    addr += sizeof(bootNumCountB);
    EEPROM.get(addr, bootNumCountC);
    addr += sizeof(bootNumCountC);
}
/**
 * @brief command_callback
 *     callback when we recive some data
 *
 * @param buff recived buff
 * @param size size of recived buff
 */
void command_callback(uint8_t *buff, uint8_t size)
{
    if (size < 2)
        return;

    switch (buff[0])
    {
        /**
         * @brief SET_INTERVAL_SECONDS
         * PUT intervalTx in seconds
         * Example:
         *  0xA0 0x09  => This command will put intervalTx in 9 seconds
         */
    case SET_INTERVAL_SECONDS:
        if (size == 2)
        {
            buff[0] = SET_INTERVAL_SECONDS;
            TX_INTERVAL = buff[1];
            Serial.print("tx interval set in seconds: ");
            Serial.println(TX_INTERVAL);
            // sendData(RESPONSE_PORT, buff, size);
            saveConfig();
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_INTERVAL_SECONDS;
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);
        }
        break;
        /**
         * @brief SET_INTERVAL_MINUTS
         * PUT intervalTx in Minuts
         * Example:
         *  0xA0 0x09  => This command will put intervalTx in 9 minuts
         */
    case SET_INTERVAL_MINUTS:
        if (size == 2)
        {

            buff[0] = SET_INTERVAL_MINUTS;
            TX_INTERVAL = buff[1] * 60;
            Serial.print("tx interval set in seconds: ");
            Serial.println(TX_INTERVAL);
            // sendData(RESPONSE_PORT, buff, size);
            saveConfig();
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_INTERVAL_MINUTS;
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);
        }
        break;
        /**
         * @brief SET_VAR_A
         * PUT varA
         * Example:
         * Header + un long 4 bytes
         *  0xA2 0x01 0x00 0x00 0x00 = 1
         * 0xA2 0x01 0x01 0x01 0x01  = 16843009
         * SI hay error enviar :
         *  0xA2 0xff
         * Si todo OK envia lo mismo que ha llegado
         *  0xA2 0x00
         */
    case SET_VAR_A:
        if (size == 5)
        {
            varA = buff[4] * (256 * 256 * 256) + buff[3] * (256 * 256) + buff[2] * (256) + buff[1] * (1);
            Serial.print("SET_VAR_A : ");
            Serial.println(varA);

            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_VAR_A;
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);

            // sendData(RESPONSE_PORT, _bufTemp, sizeof(_bufTemp));
            saveConfig();
        }
        else
        {
            // ENVIAR ERROR FF
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_VAR_A;
            _bufTemp[1] = 0xff;
            do_send(&sendjob, _bufTemp, 2);

            //  sendData(RESPONSE_PORT, _bufTemp, sizeof(_bufTemp));
        }
        break;
        /**
         * @brief SET_VAR_B
         * PUT varB
         * Example:
         *  0xA3 0x00 0x00 0x00 0x00 Header + un long 4 bytes
         * SI hay error enviar :
         *  0xA3 0xff
         * Si todo OK envia lo mismo que ha llegado
         *  0xA3 0x00
         */
    case SET_VAR_B:
        if (size == 5)
        {
            varB = buff[4] * (256 * 256 * 256) + buff[3] * (256 * 256) + buff[2] * (256) + buff[1] * (1);
            Serial.print("SET_VAR_B : ");
            Serial.println(varB);
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_VAR_B;
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);
            // sendData(RESPONSE_PORT, _bufTemp, sizeof(_bufTemp));
            saveConfig();
        }
        else
        {
            // ENVIAR ERROR FF
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_VAR_B;
            _bufTemp[1] = 0xff;
            do_send(&sendjob, _bufTemp, 2);
            // sendData(RESPONSE_PORT, _bufTemp, sizeof(_bufTemp));
        }
        break;
        /**
         * @brief SET_VAR_C
         * PUT varC
         * Example:
         *  0xA4 0x00 0x00 0x00 0x00 Header + un long 4 bytes
         * SI hay error enviar :
         *  0xA4 0xff
         * Si todo OK envia lo mismo que ha llegado
         *  0xA4 0x00
         */
    case SET_VAR_C:
        if (size == 5)
        {
            varC = buff[4] * (256 * 256 * 256) + buff[3] * (256 * 256) + buff[2] * (256) + buff[1] * (1);
            Serial.print("SET_VAR_C : ");
            Serial.println(varC);
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_VAR_C;
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);
            // sendData(RESPONSE_PORT, _bufTemp, sizeof(_bufTemp));
            saveConfig();
        }
        else
        {
            // ENVIAR ERROR FF
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_VAR_C;
            _bufTemp[1] = 0xff;
            do_send(&sendjob, _bufTemp, 2);
            // sendData(RESPONSE_PORT, _bufTemp, sizeof(_bufTemp));
        }
        break;
         /**
         * @brief SET_OFFSET_A
         * PUT OFFSET_A
         * Example:
         *  0xA5 0x00 0x00 0x00 0x00 Header + un long 4 bytes
         *  0xA5 0x09 0x00 0x00 0x00 Header + 9 de offsetA
         * SI hay error envia :
         *  0xA5 0xff
         * Si todo OK envia lo mismo que ha llegado
         *  0xA5 0x00
         */
    case SET_OFFSET_A:

        if (size == 5)
        {
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_OFFSET_A;
            offsetA = buff[4] * (256 * 256 * 256) + buff[3] * (256 * 256) + buff[2] * (256) + buff[1] * (1);
            Serial.print("SET_OFFSET_A : ");
            Serial.println(offsetA);
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);
            saveConfig();
        }
        else
        {
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_OFFSET_A;
            // ENVIAR ERROR FF
            _bufTemp[1] = 0xff;
            do_send(&sendjob, _bufTemp, 2);
        }
        break;
        /**
         * @brief SET_OFFSET_B
         * PUT OFFSET_B
         * Example:
         *  0xA6 0x00 0x00 0x00 0x00 Header + un long 4 bytes
         *  0xA6 0x09 0x00 0x00 0x00 Header + 9 de offsetA
         * SI hay error envia :
         *  0xA6 0xff
         * Si todo OK envia lo mismo que ha llegado
         *  0xA6 0x00
         */
    case SET_OFFSET_B:

        if (size == 5)
        {
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_OFFSET_B;
            offsetB = buff[4] * (256 * 256 * 256) + buff[3] * (256 * 256) + buff[2] * (256) + buff[1] * (1);
            Serial.print("SET_OFFSET_B : ");
            Serial.println(offsetB);
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);
            saveConfig();
        }
        else
        {
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_OFFSET_B;
            // ENVIAR ERROR FF
            _bufTemp[1] = 0xff;
            do_send(&sendjob, _bufTemp, 2);
        }
        break;
        /**
         * @brief SET_OFFSET_C
         * PUT OFFSET_C
         * Example:
         *  0xA7 0x00 0x00 0x00 0x00 Header + un long 4 bytes
         *  0xA7 0x09 0x00 0x00 0x00 Header + 9 de offsetA
         * SI hay error envia :
         *  0xA7 0xff
         * Si todo OK envia lo mismo que ha llegado
         *  0xA7 0x00
         */
    case SET_OFFSET_C:

        if (size == 5)
        {
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_OFFSET_C;
            offsetC = buff[4] * (256 * 256 * 256) + buff[3] * (256 * 256) + buff[2] * (256) + buff[1] * (1);
            Serial.print("SET_OFFSET_C : ");
            Serial.println(offsetC);
            _bufTemp[1] = 0x00;
            do_send(&sendjob, _bufTemp, 2);
            saveConfig();
        }
        else
        {
            uint8_t _bufTemp[2];
            _bufTemp[0] = SET_OFFSET_C;
            // ENVIAR ERROR FF
            _bufTemp[1] = 0xff;
            do_send(&sendjob, _bufTemp, 2);
        }
        break;
    }
}

void onEvent(ev_t ev)
{
    char buff[24] = "";

    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("artKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                Serial.print(artKey[i], HEX);
            }
            Serial.println("");
            Serial.print("nwkKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                Serial.print(nwkKey[i], HEX);
            }
            Serial.println("");
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
        break;
    /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        strcpy_P(buff, (LMIC.txrxFlags & TXRX_ACK) ? PSTR("RECEIVED ACK")
                                                   : PSTR("TX COMPLETE"));

        if (LMIC.dataLen)
        { // did we receive payload data -> display info
            Serial.printf("Received %d bytes of payload, RSSI %d SNR %d\n",
                          LMIC.dataLen, LMIC.rssi, LMIC.snr / 4);

            if (LMIC.txrxFlags & TXRX_PORT)
            { // FPort -> use to switch

                switch (LMIC.frame[LMIC.dataBeg - 1])
                {

                case RCMDPORT: // opcode -> call rcommand interpreter
                    command_callback(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
                    break;

                default:
                    // unknown port -> display info
                    Serial.printf("Received data on unsupported port #%d\n",
                                  LMIC.frame[LMIC.dataBeg - 1]);
                    break;
                }
            }
        }
        GOTO_DEEPSLEEP = true;
        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;
    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

/**
 * @brief sendStatus
 *  send status to lora
 * Enviamos:
 *  160   || 24 48 0 0 || 176 196 18 0 || 12 0 0 0 ||
 *  Header     long1          long2         long3
 *
 * 160 -- Header
 *
 * 24 48 0 0 -- long VarA  en hexadecimal == 18 30 0 0  -- en binario 00011000 00110000  == ordenamos donde esta el byte 1 lo pasamos al 2  0011000000011000 = 12312
 *
 * 179 196 18 0 -- long VarB en hexadecimal == b0 c4 12 0  --- en binario =   10110000 11000100 00010010 00000000 --- ordenamos cambiando
 *					 // el byte 1 a la posicion byte 4// el byte 2 a la posicion byte 3 // el byte 3 a la posicion byte 2 // el byte 4 a la posicion byte 1  resultado =  00000000000100101100010010110000  = 1230000
 *
 * 12 0 0 0 -- long VarC
 */
void sendStatus()
{
    //Serial.println("SendStatus");
    long valueA = varA * bootNumCountA + offsetA;
    long valueB = varB * bootNumCountB + offsetB;
    long valueC = varC * bootNumCountC + offsetC;
    Serial.print("valueA: ");
    Serial.println(valueA);
    Serial.print("valueB: ");
    Serial.println(valueB);
    Serial.print("valueC: ");
    Serial.println(valueC);

    char *pA = (char *)&valueA;
    char *pB = (char *)&valueB;
    char *pC = (char *)&valueC;

    mydata[0] = 0xA0;

    memcpy(&mydata[1], pA, sizeof(pA));
    memcpy(&mydata[sizeof(pA) + 1], pB, sizeof(pB));
    memcpy(&mydata[sizeof(pA) + sizeof(pB) + 1], pC, sizeof(pC));

    for (byte i = 0; i < sizeof(mydata); i++)
    {
        Serial.print(mydata[i]);
        Serial.print(' ');
    }
    Serial.println();
    saveConfig();
    do_send(&sendjob, mydata, 13);
    // sendData(INFO_PORT, (uint8_t *)_buffToSend, sizeof(_buffToSend)); // REVISAR QUE ENVIA LO QUE DEBE
}

/**
 * whenWakeUp
 *   its a function which will run at wakeUp
 */
void whenWakeUp()
{
    // nos indica que numero gpio se ha activado
    int wakeUpGPIO = log(esp_sleep_get_ext1_wakeup_status() / log(2));

    switch (wakeUpGPIO)
    {
    case 1: // 2
        Serial.println("Pin 2 High");
        bootNumCountA++;
        Serial.println("numero de boot: " + String(bootNumCountA));

        break;
    case 8: // 12
        Serial.println("Pin 12 High");
        bootNumCountB++;
        Serial.println("numero de boot: " + String(bootNumCountB));
        break;
    case 9: // 11
        Serial.println("Pin 13 High");
        bootNumCountC++;
        Serial.println("numero de boot: " + String(bootNumCountC));

        break;
    }
    sendStatus();
}
