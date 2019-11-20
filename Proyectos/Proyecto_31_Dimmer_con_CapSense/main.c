/* =======================================================================
 * Proyecto 31:
 * Programa que muestra el algoritmo que permite controlar un dimmer
 * digital haciendo uso del PWM del PSoC5LP, el control del dimmer se
 * realiza haciendo uso del CapSense con el Widget tipo Linear Slider del
 * PSoC, con esto se tiene un valor de voltaje promedio que se ve reflejado
 * en la intensidad del brillo de un LED conectado a una salida del PSoC,
 * además de dos Widgets tipo Button del CapSense que sirven para encender
 * y apagar completamente el LED.
* ========================================================================
*/

#include "project.h"

uint16 PosCur;

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    PWM_Start();
    CapSense_Start();
    CapSense_InitializeAllBaselines(); //Inicializa todos los sensores

    for(;;)
    {
        CapSense_UpdateEnabledBaselines(); //Actualiza todos los sensores
        CapSense_ScanEnabledWidgets(); //Escanea todos los sensores habilitados
        while(CapSense_IsBusy()!=0); //Espera hasta que se complete el escaneo
        //Verifica si se oprimió el botón 0 y enciende el LED si así es
        if (CapSense_CheckIsWidgetActive(CapSense_BUTTON0__BTN)){ 
            PWM_WriteCompare(255);
        }
        //Verifica si se oprimió el botón 1 y apaga el LED si así es
        if (CapSense_CheckIsWidgetActive(CapSense_BUTTON1__BTN)){
            PWM_WriteCompare(0);
        }
        //Verifica si se tocó el Linear Slider
        PosCur=CapSense_GetCentroidPos(CapSense_LINEARSLIDER0__LS);
        //Si así fue actualiza el valor del PWM
        if (PosCur!=0xFFFF) PWM_WriteCompare(PosCur<<1);        
        CapSense_ClearSensors(); //Borra sensores del CapSense
        CyDelay(100);
    }
}

/* [] END OF FILE */
