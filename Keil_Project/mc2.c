#include <LPC213X.H>

void delay(unsigned int t) {
    unsigned int i, j;
    for (j = 0; j < t; j++) {
        for (i = 0; i < 1275; i++);
    }
}

void UART0_Init(void) {
    int divisor = 3000000 / (16 * 9600);
    PINSEL0 |= (1 << 0) | (1 << 2); 
    U0LCR = 0x83;                  
    U0DLL = divisor & 0xFF;
    U0DLM = (divisor >> 8) & 0xFF;
    U0LCR = 0x03;                                 
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

void ftostr(float num, char *str) {
    int integer_part = (int)num;               
    int decimal_part = (int)((num - integer_part) * 100); 
    int i = 0, j;   
    if (integer_part == 0) {
        str[i++] = '0';
    } else {
        int temp = integer_part, digits = 0;
        char pre_str[10];
        while (temp) {
            pre_str[digits++] = (temp % 10) + '0';
            temp /= 10;
        }
        for (j = digits - 1; j >= 0; j--) {
            str[i++] = pre_str[j];
        }
    }
    str[i++] = '.';
    str[i++] = (decimal_part / 10) + '0';
    str[i++] = (decimal_part % 10) + '0';
    str[i] = '\0';  
}

void ADC0_Init() {
    PINSEL1 = (1 << 24); 
}

unsigned int ADC0_Read() {
    unsigned int data;
    AD0CR = (1 << 21) | (3 << 9) | (1 << 1);
    AD0CR |= (1 << 24);  
    while ((AD0GDR & (1 << 31)) == 0);
    data = AD0GDR;
    data = (data >> 6) & 0x03FF;  
    return data;
}

int main() {
    float pressure, tmp;
    char pre_str[10];
    char buffer[100];
    int i, j;
		unsigned int val;
    char prefix[] = "uC2 - Pressure: ";
    UART0_Init();
    ADC0_Init();
    IODIR0 = (1<<4);

    while (1) {
		
        val = ADC0_Read();
        pressure = (((float)val * 3.3 / 1023.0 + 0.095) / 0.009) - 1.46;
        ftostr(pressure, pre_str);
				
				
				if(pressure > 90)
					IOSET0 = (1<<4);
				else
					IOCLR0 = (1<<4);
        
        i = j = 0;
        while (prefix[j] != '\0') 
            buffer[i++] = prefix[j++];
        j = 0;
        while (pre_str[j] != '\0') 
            buffer[i++] = pre_str[j++];
        buffer[i++] = 'k'; 
        buffer[i++] = 'P';
        buffer[i++] = 'a'; 
				buffer[i++] = '\n';
        buffer[i] = '\0';
        UART0_SendString(buffer);
				
				delay(200);
    }
}
