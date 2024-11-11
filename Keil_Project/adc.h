#include <LPC213X.h>
#include <stdio.h>

void init_adc() {
    PINSEL0 = 0x00003000;
    PINSEL1 = 0x15400000;
}

unsigned int adc0_read() {
    unsigned int data;
    AD0CR = (1 << 21) | (3 << 9) | (1 << 1);
    AD0CR |= (1 << 24);
    while ((AD0GDR & (1 << 31)) == 0);
    data = AD0GDR;
    data = (data >> 6) & 0x03FF;
    return data;
}

unsigned int adc1_read() {
    unsigned int data;
    AD1CR = (1 << 21) | (3 << 9) | (1 << 1);
    AD1CR |= (1 << 24);
    while ((AD1GDR & (1 << 31)) == 0);
    data = AD1GDR;
    data = (data >> 6) & 0x03FF;
    return data;
}

float convertVoltageToPressure(float voltage) {
    float Vmin = 0.2f;
    float Vmax = 4.7f;
    float Pmax = 115.0f;
    return ((voltage - Vmin) / (Vmax - Vmin)) * Pmax;
}

float convertVoltageToTemperature(float voltage) {
    float T_min = -40.0f;
    float T_max = 150.0f;
    float Vmin = 0.5f;
    float Vmax = 3.5f;
    return ((voltage - Vmin) / (Vmax - Vmin)) * (T_max - T_min) + T_min;
}

void floatToString(float value, char *buffer, int decimalPlaces) {
    int intPart = (int)value;
    float fracPart = value - intPart;
    char intBuffer[20];
    char *p;
    int fracInt;
    int i;

    if (value < 0) {
        *buffer++ = '-';
        intPart = -intPart;
        fracPart = -fracPart;
    }

    p = intBuffer + sizeof(intBuffer) - 1;
    *p = '\0';
    do {
        *--p = (intPart % 10) + '0';
        intPart /= 10;
    } while (intPart > 0);

    while (*p) {
        *buffer++ = *p++;
    }

    if (decimalPlaces > 0) {
        *buffer++ = '.';
        for (i = 0; i < decimalPlaces; i++) {
            fracPart *= 10;
        }
        fracInt = (int)(fracPart + 0.5f);
        for (i = decimalPlaces - 1; i >= 0; i--) {
            buffer[i] = (fracInt % 10) + '0';
            fracInt /= 10;
        }
        buffer[decimalPlaces] = '\0';
        buffer += decimalPlaces;
    }

    *buffer = '\0';
}
