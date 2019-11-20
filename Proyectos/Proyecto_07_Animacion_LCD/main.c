/* =======================================================================
 * Proyecto 7:
 * Programa que permite mostrar el desplazamiento de la imagen del logo de
 * un avión en una LCD alfanumérica de 2x16, el desplazamiento se realiza
 * de izquierda a derecha, así se produce en la LCD un efecto de animación
 * rudimentaria. La velocidad de desplazamiento de la imagen se ajusta por
 * medio de cuatro pulsadores N.A., entre cuatro valores posibles: 200ms.,
 * 400ms., 600ms., y 800ms..
 * =======================================================================
*/

#include "project.h"

uint8 i;
volatile uint16 tiempo;

CY_ISR(INT_SW){
    switch (SW_Read()){
        case 0x0E: //Si pulsador oprimido es el uno
            tiempo=200; //Se cambia tiempo a 200ms
        break;
        case 0x0D: //Si pulsador oprimido es el dos
            tiempo=400; //Se cambia tiempo a 4000ms
        break;
        case 0x0B: //Si pulsador oprimido es el tres
            tiempo=600; //Se cambia tiempo a 600ms
        break;
        case 7: //Si pulsador oprimido es el cuatro
            tiempo=800; //Se cambia tiempo a 800ms
        break;
        default:
        break;
    }
    SW_ClearInterrupt();
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start();
    ISR_SW_StartEx(INT_SW);
    tiempo=400; //Valor de tiempo por defecto
    
    for(;;)
    {        
    	LCD_ClearDisplay();
	    LCD_Position(0,36);
	    LCD_PutChar(LCD_CUSTOM_0);//Visualiza carácter 0
	    LCD_PutChar(LCD_CUSTOM_1);//Visualiza carácter 1
	    LCD_PutChar(LCD_CUSTOM_2);//Visualiza carácter 2
	    LCD_PutChar(LCD_CUSTOM_3);//Visualiza carácter 3
        LCD_Position(1,36);
	    LCD_PutChar(LCD_CUSTOM_4);//Visualiza carácter 4
	    LCD_PutChar(LCD_CUSTOM_5);//Visualiza carácter 5
	    LCD_PutChar(LCD_CUSTOM_6);//Visualiza carácter 6
	    LCD_PutChar(LCD_CUSTOM_7);//Visualiza carácter 7
	    for (i=0;i<20;i++){ //Desplaza mensaje a la derecha 20 veces
            CyDelay(tiempo); //Tiempo de desplazamiento de 400ms.
            LCD_WriteControl(LCD_DISPLAY_SCRL_RIGHT); //Comando para desplazar
        }                                                     
    }
}

/* [] END OF FILE */
