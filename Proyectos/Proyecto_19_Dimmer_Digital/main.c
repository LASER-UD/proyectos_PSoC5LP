/* =======================================================================
 * Proyecto 19:
 * Programa que muestra el algoritmo que controla un dimmer digital 
 * haciendo uso del PWM del PSoC5LP. El control del dimmer se realiza con
 * dos pulsadores conectados a las entradas del PSoC, con esto se tiene un
 * valor de voltaje promedio que se ve reflejado con la intensidad del
 * brillo de un LED conectado a una salida del PSoC.
 * =======================================================================
*/

#include <project.h>

volatile uint8 cont;

CY_ISR(Int_Pulsadores) //Rutina de int. para determinar pulsador oprimido
{
    switch (Pulsadores_Read()){ //Lee estado de pulsadores 

        case 1:
            if (cont!=255) cont=cont+5; //Incrementa variable de conteo
            while (PWM_STATUS_TC==0);
		    PWM_WriteCompare(cont); //Actualiza ciclo útil
        break;
        case 2: 
            if (cont!=0) cont=cont-5; //Decrementa variable de conteo 
            while (PWM_STATUS_TC==0);
		    PWM_WriteCompare(cont); //Actualiza ciclo útil
        break;
        default:
        break;
    }
    CyDelay(100);
    Pulsadores_ClearInterrupt(); //Borra solicitudes de interrupción
}

int main()
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    ISR_Pulsadores_StartEx(Int_Pulsadores);
    PWM_Start();
    cont=125; //Valor inicial del PWM
    for(;;)
    {
    }
}

/* [] END OF FILE */

