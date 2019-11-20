/* =======================================================================
 * Proyecto 35:
 * Programa que muestra el algoritmo que permite controlar el 
 * funcionamiento de un motor DC por medio de una señal seno de 5vpp. El
 * motor debe estar encendido para frecuencias entre 1KHz y 10KHz, para
 * cualquier otra frecuencia debe estar apagado. La implementación se
 * desarrolló con los filtros digitales incluidos en el PSoC5LP.
* ========================================================================
*/

#include "project.h"

    /* Defines for DMA */
#define DMA_BYTES_PER_BURST 1
#define DMA_REQUEST_PER_BURST 1
#define DMA_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA */
/* Move these variable declarations to the top of the function */
uint8 DMA_Chan;
uint8 DMA_TD[1];

CY_ISR(Int_Filtro){
    VDAC8_SetValue(Filtro_PB_Read8(Filtro_PB_CHANNEL_A)+128);
}

int main(void)
{
    /* DMA Configuration for DMA */
    DMA_Chan = DMA_DmaInitialize(DMA_BYTES_PER_BURST, DMA_REQUEST_PER_BURST, 
        HI16(DMA_SRC_BASE), HI16(DMA_DST_BASE));
    DMA_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_TD[0], 1, CY_DMA_DISABLE_TD, 0);
    CyDmaTdSetAddress(DMA_TD[0], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)Filtro_PB_STAGEA_PTR));
    CyDmaChSetInitialTd(DMA_Chan, DMA_TD[0]);
    CyDmaChEnable(DMA_Chan, 1);
   
    CyGlobalIntEnable; //Habilita interrupciones globales
    ADC_Start();
    Filtro_PB_Start();
    VDAC8_Start();
    Comp_Start();
    ISR_Filtro_StartEx(Int_Filtro);

    for(;;)
    {
    }
}

/* [] END OF FILE */
