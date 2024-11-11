#include <LPC213X.H>

void delay(unsigned int t) {
    unsigned int i, j;
    for (j = 0; j < t; j++) {
        for (i = 0; i < 1275; i++);
    }
}

void UART0_Init() {
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
    while (*str) 
        UART0_SendChar(*str++);
}

char UART0_ReceiveChar() {
    while (!(U0LSR & 0x01)); 
    return U0RBR;
}

void UART0_ReceiveString(char *buffer) {
    int i = 0;
		int j = 0;
		char ch;
    while (1) {
        ch = UART0_ReceiveChar();
        if (ch == '\n' || ch == '\r') 
				{ 
            buffer[i] = '\0'; 
            break;
        } 
				else
            buffer[i++] = ch; 
    }
}


void itstr(int num, char *str) 
{
    int i = 0, temp, j;
    if (num == 0) 
		{
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (num > 0) 
		{
        temp = num % 10;
        str[i++] = temp + '0';
        num /= 10;
    }
    str[i] = '\0';
    for (j = 0; j < i / 2; j++) 
		{
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}


void ADC0_Init() 
{
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





int main() 
{
    char temp_str[10], buffer[100], buff[100];
    unsigned int val;int temp;
    char prefix[100] = "uC1 - Temperature: ";
    int i, j;
    
    IODIR0 = (1 << 4)|(1<<15);
    UART0_Init();
    ADC0_Init(); 
    
    while (1) 
		{
      //Data Acquisiztion and Conversion
			
			val = ADC0_Read();
			temp = (val * 3.3 / 1023) * 100;  
			itstr(temp, temp_str);
			
			//Preparing String to be Transmitted
			i = j = 0;
			while (prefix[j] != '\0') {
					buffer[i++] = prefix[j++];
			}
			j = 0;
			while (temp_str[j] != '\0') {
					buffer[i++] = temp_str[j++];
			}
			buffer[i++] = '*';
			buffer[i++] = 'C';
			buffer[i++] = '\r'; 
			buffer[i++] = '\n';  
			buffer[i] = '\0';
			
			//Transmitting the String
			UART0_SendString(buffer);  
			
			UART0_ReceiveString(buff);
			UART0_SendString(buff);
			UART0_SendChar('\n');
        
			//Checking for System in ideal or in Warning Mode
			if (temp > 45  || (IOPIN0>>3 & 0x01)) 
			{
				delay(20);
				UART0_SendString("Warning\n");
				delay(20);
				IOSET0 = (1 << 4)|(1<<15);
      }
			else 
				IOCLR0 = (1 << 4)|(1<<15);
			
			//Small Delay for smoother Workflow
      delay(100);
    }
}