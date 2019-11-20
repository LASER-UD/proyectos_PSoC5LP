/* =======================================================================
 * Proyecto 24:
 * Programa que muestra el algoritmo que permite visualizar en una LCD
 * alfanumérica la información generada por una brújula digital de tipo 
 * I2C de referencia CMPS10. La información visualizada corresponde a los
 * siguientes parámetros: compás en formato tanto de 8 como de 16 bits,
 * ángulo pitch y ángulo roll, los datos se actualizan una vez por segundo.
* ========================================================================
*/

#include "project.h"
#include <stdlib.h>

#define SlaveAddress 0x60

uint8 i,Tabla[4];
uint16 result,angulo;
char TablaConv[4];

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    I2C_Start();
    LCD_Start();
    
    for(;;)
    {        
        //Lectura de una posición de memoria (dirección 0x01 del sensor)
        do{
            result=I2C_MasterSendStart(SlaveAddress,I2C_WRITE_XFER_MODE);
        }
        while(result!=I2C_MSTR_NO_ERROR); //Repite hasta que no exista error
        I2C_MasterWriteByte(0x01); //Dirección de memoria a leer
        //Lee dirección 0x01
        I2C_MasterSendRestart(SlaveAddress,I2C_READ_XFER_MODE); 
        result=I2C_MasterReadByte(I2C_NAK_DATA); //Guarda dato leído del sensor
        I2C_MasterSendStop(); //Envía bit de parada al esclavo
        LCD_ClearDisplay();
        LCD_PutChar(LCD_CUSTOM_0); //Visualiza símbolo del ángulo
        result=(result*141)/10; //Ajusta valor leído en formato de 8 bits a
                                //valor angular entre 0 y 359.9°
        itoa(result,TablaConv,10); //Convierte número a cadena de caracteres
        for(i=0;i<sizeof(TablaConv);i++){ //Visualiza los datos en la LCD
            if (TablaConv[i+1]>='0'&&TablaConv[i+1]<='9'){
                LCD_PutChar(TablaConv[i]);
            }else{
                if (i==0) LCD_PutChar('0');
                LCD_PutChar('.');
                LCD_PutChar(TablaConv[i]);
                i=sizeof(TablaConv)-1;
            }
        }   
        LCD_PutChar(0xDF); //Visualiza símbolo de grados
        
        //Lectura de múltiples posiciones (direcciones 0x02 a 0x05 del sensor)
        do{
            result=I2C_MasterSendStart(SlaveAddress,I2C_WRITE_XFER_MODE);
        }
        while(result!=I2C_MSTR_NO_ERROR); //Repite hasta que no exista error
        I2C_MasterWriteByte(0x02); //Dir. de memoria donde se inicia lectura
        I2C_MasterSendRestart(SlaveAddress,I2C_READ_XFER_MODE); 
        for (i=0;i<3;i++){ //Lee y almacena datos de posiciones 0x02 a 0x04
            Tabla[i]=I2C_MasterReadByte(I2C_ACK_DATA);    
        }
        Tabla[3]=I2C_MasterReadByte(I2C_NAK_DATA); //Lee posición 0x05
        I2C_MasterSendStop(); //Envía bit de parada al esclavo
        angulo=(uint16)Tabla[0]; //Concatena parte alta con parte baja de valor 
        angulo=angulo<<8;        //leído en formato de 16 bits
        angulo=angulo|(uint16)Tabla[1]; 
        LCD_Position(1,0);
        LCD_PutChar(LCD_CUSTOM_0); //Visualiza símbolo del ángulo
        itoa(angulo,TablaConv,10); //Convierte número a cadena de caracteres
        for(i=0;i<sizeof(TablaConv);i++){ //Visualiza los datos en la LCD
            if (TablaConv[i+1]>='0'&&TablaConv[i+1]<='9'){
                LCD_PutChar(TablaConv[i]);
            }else{
                if (i==0) LCD_PutChar('0');
                LCD_PutChar('.');
                LCD_PutChar(TablaConv[i]);
                i=sizeof(TablaConv)-1;
            }
        } 
        LCD_PutChar(0xDF); //Visualiza símbolo de grados
        LCD_Position(0,8);
        LCD_PrintString("Pch:"); //Visualiza mensaje para Pitch
        if (Tabla[2]>127){
            LCD_PutChar('-');
            Tabla[2]=~Tabla[2];
        }else{
            LCD_PutChar('+');
        }
        itoa(Tabla[2],TablaConv,10); //Convierte Pitch a cadena de caracteres
        if (TablaConv[0]=='0') LCD_Position(0,12); //Visualiza Pitch
        LCD_PutChar(TablaConv[0]);
        if (TablaConv[1]>='0'&&TablaConv[1]<='9') LCD_PutChar(TablaConv[1]);
        LCD_PutChar(0xDF); //Visualiza simbolo de grados
        LCD_Position(1,8);
        LCD_PrintString("Rll:"); //Visualiza mensaje para Roll
        if (Tabla[3]>127){
            LCD_PutChar('-');
            Tabla[3]=~Tabla[3];
        }else{
            LCD_PutChar('+');
        }
        itoa(Tabla[3],TablaConv,10); //Convierte Roll a cadena de caracteres
        if (TablaConv[0]=='0') LCD_Position(1,12); //Visualiza Roll
        LCD_PutChar(TablaConv[0]);
        if (TablaConv[1]>='0'&&TablaConv[1]<='9') LCD_PutChar(TablaConv[1]);
        LCD_PutChar(0xDF); //Visualiza simbolo de grados 
        CyDelay(1000); //Actualiza datos una vez por segundo
    }
}

/* [] END OF FILE */
