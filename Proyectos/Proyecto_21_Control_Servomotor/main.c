/* =======================================================================
 * Proyecto 21:
 * Programa que muestra el algoritmo que realiza el control de posición
 * para un servomotor, la posición de referencia se establecida por medio
 * de un potenciómetro conectado directamente al PSoC. Se visualiza en
 * una LCD alfanumérica el valor de la posición como una ángulo entre 0 y
 * 180°, representado como un valor numérico y un valor gráfico, para 
 * esto último se hará uso de barras horizontales. 
* =======================================================================
*/

#include "project.h"

uint8 volt;
uint32 angulo;

int main(void)
{
    LCD_Start();
    ADC_Start();
    PWM_Start();
    LCD_PrintString("Angulo:"); //Visualiza mensaje inicial    
    for(;;)
    {
        ADC_StartConvert(); //Inicia conversión
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        volt=ADC_GetResult8(); //Lee resultado de la conversión
        while (PWM_STATUS_TC==0); //Espera final del periodo del PWM
        PWM_WriteCompare(volt); //Actualiza ciclo útil (ángulo)
        angulo=(volt*706)/1000; //Ajusta valor del ángulo para visualizarlo
        LCD_Position(0,7);
        LCD_PrintString("    ");
        LCD_Position(0,7);
        LCD_PrintNumber(angulo); //Visualiza ángulo actual del servomotor
        LCD_PutChar(0xDF); //Carácter ASCII del símbolo de grados
        LCD_DrawHorizontalBG(1,0,16,(volt/3)); //Visualiza valor gráfico
        CyDelay(500); //Repite el proceso cada 500 ms
    }
}

/* [] END OF FILE */
