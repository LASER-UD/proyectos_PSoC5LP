/* =======================================================================
 * Proyecto 12:
 * Programa que muestra el algoritmo para implementar un medidor de
 * distancia con el PSoC5LP haciendo uso de un sensor de ultrasonido
 * comercial. La distancia medida se visualiza en una LCD, el sistema
 * realiza las medidas en unidades de centímetro (la resolución del 
 * sistema de medición es 1cm), en el rango de 5cm a 3m, los datos en la
 * LCD se actualizan una vez por segundo.
 * =======================================================================
*/

#include "project.h"

uint8 i;
uint16 tiempo;
uint32 tiempot;

int main(void)
{
    LCD_Start();
    Timer_Start(); //Inicaliza Timer
    LCD_PrintString("Distancia:"); //Mensaje inicial
    LCD_Position(1,14);
    LCD_PrintString("cm");
    tiempot=0; //Inicializa variable de tiempo
    
    for(;;)
    {
        for(i=0;i<10;i++){ //Toma 10 muestras cada 50ms
            Gen_Trig_Write(1); //Genera pulso de inicio
            CyDelayUs(10); //Ancho del pulso de 10us
            Gen_Trig_Write(0); 
            while(Echo_Read()==0); //Espera respuesta del sensor
            while(Echo_Read()==1); //Espera hasta que se haya generado todo
                                   //el pulso de respuesta
            tiempo=65535-Timer_ReadCounter(); //Calcula tiempo medido
            tiempot=tiempot+tiempo; //Acumula las muestras tomadas
            CyDelay(50); //Espera entre muestras
        }
        tiempot=tiempot/58; //Calcula la distancia con un decimal
        LCD_Position(1,9);
        LCD_PrintString("     "); //Borra dato anterior
        if (tiempot<100){ //Visualiza de acuerdo al rango del valor medido
            LCD_Position(1,12);
            LCD_PrintNumber((uint16)tiempot);
            LCD_Position(1,11);
            LCD_PrintNumber((uint16)tiempot);
            LCD_Position(1,12);
            LCD_PutChar('.'); //Visualiza punto decimal
        }else{
            if (tiempot<1000){
                LCD_Position(1,11);
                LCD_PrintNumber((uint16)tiempot);
                LCD_Position(1,10);
                LCD_PrintNumber((uint16)tiempot);
                LCD_Position(1,12);
                LCD_PutChar('.');//Visualiza punto decimal
            }else{
                LCD_Position(1,10);
                LCD_PrintNumber((uint16)tiempot);
                LCD_Position(1,9);
                LCD_PrintNumber((uint16)tiempot);
                LCD_Position(1,12);
                LCD_PutChar('.');//Visualiza punto decimal
            }
        }
        tiempot=0; //Reinicia variable que acumula muestras
        CyDelay(500); //Espera 500ms para reiniciar proceso
    }
}

/* [] END OF FILE */
