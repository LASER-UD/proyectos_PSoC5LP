/* =======================================================================
 * Proyecto 11:
 * Programa que genera el algoritmo para manejar un reloj digital que
 * visualiza en una LCD alfanumérica de 2x16 la hora en formato militar
 * HH:mm:ss y la fecha en formato dd/mm/aa. Todos los parámetros del reloj
 * (hora y fecha) se pueden configurar haciendo uso de cinco pulsadores
 * (dos para la hora y tres para la fecha). La hora inicial al encender el
 * reloj son las 00:00:00 y la fecha 01/01/19, por medio de los pulsadores
 * se ajusta la hora y la fecha a la que el usuario desee.
 * =======================================================================
*/

#include "project.h"

uint8 temp;
volatile uint8 seg,min,hora,dia,mes,anio;
uint8 tabla_dia[]={31,28,30,30,31,30,31,31,30,31,30,31}; //Tabla que
                                //contiene los días finales de cada mes

void Vis_Tiempo(void){ //Rutina de visualización de la hora
    LCD_Position(0,0); //Ubica cursor al inicio
    if (hora<=9) LCD_PutChar('0'); //Si hora <= a 9 visualiza un cero
    LCD_PrintNumber(hora); //Visualiza variable hora
    LCD_PutChar(':');
    if (min<=9) LCD_PutChar('0'); //Si minutos <= a 9 visualiza un cero    
    LCD_PrintNumber(min); //Visualiza variable minutos
    LCD_PutChar(':');
    if (seg<=9) LCD_PutChar('0'); //Si segundos <= a 9 visualiza un cero
    LCD_PrintNumber(seg); //Visualiza variable segundos   
}

void Vis_Fecha(void){ //Rutina de visualización de la fecha
    LCD_Position(1,8); //Ubica cursor en la fila 1 columna 8
    if (dia<=9) LCD_PutChar('0'); //Si dia <= a 9 visualiza un cero
    LCD_PrintNumber(dia); //Visualiza variable dia
    LCD_PutChar('/');
    if (mes<=9) LCD_PutChar('0'); //Si mes <= 9 visualiza un cero
    LCD_PrintNumber(mes); //Visualiza variable mes
    LCD_PutChar('/');
    if (anio<=9) LCD_PutChar('0'); //Si año <= a 9 visualiza un cero
    LCD_PrintNumber(anio); //Visualiza variable del año
}

CY_ISR(INT_Timer){    
    seg++; //Incrementa variable segundos
    if (seg==60){ //Si es igual a 60 ajusta
        seg=0; //Ajusta variable segundos
        min++; //Incrementa variable minutos
        if (min==60){ //Si es igual a 60 ajusta
            min=0; //Ajusta variable minutos
            hora++; //Incrementa variable hora
            if (hora==24){ //Si es igual a 24 ajusta
                hora=0; //Ajusta variable hora
                dia++; //Incrementa variable dia
                if(dia>tabla_dia[mes-1]){ //Si dia > a ultimo dia de mes ajusta
                    if (mes==2&&dia==29){ //Si mes=2&dia=29 verifica si bisiesto
                        temp=anio%4; //Año bisiesto es multiplo de cuatro
                        if (temp!=0){ //Si lo es
                            dia=1; //Ajusta variable dia
                            mes++; //Incrementa variable mes
                            if(mes==13){ //Si es igual a 13 ajusta
                                mes=1; //Ajusta variable mes
                                anio++; //Incrementa variable año             
                            }
                        }
                    }else{ //Si no es año bisiesto
                        dia=1; //Ajusta variable dia
                        mes++; //Incrementa variable mes
                        if(mes==13){
                            mes=1; //Ajusta variable mes
                            anio++; //Incrementa variable año                       
                        }
                    }
                }
                Vis_Fecha(); //Llama rutina de visualización de fecha              
            }
        }
    }
    Vis_Tiempo(); //Llama rutina de visualización de la hora
    ISR_Timer_ClearPending();
}

CY_ISR(INT_Tiempo){ //Rutina de int. de pulsadores para ajuste de tiempo
    switch (Aj_Tiempo_Read()){ //Lee estado de pulsadores 
        case 0b01: 
            hora++; //Incrementa variable de hora
            if (hora==24) hora=0; //Ajusta la hora                            
        break;
        case 0b10:
            min++; //Incrementa variable de minutos
            seg=0; //Reinicia los segundos
            if (min==60) min=0; //Ajusta los minutos
        break;
        default:
        break;
    }
    Vis_Tiempo(); //Llama rutina de visualización de la hora
    CyDelay(100);
    Aj_Tiempo_ClearInterrupt(); //Borra solicitudes de interrupción
}

CY_ISR(INT_Fecha){ //Rutina de int. de pulsadores para ajuste de fecha
    switch (Aj_Fecha_Read()){ //Lee estado de pulsadores 
        case 0b110: 
            anio++; //Incrementa variable del año
            if (anio==100) anio=19; //Ajusta el año
        break;
        case 0b101:
            mes++; //Incrementa variable del mes
            if (mes==13) mes=1; //Ajusta el mes
            if(dia>tabla_dia[mes-1]) dia=1;
        break;
        case 0b011:
            dia++; //Incrementa variable dia            
            if(dia>tabla_dia[mes-1]){
                if (mes==2&&dia==29){
                    temp=anio%4;
                    if (temp!=0) dia=1; //Ajusta el dia
                }else{
                    dia=1;                    
                }
            }
        break;    
        default:
        break;
    }
    Vis_Fecha(); //Llama rutina de visualización de fecha
    CyDelay(100);
    Aj_Fecha_ClearInterrupt(); //Borra solicitudes de interrupción
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start();
    Timer_Start(); //Inicializa Timer
    ISR_Timer_StartEx(INT_Timer);
    ISR_Aj_Tiempo_StartEx(INT_Tiempo);
    ISR_Aj_Fecha_StartEx(INT_Fecha);
    seg=0; //Inicializa variables
    min=0;
    hora=0;
    dia=1;
    mes=1;
    anio=19;
    Vis_Tiempo(); //Llama rutina de visualización de la hora
    Vis_Fecha(); //Llama rutina de visualización de fecha
    for(;;)
    {        
    }
}

/* [] END OF FILE */
