/* =======================================================================
 * Proyecto 23:
 * Programa que muestra el algoritmo que implementa un termómetro digital
 * de 4 dígitos BCD (dos dígitos enteros y 2 dígitos decimales) 
 * transmitiendo los datos por UART a un terminal de datos en Windows, la
 * temperatura medida varía entre 0 y 99.99°C (medida con un sensor de
 * temperatura de referencia LM35), los datos se actualizan una vez por
 * segundo.
* =======================================================================
*/

#include "project.h"

uint8 i;
uint16 volt;
uint32 temp;

int main(void)
{
    LCD_Start();
    VDAC8_Start();
    ADC_Start();
    UART_Start();
    LCD_PrintString("Temperatura:"); //Visualiza menaje en LCD
    UART_PutString("\r\nTemperatura:\r\n"); //Visualiza mensaje en PC
    for(;;)
    {
        temp=0;
        for(i=0;i<100;i++){ //Toma 100 muestras para eliminar ruido
            ADC_StartConvert(); //Inicia conversión
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
            volt=ADC_GetResult16(); //Lee resultado de conversión
            temp=temp+volt;
        }
        temp=(temp*2442)/100000; //Ajusta valor convertido a rango de
        LCD_Position(1,8);       //temperatura requerido   
        LCD_PrintString("      ");
        LCD_Position(1,8);
        volt=temp/1000; //Convierte a decimal dato de temperatura realizando
        temp=temp%1000; //divisiones sucesivas por múltiplos de 10
        UART_PutChar(volt+0x30); //Luego envía cada digito por UART al PC
        LCD_PutChar(volt+0x30); //Tambén lo envía a la LCD
        volt=temp/100;
        temp=temp%100;
        UART_PutChar(volt+0x30);
        LCD_PutChar(volt+0x30);
        UART_PutChar('.'); //Punto decimal para separar enteros de decimales
        LCD_PutChar('.');
        volt=temp/10;
        temp=temp%10;
        UART_PutChar(volt+0x30);
        LCD_PutChar(volt+0x30);
        UART_PutChar((uint8)temp+0x30);
        LCD_PutChar((uint8)temp+0x30);        
        UART_PutChar(0xA7); //Carácter ASCII en PC para los grados
        LCD_PutChar(0xDF); //Carácter ASCII en LCD para los grados
        UART_PutChar('C');
        LCD_PutChar('C');        
        UART_PutChar(0x0D); //Retorno de carro para actualizar dato en PC
        CyDelay(1000); //Repite proceso una vez por segundo
    }
}

/* [] END OF FILE */
