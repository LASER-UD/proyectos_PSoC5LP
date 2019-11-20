/* =======================================================================
 * Proyecto 5:
 * Programa que permite hacer uso de una LCD de 2x16 caracteres para
 * visualizar cuatro (4) mensajes diferentes en la LCD, los mensajes están
 * almacenados en la memoria del PSoC5LP y cambian generando cuatro (4)
 * efectos: mensaje estático, mensaje parpadea, mensaje se desplaza a la
 * izquierda, cursor habilitado.
 * =======================================================================
*/

#include "project.h"

uint8 i;

int main(void)
{
    LCD_Start(); //Inicializa LCD
    
    for(;;)
    {
        LCD_ClearDisplay(); //Borra la LCD
        LCD_Position(0,1); //Ubica el cursor en la fila 0 columna 1
    	LCD_PrintString("Proyecto No. 5"); //Escribe mensaje en esa posición
    	LCD_Position(1,5); //Ubica el cursor en la fila 1 columna 5
        LCD_PrintString("PSoC5LP"); //Escribe mensaje en esa posición
        CyDelay(3000); //Mantiene el mensaje 3 segundos
        
        LCD_ClearDisplay(); //Borra LCD y ubica cursor en fila 0 columna 0
        LCD_PrintString("Mensaje Parpadea");
        LCD_Position(1,4); //Ubica el cursor en la fila 1 columna 4
        LCD_PrintString("5 veces");
        for (i=0;i<5;i++){ //Repite proceso de encendido y apagado 5 veces
    		CyDelay(500); //Tiempo de parpadeo de 500ms.
    		LCD_DisplayOff(); //Apaga el display
    		CyDelay(500);
    		LCD_DisplayOn(); //Enciende el display
        }
        CyDelay(3000); //Mantiene el mensaje 3 segundos
        
        LCD_ClearDisplay(); //Borra LCD y ubica cursor en fila 0 columna 0
        CyDelay(1000);
        LCD_PrintString("Mensaje se desplaza ");
        LCD_PutChar(0x7F); //Carácter ASCII de flecha a la izquierda
        for (i=0;i<23;i++){ //Desplaza mensaje a la izquierda 23 veces
            CyDelay(400); //Tiempo de desplazamiento de 400ms.
            LCD_WriteControl(LCD_DISPLAY_SCRL_LEFT); //Comando para desplazar
        }                                     //mensaje a la izquierda una vez
        
        LCD_ClearDisplay();
        LCD_WriteControl(LCD_DISPLAY_CURSOR_ON); //Enciende cursor
        LCD_Position(0,1); //Ubica el cursor en la fila 0 columna 1
    	LCD_PrintString("Cursor visible");
    	LCD_Position(1,2); //Ubica el cursor en la fila 1 columna 2
        LCD_PrintString("al escribir");
        CyDelay(3000); //Mantiene mensaje 3 s. con cursor encendido
        LCD_WriteControl(LCD_CURSOR_BLINK); //Cursor parpadea
        CyDelay(3000); //Mantiene mensaje 3 s. con cursor parpadeando
        LCD_WriteControl(LCD_DISPLAY_ON_CURSOR_OFF); //Apaga cursor
    }
}

/* [] END OF FILE */
