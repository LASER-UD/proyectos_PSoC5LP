/* =======================================================================
 * Proyecto 34:
 * Programa que muestra el algoritmo que permite controlar por medio de un
 * teléfono inteligente un LED RGB, la comunicación entre el teléfono y el
 * PSoC5LP se realiza por medio del protocolo inalámbrico Bluetooth. En el
 * teléfono se utilizar una aplicación libre que permita enviar en formato
 * R,G,B el color que se desea asignar al LED, teniendo en cuenta que para
 * cada caso el valor asignado puede variar entre 0 y 255. Además en una 
 * LCD alfanumérica 2x16 se visualizará el valor asignado a cada LED del
 * módulo RGB.
* ========================================================================
*/

#include "project.h"

CY_ISR(INT_Rx){
Timer_Start(); //Habilita el Timer al recibir el primer carácter
}

CY_ISR(INT_Timer){ //Cuando pasan los 25ms verifica la información almacenada
                   //en el buffer del bloque UART
uint8 size,dato,dato1,dato2,R,G,B;   
    
    Timer_Stop();
    size=UART_GetRxBufferSize(); //Lee la cantidad de datos recibidos
    if(size>=7&&size<=13){ //Si se recibieron entre 7 y 13 datos continua
        dato=UART_GetChar();
        dato=dato&0x0F;
        dato1=UART_GetChar();
        if(dato1!=','){ //Verifica si es digito o es el carácter separador
            dato1=dato1&0x0F;
            dato2=UART_GetChar();
            if(dato2!=','){ //Tres dígitos
                dato2=dato2&0x0F; 
                dato=dato*100;       
                dato1=dato1*10;
                R=dato+dato1+dato2; //Concatena y convierte el valor a Hx
                dato=UART_GetChar();//Lee la coma
            }else{ //Dos dígitos
                dato=dato*10;
                R=dato+dato1;    
            }
        }else{ //Un digito
            R=dato;
        }
        dato=UART_GetChar();
        dato=dato&0x0F;
        dato1=UART_GetChar();
        if(dato1!=','){ //Verifica si es digito o es el carácter separador
            dato1=dato1&0x0F;
            dato2=UART_GetChar();
            if(dato2!=','){ //Tres dígitos
                dato2=dato2&0x0F;
                dato=dato*100;       
                dato1=dato1*10;
                G=dato+dato1+dato2; //Concatena y convierte el valor a Hx
                dato=UART_GetChar();//Lee la coma
            }else{ //Dos dígitos
                dato=dato*10;
                G=dato+dato1;    
            }
        }else{ //Un digito
            G=dato;
        }        
        dato=UART_GetChar();
        dato=dato&0x0F;
        dato1=UART_GetChar();
        if(dato1!=0x0D){
            dato1=dato1&0x0F;
            dato2=UART_GetChar();
            if(dato2!=0x0D){ //Tres dígitos
                dato2=dato2&0x0F;
                dato=dato*100;       
                dato1=dato1*10;
                B=dato+dato1+dato2; //Concatena y convierte el valor a Hx
            }else{ //Dos dígitos
                dato=dato*10;
                B=dato+dato1;    
            }
        }else{ //Un digito
            B=dato;
        }
        PWM_Red_WriteCompare(R); //Actualiza PWM del LED Rojo
        PWM_Green_WriteCompare(G); //Actualiza PWM del LED Verde
        PWM_Blue_WriteCompare(B); //Actualiza PWM del LED Azul
        LCD_ClearDisplay();
        LCD_PrintDecUint16(R);
        LCD_PutChar(' ');
        LCD_PrintDecUint16(G);
        LCD_PutChar(' ');
        LCD_PrintDecUint16(B);
        LCD_PrintString(" (RGB)");        
    }    
    UART_ClearRxBuffer();
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start();
    PWM_Red_Start();
    PWM_Green_Start();
    PWM_Blue_Start();
    UART_Start();
    ISR_Rx_StartEx(INT_Rx);
    ISR_Timer_StartEx(INT_Timer);

    for(;;)
    {
    }
}

/* [] END OF FILE */
