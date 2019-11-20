/* ==================================================================
 * Proyecto 1:
 * Programa para hacer oscilar un LED conectado a uno de los pines del
 * PSoC5LP, el tiempo de oscilación del LED dependerá del último 
 * pulsador oprimido entre cinco posibles, así: 
 *      Pulsador uno: tiempo 25 ms
 *      Pulsador dos: tiempo 50ms
 *      Pulsador tres: tiempo 100ms
 *      Pulsador cuatro: tiempo 200ms (tiempo por defecto)
 *   	Pulsador cinco: tiempo 500ms 
 * El proceso de temporización se realiza por software.
 * ==================================================================
*/
#include <project.h>

/* Definición de variables */
uint16 tiempo;
uint8 estado;

int main()
{
tiempo=200; //Tiempo inicial de 200ms
    for(;;)
    {
        estado=Pulsadores_Read(); //Lee estado de los pulsadores
        if (estado==0x1F){   //Si no se oprime ninguno se procede a generar
            CyDelay(tiempo); //el tiempo y a cambiar el estado del pin
            LED_Write(~LED_Read());
        }else{ //Si se detecta un pulsador oprimido se verifica cual fue
            switch (estado){
                case 0x1E: //Si pulsador oprimido es el uno
                    tiempo=25; //Se cambia tiempo a 25ms
                break;
                case 0x1D: //Si pulsador oprimido es el dos
                    tiempo=50; //Se cambia tiempo a 50ms
                break;
                case 0x1B: //Si pulsador oprimido es el tres
                    tiempo=100; //Se cambia tiempo a 100ms
                break;
                case 0x17: //Si pulsador oprimido es el cuatro
                    tiempo=200; //Se cambia tiempo a 200ms
                break;
                case 0x0F: //Si pulsador oprimido es el quinto
                    tiempo=500; //Se cambia tiempo a 500ms
                break;
                default:
                break;
            }
        }
        
    }
}

/* [] END OF FILE */

