/* =======================================================================
 * Proyecto 4:
 * Programa que controla con el PSoC5LP un sistema de turnos (“turnero”)
 * para dos puestos de atención, cada puesto tendrá un pulsador para 
 * realizar el llamado, cuando se realice dicho llamado se deberá generar
 * una señal sonora (puede ser generada por un buzzer) con una duración de
 * cinco (5) segundos. Se debe visualizar en tres (3) displays de 7
 * segmentos el número del turno y en un display adicional el número del
 * puesto de atención que hace el llamado (1 o 2), en los displays se debe
 * estar alternando cada 3 segundos el último turno asignado por cada
 * puesto de atención. Los turnos asignados son secuenciales y cíclicos
 * desde 000 hasta 999.
* ========================================================================
*/

#include "project.h"

uint8 i;
volatile uint16 cont1,cont2;

CY_ISR(Int_Puesto1){ //Interrupción encargada de actalizar contador 1
    cont1++; //Incrementa contador
    if (cont1==999) {cont1=0;} //Ajusta contador al llegar a 999
    Display_Write7SegNumberDec(cont1,1,3,Display_ZERO_PAD);
    Display_Write7SegNumberDec(1,0,1,Display_ZERO_PAD);
    Display_PutDecimalPoint(1,3);
    for (i=0;i<12;i++){ //Genera señal auditiva por cerca de 5 segundos
        Buzzer_Write(1);
        CyDelay(210);
        Buzzer_Write(0);
        CyDelay(210);
    }
    while(Puesto1_Read()!=1); //Espera hasta que se suelte el pulsador
    Puesto1_ClearInterrupt(); //Borra interrupción generada por rebote
}

CY_ISR(Int_Puesto2){ //Interrupción encargada actualizar contador 2 
    cont2++; //Incrementa contador
    if (cont2==999) {cont2=0;} //Ajusta contador al llegar a 999
    Display_Write7SegNumberDec(cont2,1,3,Display_ZERO_PAD);
    Display_Write7SegNumberDec(2,0,1,Display_ZERO_PAD);
    Display_PutDecimalPoint(1,3);
    for (i=0;i<12;i++){ //Genera señal auditiva por cerca de 5 segundos
        Buzzer_Write(1);
        CyDelay(210);
        Buzzer_Write(0);
        CyDelay(210);
    }
    while(Puesto2_Read()!=1); //Espera hasta que se suelte el pulsador
    Puesto2_ClearInterrupt(); //Borra interrupción generada por rebote
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    Display_Start(); //Carga la configuración seccionada del bloque Display
    ISR_SW1_StartEx(Int_Puesto1); //Habilita int. del Pulsador1
    ISR_SW2_StartEx(Int_Puesto2); //Habilita int. del Pulsador2
    cont1=0; //Inicializa variables
    cont2=0;    
     
    for(;;)
    {
        Display_Write7SegNumberDec(cont1,1,3,Display_ZERO_PAD);
        Display_Write7SegNumberDec(1,0,1,Display_ZERO_PAD);
        Display_PutDecimalPoint(1,3);
        CyDelay(3000);
        Display_Write7SegNumberDec(cont2,1,3,Display_ZERO_PAD);
        Display_Write7SegNumberDec(2,0,1,Display_ZERO_PAD);
        Display_PutDecimalPoint(1,3);
        CyDelay(3000);
    }
}

/* [] END OF FILE */
