/* =======================================================================
 * Proyecto 9:
 * Programa que genera algoritmo para controlar un tacómetro digital que
 * realiza la medición de la velocidad en RPMs de un motor cuya velocidad
 * máxima es de 800RPMs, en una LCD alfanumérica de 2x16 caracteres. El
 * dato de velocidad es mostrado como un valor numérico y además, en una
 * barra horizontal que representa el rango correspondiente, estos valores
 * se actualizan en la LCD cada 3 segundos.
 * =======================================================================
*/

#include "project.h"

volatile uint16 valor;

CY_ISR(INT_Sensor){
    valor++; //Incrementa contador de pulsos
    Sensor_ClearInterrupt(); //Limpia interrupciones
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start(); //Inicializa LCD
    ISR_Sensor_StartEx(INT_Sensor); //Inicializa y conf. int. del sensor      
    LCD_PrintString("Valor=   RPM"); //Mensaje inicial
    valor=0; //Inicializa valor del contador

    for(;;)
    {
        CyDelay(3000); //Espera 3 segundos para actualizar conteo en la LCD
        ISR_Sensor_Disable(); //Deshabilita int. del sensor
        valor=valor*3; //Multiplica lectura del contador x 3 para ajustar
                       //de acuerdo a la cantidad de ranuras del disco
                       //acoplado al eje del motor (20 en este caso)
        LCD_Position(0,6); //Ubica cursor en fila 0 columna 6
        LCD_PrintString("   "); //Borra valor actual
        LCD_Position(0,6); //Ubica cursor en fila 0 columna 6
        LCD_PrintNumber(valor); //Convierte y visualiza el dato del contador
        valor=valor/10; //Ajusta escala para representar en barra horizontal
        LCD_DrawHorizontalBG(1,0,16,valor); //Dibuja la barra horizontal 
							              //iniciando en la fila 1 columna 0
        valor=0; //Reincia contador
        ISR_Sensor_Enable(); //Habilita int. del sensor        
    }
}

/* [] END OF FILE */
