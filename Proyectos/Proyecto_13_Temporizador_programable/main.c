/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

uint8 bt1=0,bt2=0,bt3=0,bt4=0,i;
uint16 tabla_tiempo[]={30,60,120,300,600,900,1200,1800,2700,3600,5400
};

CY_ISR(INT_KB){
uint8 tecla,columna;
    tecla=Filas_Read(); //Lee valor de filas   
    columna=Columnas_Read(); //Lee valor de contador de las columnas
    tecla=tecla<<4;
    tecla=tecla|columna; //Concatena los dos datos obtenidos
    if(bt1==0&&bt2==0&&bt3==0&&bt4==0){
        switch(tecla){ //Determina la tecla oprimida
            case 0xE7: //Tecla A
                bt1=~bt1;
                LCD_ClearDisplay();
                LCD_PrintString("Selec. tiempo de");
                LCD_Position(1,1);
                LCD_PrintString("Temporizador A");
            break; 
            case 0xD7: //Tecla B
                bt2=~bt2;
                LCD_PutChar('B');    
            break;
            case 0xB7: //Tecla C
                bt3=~bt3;
                LCD_PutChar('C');    
            break;
            case 0x77: //Tecla D
                bt4=~bt4;
                LCD_PutChar('D');    
            break;
            default:
            break;    
        }
    }else{
        switch(tecla){ //Determina la tecla oprimida
            case 0xEE:
                LCD_PutChar('1');    
            break;
            case 0xED:
                LCD_PutChar('2');    
            break;
            case 0xEB:
                LCD_PutChar('3');    
            break;
       
            case 0xDE:
                LCD_PutChar('4');    
            break;
            case 0xDD:
                LCD_PutChar('5');    
            break;
            case 0xDB:
                LCD_PutChar('6');    
            break;

            case 0xBE:
                LCD_PutChar('7');    
            break;
            case 0xBD:
                LCD_PutChar('8');    
            break;
            case 0xBB:
                LCD_PutChar('9');    
            break;
            
            case 0x7E:
                LCD_PutChar('*');    
            break;
            case 0x7D:
                LCD_PutChar('0');    
            break;
            case 0x7B:
                LCD_PutChar('#');    
            break;
        }
    }
    while(Filas_Read()!=0x0F);
    CyDelay(50);
    Filas_ClearInterrupt();
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start(); //Inicializa LCD
    ISR_KB_StartEx(INT_KB);
    LCD_PrintString("Tempo. a config.");
    LCD_Position(1,3);
    LCD_PrintString("A  B  C  D");
    for (i=0;i<5;i++){
        CyDelay(500);
        LCD_DisplayOff();
        CyDelay(500);
        LCD_DisplayOn();
    }
    
    for(;;)
    {
        Columnas_Write(0b1110); //Rota un 0 lógico en las columnas del
        CyDelayUs(500);         //teclado cada 500us generando el   
        Columnas_Write(0b1101); //barrido correspondiente
        CyDelayUs(500);
        Columnas_Write(0b1011);
        CyDelayUs(500);
        Columnas_Write(0b0111);
        CyDelayUs(500);
    }
}

/* [] END OF FILE */
