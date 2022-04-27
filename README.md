# WATERCOUNTER
Proyecto realizado por Daniel Burruchaga Sola (danielburru@gmail.com) 27/04/22
Trata de contar las vueltas de los contadores de agua mediante un sensor, este en uno de sus polos debe tener 5V y al realizar la cuenta el otro polo se activará (como un relé) estos pines deben de ir conectados a los pines 2, 12, 13 
```cpp
#define BUTTON_PIN_BITMASK 0x3004 // HEX(2^12 + 2^12+2^13) = 0x3004
`````   
Estas vueltas se detectan mediante estado un alto, este numero de vueltas se multiplica por una variable, configurable mediante lora con SET VAR A/B/C para cada uno de los canales disponibles 3 en total, por lo que se pueden conectar 3 contadores uno a cada pin. PIN 2 -- canal A // PIN 12 -- canal B // PIN 13 -- canal C
```cpp
    long valueA = varA * bootNumCountA + offsetA; // pin2
    long valueB = varB * bootNumCountB + offsetB; // pin12
    long valueC = varC * bootNumCountC + offsetC; // pin13
`````   
## DOWNLINK 
### SET INTERVAL (seconds) 
```cpp
        /**
         * @brief SET_INTERVAL_SECONDS
         * PUT intervalTx in seconds
         * Example:
         *  0xA0 0x09  => This command will put intervalTx in 9 seconds
         */
`````   
### SET INTERVAL (minuts) 
```cpp
       /**
         * @brief SET_INTERVAL_MINUTS
         * PUT intervalTx in Minuts
         * Example:
         *  0xA0 0x09  => This command will put intervalTx in 9 minuts
         */
`````   
### SET VAR A 
```cpp
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
`````   
### SET VAR B
```cpp
        /**
         * @brief SET_VAR_B
         * PUT varB
         * Example:
         *  0xA2 0x01 0x01 0x01 0x01  = 16843009
         *  0xA3 0x00 0x00 0x00 0x00 Header + un long 4 bytes
         * SI hay error enviar :
         *  0xA3 0xff
         * Si todo OK envia lo mismo que ha llegado
         *  0xA3 0x00
         */
`````   

### SET VAR C
```cpp
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
`````   

### SET OFFSET A
```cpp
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
`````   
        
### SET OFFSET B
```cpp
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
`````   
### SET OFFSET C
```cpp
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
`````   
## UPLINK
### SEND STATUS
```cpp

        /**
         * @brief sendStatus
         *  send status to lora
         * Enviamos:
         *  160   || 24 48 0 0 || 176 196 18 0 || 12 0 0 0 ||
         *  Header     long1          long2         long3
         *
         * 160 -- Header
         *
         * 24 48 0 0 -- long VarA  en hexadecimal == 18 30 0 0  -- en binario 00011000 00110000  == ordenamos donde esta el             byte 1 lo pasamos al 2  0011000000011000 = 12312
         *
         * 179 196 18 0 -- long VarB en hexadecimal == b0 c4 12 0  --- en binario =   10110000 11000100 00010010 00000000 --- ordenamos cambiando // el byte 1 a la posicion byte 4// el byte 2 a la posicion byte 3 // el byte 3 a la posicion byte 2 //  el byte 4 a la posicion byte 1  resultado =  00000000000100101100010010110000  = 1230000
         *
         * 12 0 0 0 -- long VarC
         */
`````   
