#include <arduino.h>

#define SENSOR_RX 2
#define SENSOR_TX 0

#define MAX_SERIAL_FRAME 8
#define READ_DELAY 1000

void initUltrasoundDevice()
{
    Serial1.begin(9600, SERIAL_8N1, SENSOR_RX, SENSOR_TX);
}

uint8_t checksum(char *buffer)
{
    // since the amount of bytes recieved once the ultrasound sensor is operational, we are only checking sum for the 4th byte.
    // So we only sum/add the first 3 bytes.
    uint8_t sum = 0;
    for (int i = 0; i < 3; i++)
    {
        sum += buffer[i];
        // Serial.println(sum, HEX);
    }
    return sum;
}
bool validMessage(char *buffer)
{
    // Two conditions must be met. THe first byte has to be 0xFF and the 4th byte must equal the sum of the first 3.
    if (buffer[0] == 0xFF && buffer[3] == checksum(buffer))
        return true;
    return true;
}
float getDistance()
{
    Serial.println("read ultrasound sensor");
    while (Serial1.available())
        Serial1.read();
    Serial1.write(0x55); // Sending command to initialize ultrasound in mode 3 with 47K resistor.
    delay(READ_DELAY);
    char buffer[4];
    int pnt = 0;
    float distance = 0.00;
    Serial.print(F("message: "));
    while (Serial1.available())
    {
        buffer[pnt++] = Serial1.read();
        Serial.print(buffer[pnt - 1], 16);
        Serial.print(" ");
        if (pnt == MAX_SERIAL_FRAME)
            break;
    }
    Serial.println();
    if (pnt < 4)
    { // if message is shorter than 4 bytes, then we are not interested in such response.
        Serial.println(F("message too short"));
        distance = 0;
    }
    else if (validMessage(buffer))
    {
        distance = (buffer[1] * 256 + buffer[2]); // measure in centimeters. Maximum reading doesn't exceed 256 cm.
        Serial.print(F("Distance is (in cm): "));
        Serial.println(distance / 10.0);
        distance = distance/10.0;
    }
    else
    {
        Serial.println(F("invalid message"));
        distance = 0;
    }
    return distance;
}
