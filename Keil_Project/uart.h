#include <LPC213X.h>

void UART0_Init(void) {
    int divisor = 3000000 / (16 * 9600);
    PINSEL0 |= (1 << 0) | (1 << 2);
    U0LCR = 0x83;
    U0DLL = divisor & 0xFF;
    U0DLM = (divisor >> 8) & 0xFF;
    U0LCR = 0x03;
    U0FCR = 0x07;
}

void UART0_SendChar(char ch) {
    while (!(U0LSR & (1 << 5)));
    U0THR = ch;
}

void UART0_SendString(char *str) {
    while (*str) {
        UART0_SendChar(*str++);
    }
}

void UART0_MoveCursorUp(int lines) {
    UART0_SendChar(27); // ESC character
    UART0_SendString("[");
    UART0_SendChar('0' + lines);
    UART0_SendChar('A'); // ANSI code for moving cursor up
}
