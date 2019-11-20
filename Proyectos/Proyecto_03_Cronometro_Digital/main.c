/* =======================================================================
 * Proyecto 3:
 * Programa que opera un cronometro digital (implementado con un PSoC5LP)
 * que visualice en 4 displays de 7 segmentos la información del tiempo
 * transcurrido. La visualización en los displays de 7 segmentos debe ser
 * dinámica, deberá mostrar dos dígitos para los minutos (hasta 59) y dos
 * dígitos para segundos (hasta 59). Se utilizará un pulsador N.A. que 
 * tendrá como función detener o hacer que continué la temporización, y 
 * un segundo pulsador para tomar tiempo parcial, es decir, en la 
 * visualización se pausará el tiempo pero el cronometro seguirá 
 * temporizando, al oprimirlo nuevamente seguirá la temporización normal.
 * Para reiniciar el cronometro se hará uso de la señal de reset del PSoC,
 * el proceso de temporización se realiza por software.
* =======================================================================
*/

#include "project.h"

#include "project.h"

uint8 seg,min,dp;
volatile uint8 start_stop,lap;

CY_ISR(Int_Pulsador){ //Interrupción encargada de detectar el estado
                      //de los pulsadores y actualizar variables
    switch(Pulsadores_Read()){ //Verifica el estado de los pulsadores
        case 0b10:
            start_stop=~start_stop; //Inicia o detiene el cronómetro
        break;
        case 0b01:
            lap=~lap; //Cambia de modo el cronómetro            
        break;
        default:
        break;
    }
    while(Pulsadores_Read()!=0b11); //Espera hasta que se suelte el pulsador
    Pulsadores_ClearInterrupt(); //Borra interrupciones generadas por rebote
}

int main(void)
{   
    CyGlobalIntEnable; //Habilita interrupciones globales
    Display_Start(); //Carga la configuración seccionada del bloque Display
    ISR_SW_StartEx(Int_Pulsador); //Habilita int. generada por pulsadores    
    seg=0; //Inicializa variables
    min=0;    
    start_stop=0;
    lap=0;
    Display_Write7SegNumberDec(min,0,2,Display_ZERO_PAD); //Visualiza segundos
    Display_Write7SegNumberDec(seg,2,2,Display_ZERO_PAD); //Visualiza minutos
    Display_PutDecimalPoint(1,3); //Enciende punto decimal para separar los
                                  //minutos de los segundos
    for(;;)
    {
        if (start_stop!=0){
            CyDelay(1000); //Base de tiempo de un segundo
            seg++; //Incrementa variable segundos
            if (seg==60){ //Verifica si variable segundos igual a 60
                seg=0; //Si lo es la hace 0 e incrementa la variable minutos
                min++;
                if (min==60){ //Verifica si variable minutos igual a 60
                    min=0; //Si lo es la hace 0
                }
            }
            if (lap==0){ //Verifica si esta en modo normal o en tiempo parcial
                         //Si está en modo normal visualiza el tiempo   
                Display_Write7SegNumberDec(min,2,2,Display_ZERO_PAD);
                Display_Write7SegNumberDec(seg,0,2,Display_ZERO_PAD);
                Display_PutDecimalPoint(1,3);
            }else{ //Si está en modo tiempo parcial detiene la visualización
                   //y hace conmutar el estado del punto decimal del display 
                dp=Display_GetDecimalPoint(3); //Determina estado actual del
							      //punto decimal
                dp=~dp; //Complementa el estado leído
                dp=dp&0x01; //Asegura un valor lógico 0 o 1
                Display_PutDecimalPoint(dp,3); //Envía el valor para encender 								//o apagar el punto
            }
        }
    }
}

/* [] END OF FILE */
