/* =======================================================================
 * Proyecto 2:
 * Programa para controlar un Semáforo multifunción con tres LEDs y dos
 * pulsadores N.A. para las funciones que se denominarán: “Temporización”
 * y “Auto/Manual” el modo de funcionamiento que debe tener el semáforo: 
 *      Cuando se presione el botón de reset del PSoC5LP se reiniciará a
 *      su estado inicial de funcionamiento automático y ejecutando la
 *      secuencia 1 del Semáforo.
 *      En el modo automático el Semáforo funcionará como su nombre lo
 *      indica, de forma automática, con cierta duración de encendido en
 *      cada color del Semáforo, en este caso, dos secuencias de tiempo
 *      distintas, para cambiar de una a otra se deberá presionar el 
 *      pulsador  de “Temporización”. 
 *      Si se presiona el pulsador “Auto/Manual” mientras se está en el
 *      modo automático el Semáforo pasara al modo manual en el que cada
 *      luz (verde, amarillo y rojo) permanecerá encendida por tiempo
 *      indefinido y cambiara hasta que se presione el botón de temporización,
 *      pasando de la verde a la amarilla (con parpadeo), de la amarilla
 *      a la roja, de la roja a la amarillo y rojo y finalmente regresará
 *      de la amarilla y roja a la verde.
 *      Si se oprime de nuevo el botón “Auto/Manual” el semáforo pasará
 *      del modo manual al modo automático ejecutando la secuencia 1.
 * El proceso de temporización se realiza por software.
 * =======================================================================
*/

#include "project.h"

/* Definición de variables */
uint8 i,secuencia,auto_man;

int main(void)
{
secuencia=0; //Secuencia 1 por defecto
auto_man=0; //Modo automático por defecto
    for(;;)
    {      
        switch(Pulsadores_Read()){ //Verifica el estado de los pulsadores
            case 0b10:
                secuencia=~secuencia; //Cambia de secuencia
            break;
            case 0b01:
                auto_man=~auto_man; //Cambia de modo            
            break;
            default:
            break;
        }
        if (auto_man==0){ //Si está en modo automático escoge secuencia
            if (secuencia==0){ //Ejecuta secuencia 1
                LED_Write(0b100); //LED verde 6 segundos encendido
                CyDelay(6000);
                for (i=0;i<5;i++){ //LED amarillo parpadea 5 veces
                    LED_Write(0b010);
                    CyDelay(500);
                    LED_Write(0);
                    CyDelay(500);
                }
                LED_Write(0b001); //LED rojo 5 segundos encendido
                CyDelay(5000);
                LED_Write(0b011); //LEDs rojo y amarillo 2 segundos encendidos
                CyDelay(2000);
            }else{ //Ejecuta secuencia 2
                LED_Write(0b100); //LED verde 4 segundos encendido
                CyDelay(4000);
                for (i=0;i<4;i++){ //LED amarillo parpadea 4 veces
                    LED_Write(0b010);
                    CyDelay(500);
                    LED_Write(0);
                    CyDelay(500);
                }
                LED_Write(0b001); //LED rojo 6 segundos encendido
                CyDelay(6000);
                LED_Write(0b011); //LEDs rojo y amarillo 3 segundos encendidos
                CyDelay(3000);    
            }
        }else{ //Entra a modo manual 
            while (auto_man!=0){ //Cambia de modo hasta que se oprima pulsador                
                LED_Write(0b100); //LED verde encendido
                CyDelay(1000);
                do{
                    secuencia=Pulsadores_Read();
                    if (secuencia==0b01){
                        auto_man=0;
                        secuencia=0;
                    }
                }while (secuencia==0b11);
                CyDelay(500);
                while (Pulsadores_Read()==0b11){ //LED amarillo parpadea
                    LED_Write(0b010);
                    CyDelay(500);
                    LED_Write(0);
                    CyDelay(500);
                }
                CyDelay(500);
                LED_Write(0b001); //LED rojo encendido
                do{
                    secuencia=Pulsadores_Read();
                    if (secuencia==0b01){
                        auto_man=0;
                        secuencia=0;
                    }
                }while (secuencia==0b11); 
                CyDelay(500);
                LED_Write(0b011); //LEDs rojo y amarillo encendidos
                do{
                    secuencia=Pulsadores_Read();
                    if (secuencia==0b01){
                        auto_man=0;
                        secuencia=0;
                    }
                }while (secuencia==0b11);
                CyDelay(500);
            }
        }    
    }
}

/* [] END OF FILE */
