/* =======================================================================
 * Proyecto 32:
 * Programa que muestra el algoritmo que permite realizar operaciones
 * aritméticas básicas (+,-,/,=) entre un valor constante y una entrada de
 * voltaje AC (triangular, cuadrada o seno) que varíe entre 0 y 5V, para
 * esto se debe hacer uso del módulo DMA del PSoC5LP, la función a ejecutar
 * se seleccionará haciendo uso de cuatro pulsadores N.A. La función 
 * seleccionada será visualizada en una LCD hasta que se seleccione una
 * nueva.
* ========================================================================
*/

#include "project.h"

    /* Defines for DMA_ADC */
#define DMA_ADC_BYTES_PER_BURST 2
#define DMA_ADC_REQUEST_PER_BURST 1
#define DMA_ADC_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_ADC_DST_BASE (CYDEV_SRAM_BASE)

    /* Defines for DMA_VDAC */
#define DMA_VDAC_BYTES_PER_BURST 1
#define DMA_VDAC_REQUEST_PER_BURST 1
#define DMA_VDAC_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA_VDAC_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_ADC */
/* Move these variable declarations to the top of the function */
uint8 DMA_ADC_Chan;
uint8 DMA_ADC_TD[1];

/* Variable declarations for DMA_VDAC */
/* Move these variable declarations to the top of the function */
uint8 DMA_VDAC_Chan;
uint8 DMA_VDAC_TD[1];

uint8 Tabla_ADC[1];
uint8 Tabla_VDAC[1];
volatile uint8 tarea;

CY_ISR(Int_DMA){
switch (tarea){ //Lee estado de variable tarea
        case 0: //Señal sin cambio
            Tabla_VDAC[0]=Tabla_ADC[0];
        break;
        case 1: //Sumar la señal
            Tabla_VDAC[0]=Tabla_ADC[0]+32;
        break;
        case 2: //Restar la señal
            Tabla_VDAC[0]=Tabla_ADC[0]-32;   
        break;
        case 3: //Dividir la señal
            Tabla_VDAC[0]=Tabla_ADC[0]>>1;
        break;
        default:
        break;
    }
}

CY_ISR(Int_Funcion){
    switch (Funcion_Read()){ //Lee estado de pulsadores 
        case 0b1110: //Selecciona señal sin cambio
            tarea=0;
            LCD_ClearDisplay();
            LCD_PrintString("Senal sin cambio");
            LCD_Position(0,2);
            LCD_PutChar(0xEE);
        break;
        case 0b1101: //Selecciona sumar la señal
            tarea=1;
            LCD_ClearDisplay();
            LCD_Position(0,2);  
            LCD_PrintString("Sumando a la");
            LCD_Position(1,4);  
            LCD_PrintString("senal...");
            LCD_Position(1,6);
            LCD_PutChar(0xEE);

        break;
        case 0b1011: //Seleccion restar la señal
            tarea=2;
            LCD_ClearDisplay();
            LCD_Position(0,5);  
            LCD_Position(0,2);  
            LCD_PrintString("Restando a la");
            LCD_Position(1,4);  
            LCD_PrintString("senal...");
            LCD_Position(1,6);
            LCD_PutChar(0xEE);
        break;
        case 0b0111: //Dividir la señal
            tarea=3;
            LCD_ClearDisplay();
            LCD_Position(0,1);  
            LCD_PrintString("Dividiendo la");
            LCD_Position(1,4);  
            LCD_PrintString("senal...");
            LCD_Position(1,6);
            LCD_PutChar(0xEE);
        break;
        default:
        break;
    while(Funcion_Read()!=0b1111); //Espera hasta que se suelte el pulsador
    CyDelay(100);
    Funcion_ClearInterrupt(); //Borra solicitudes de interrupción            
    }
}

int main(void)
{
    /* DMA Configuration for DMA_ADC */
    DMA_ADC_Chan = DMA_ADC_DmaInitialize(DMA_ADC_BYTES_PER_BURST, DMA_ADC_REQUEST_PER_BURST, 
        HI16(DMA_ADC_SRC_BASE), HI16(DMA_ADC_DST_BASE));
    DMA_ADC_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_ADC_TD[0], 1, CY_DMA_DISABLE_TD, CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_ADC_TD[0], LO16((uint32)ADC_SAR_WRK0_PTR), LO16((uint32)Tabla_ADC));
    CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);
    CyDmaChEnable(DMA_ADC_Chan, 1);
    
    /* DMA Configuration for DMA_VDAC */
    DMA_VDAC_Chan = DMA_VDAC_DmaInitialize(DMA_VDAC_BYTES_PER_BURST, DMA_VDAC_REQUEST_PER_BURST, 
        HI16(DMA_VDAC_SRC_BASE), HI16(DMA_VDAC_DST_BASE));
    DMA_VDAC_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_VDAC_TD[0], 1, DMA_VDAC_TD[0], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_VDAC_TD[0], LO16((uint32)Tabla_VDAC), LO16((uint32)VDAC8_Data_PTR));
    CyDmaChSetInitialTd(DMA_VDAC_Chan, DMA_VDAC_TD[0]);
    CyDmaChEnable(DMA_VDAC_Chan, 1);

    CyGlobalIntEnable; //Habilita interrupciones globales
    ADC_Start();
    VDAC8_Start();
    LCD_Start();
    ISR_DMA_StartEx(Int_DMA);
    ISR_Funcion_StartEx(Int_Funcion);
    tarea=0;
    LCD_ClearDisplay();
    LCD_PrintString("Senal sin cambio");
    LCD_Position(0,2);
    LCD_PutChar(0xEE);

    for(;;)
    {
    }
}

/* [] END OF FILE */
