/* =======================================================================
 * Proyecto 22:
 * Programa que muestra el algoritmo que permite enviar hacia un terminal
 * de datos en Windows (por ejemplo HyperTerminal) un mensaje predefinido
 * codificado en ASCII, luego el algoritmo se queda en un ciclo de espera
 * de información enviada desde el terminal de datos, los datos recibidos
 * son visualizados en una LCD alfanumérica, debe tenerse en cuenta que
 * cada vez que se oprime la tecla Enter del teclado del PC en la LCD se
 * cambia la línea en la que se está escribiendo la información, además se
 * limita la escritura de máximo 16 caracteres por línea realizando el
 * cambio automático de línea cuando se llegue el final de la misma.
* ========================================================================
*/

#include <project.h>

uint8 dato,cont,bandera;

int main()
{
    LCD_Start(); //Inicializa LCD
	UART_Start(); //Inicializa UART
	UART_PutString("Mensaje de prueba con PSoC5LP a 57600 bps\r\n");
	LCD_Position(0,0);
	bandera=0; //Variable para determinar línea actual en la LCD
    cont=0; //Variable para contar cantidad de caracteres por línea
	for(;;)
    {
		while ((dato=UART_GetChar())==0); //Espera hasta recibir dato válido
		switch (dato){
        	case 0x0D: //Verifica si se recibio CR
        		bandera=~bandera;
        	break;
        	case 0x0A: //Verifica si se recibio LF
        		if (bandera!=0){
            	LCD_Position(1,0); //Ubica cursor en línea 1 de LCD
        		}else{
            	LCD_Position(0,0); //Ubica cursor en línea 0 de LCD
        		}
        	break;
        	default:
        	    LCD_PutChar(dato); //Escribe en LCD dato recibido
        	    UART_PutChar(dato); //Reenvia al PC dato recibido
                cont++;
                if(cont==16){
                    bandera=~bandera;
                    if (bandera!=0){
                    	LCD_Position(1,0); //Ubica cursor en línea 1 de LCD
            		}else{
                    	LCD_Position(0,0); //Ubica cursor en línea 0 de LCD
            		}
                    cont=0;
                }
        	break;
    	}
    }
}
