/* =======================================================================
 * Proyecto 20:
 * Programa que muestra el algoritmo que realiza el control de velocidad
 * en lazo cerrado de un motor DC. El usuario del sistema tiene la
 * posibilidad de seleccionar por medio de pulsadores N.A. la velocidad
 * deseada entre 4 valores diferentes (250rpms, 500rpms, 750rpms y
 * 1000rpms), el menú de selección es visualizado en una LCD. La velocidad
 * del motor es controlada por medio del PWM del PSoC, para realizar la
 * medición de velocidad se acopló al motor otro que servirá como
 * generador, permitiendo así medir voltaje que será finalmente el 
 * parámetro para la realimentación del sistema de control. Se visualiza
 * en todo momento la velocidad deseada y la velocidad real del motor.
 * =======================================================================
*/

#include "project.h"

uint8 ciclo;
uint16 velo1=0,i;
volatile uint8 volt=0;
volatile uint16 vel=0;

CY_ISR(Int_Pulsadores){ //Rutina de int. para determinar pulsador oprimido
    switch (Pulsadores_Read()){ //Lee estado de pulsadores 
        case 0b1110:
            vel=250; //Selecciona 250rpms
            volt=36; //Equivale a 0,7 voltios
            while (PWM_STATUS_TC==0);
		    PWM_WriteCompare(volt); //Actualiza ciclo útil al 14%
        break;
        case 0b1101:
            vel=500; //Selecciona 500rpms
            volt=77; //Equivale a 1,5 voltios
            while (PWM_STATUS_TC==0);
		    PWM_WriteCompare(volt); //Actualiza ciclo útil al 30%
        break;
        case 0b1011:
            vel=750; //Selecciona 750rpms
            volt=122; //Equivale a 2,4 voltios 
            while (PWM_STATUS_TC==0);
		    PWM_WriteCompare(volt); //Actualiza ciclo útil al 48%
        break;
        case 0b0111:
            vel=1000; //Selecciona 1000rpms
            volt=153; //Equivale a 3 voltios 
            while (PWM_STATUS_TC==0);
		    PWM_WriteCompare(volt); //Actualiza ciclo útil al 60%
        break;
        default:
        break;
    }
    CyDelay(100);
    Pulsadores_ClearInterrupt(); //Borra solicitudes de interrupción
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start();
    ADC_Start();
    PWM_Start();
    ISR_Pulsadores_StartEx(Int_Pulsadores);
    LCD_PrintString(" Sel. velocidad"); //Visualiza mensaje inicial
    LCD_Position(1,0);
    LCD_PrintString("250 500 750 1000");
    for (i=0;i<5;i++){
        CyDelay(500);
        LCD_DisplayOff();
        CyDelay(500);
        LCD_DisplayOn();
    }

    for(;;)
    {
        while(vel==0); //Espera hasta que se haya seleccionado una velocidad
        LCD_ClearDisplay();
        LCD_PrintString("Vel. sel.: "); //Visualiza velocidad deseada
        LCD_PrintNumber(vel);
        LCD_Position(1,0);
        LCD_PrintString("Vel. act.: "); //Visualiza velocidad medida
        LCD_PrintNumber(velo1);        
        for(i=0;i<3000;i++){ //Realiza 3000 ciclos de ajuste de velocidad
            ADC_StartConvert(); //Inicia conversión
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT); //Fin de conversión?
            ciclo=ADC_GetResult8(); //Lee dato del ADC
            if (ciclo>volt) ciclo=ciclo-5; //Si valor medido > a vel. deseada 
                                           //reduce ciclo útil
            if (ciclo<volt) ciclo=ciclo+5; //Si valor medido < a vel. deseada
                                           //aumenta ciclo útil
            while (PWM_STATUS_TC==0);
    	    PWM_WriteCompare(ciclo); //Actualiza ciclo útil
        }
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        ciclo=ADC_GetResult8();
        velo1=((ciclo*63)/10)+12; //Calcula velocidad actual
    }
}

/* [] END OF FILE */