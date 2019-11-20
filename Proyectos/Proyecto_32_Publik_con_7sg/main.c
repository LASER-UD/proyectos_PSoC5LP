/* =======================================================================
 * Proyecto 32:
 * Programa que muestra el algoritmo que permite controlar un publik que
 * hace uso de un módulo MAX7219 que controla ocho displays de 7 segmentos.
 * El mensaje a visualizar se almacena en la memoria del PSoC y se 
 * visualiza constantemente realizando desplazamiento de derecha a 
 * izquierda.
* ========================================================================
*/

#include "project.h"

uint8 i,j,k;
uint16 dato;

#define modo 0x0900
#define intensidad 0x0A00
#define escaneo 0x0B00
#define shutdown 0x0C00
#define test 0x0F00

const uint8 Tabla_7sg[]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B,
    0,0,0,0,0,0,0,
    0x77,0x1F,0x0D,0x3D,0x4F,0x47,0x5E,0x17,0x04,0x3C,
    0x37,0x0E,0x55,0x15,0x1D,0x67,0x73,0x05,0x5B,0x0F,
    0x1C,0x1C,0x5C,0x37,0x3B,0x6D
};

//Mensaje a visualizar
uint8 Tabla_Msg[]={"        PROYECTO NO.32 CON PSOC5LP        "}; 

void Ini_Displays(void){ //Comandos de configuración del controlador
uint16 i;    
    SPIM_WriteTxData(escaneo+0x07); //Habilita los 8 displays a controlar
    CyDelayUs(50);
    SPIM_WriteTxData(modo+0x00); //Registro de modo de decodificación
    CyDelayUs(50);               //se selecciona no decodificar
    SPIM_WriteTxData(test+0x00); //No realizar test a los displays
    CyDelayUs(50);
    SPIM_WriteTxData(shutdown+0x01); //Modo de operación normal (no shutdown)
    CyDelayUs(50);
    SPIM_WriteTxData(intensidad+0x04); //Brillo de los displays medio-bajo (00-0F)
    CyDelayUs(50);    
    for (i=0x0100;i<0x0900;i=i+0x0100){
        SPIM_WriteTxData(i); //Limpiar todos los displays
        CyDelayUs(50);
    }   
}

int main(void)
{
    SPIM_Start();
    Ini_Displays();
    
    for(;;)
    {                        
        for (j=0;j<(sizeof(Tabla_Msg)-7);j++){ //Lee tabla con mensaje
            k=8;
            for(i=0;i<8;i++){ //Lee los 8 datos de cada display
                dato=((k*0x100)+ Tabla_7sg[Tabla_Msg[j+i]]);
                SPIM_WriteTxData(dato);
                k--;
                CyDelay(25); //Tiempo de desplazamiento del mensaje
            }            
        }      
    }
}

/* [] END OF FILE */
