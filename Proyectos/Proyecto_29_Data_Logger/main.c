/* =======================================================================
 * Proyecto 29:
 * Programa que muestra el algoritmo que permite generar el registro de
 * datos (Data Logger) para almacenamiento de información de temperatura y
 * humedad relativa haciendo uso de un sensor I2C de referencia AM2320, los
 * datos leídos del sensor deberán ser visualizados en una LCD alfanumérica
 * de 16x2 y actualizados una vez por segundo. 
 * Para el manejo de las funciones del Data Logger se tendrán cuatro 
 * pulsadores N.A. que tendrán las siguientes funciones:
 *      Inicio/Parada de la captura de los datos, con la opción inicio se
 *      capturan datos de temperatura y humedad cada cinco minutos, estos
 *      se almacenan en la memoria EEPROM de 2KB del PSoC5LP, por lo que
 *      se podrán almacenar hasta 512 conjuntos de datos, dando un tiempo
 *      de almacenamiento de aproximadamente 42 horas, mientras se esté en
 *      este modo se tendrá un LED indicando que se está en modo de captura
 *      de datos. Con la opción de parada se detiene el proceso y se apaga
 *      el LED mencionado anteriormente. Cuando la memoria se llene se
 *      detiene el proceso y se enciende un LED adicional que indica este
 *      evento.
 *      Dos pulsadores para leer los datos almacenados en la memoria, los
 *      datos se visualizarán en la LCD, uno se utilizará para incrementar
 *      la lectura de la memoria y otro para decrementar esa lectura.
 *      El cuarto pulsador permitirá el borrado de la memoria para cuando
 *      esta esté llena.
* ========================================================================
*/

#include "project.h"
#include <stdlib.h>

#define Sensor_Address 0x05C //Dirección del sensor AM2320

uint8 i,j,start;
volatile uint8 leer,vacio,Tabla[8];
uint16 temperatura,humedad;
char TablaConv[4];

#define n_datos 200

CY_ISR(Int_Funcion){ //Rutina de int. para determinar pulsador oprimido
    switch (Funcion_Read()){ //Lee estado de pulsadores 
        case 0b1110: //Pulsador Inicio/Parada
        start=~start;
        if(start!=0){ //Inicia captura
            Timer_Start();
            CyPins_SetPin(LED_0);
        }else{
            Timer_Stop(); //Detiene captura
            CyPins_ClearPin(LED_0);
        }            
        break;
        case 0b1101: //Pulsador lectura datos incremental
            if(leer<=(n_datos-4)){
                LCD_ClearDisplay();
                LCD_PrintNumber(leer);
                LCD_PutChar(' ');
                LCD_PrintNumber((((uint16)EEPROM_ReadByte(leer)&0x7F)<<8)+EEPROM_ReadByte(leer+1));
                LCD_PutChar(' ');
                LCD_PrintNumber(((uint16)EEPROM_ReadByte(leer+2)<<8)+EEPROM_ReadByte(leer+3));
                if(leer!=(n_datos-4)) leer=leer+4;
            }
        break;
        case 0b1011: //Pulsador lectura datos decremental
            if(leer>=0){
                LCD_ClearDisplay();
                LCD_PrintNumber(leer);
                LCD_PutChar(' ');
                LCD_PrintNumber((((uint16)EEPROM_ReadByte(leer)&0x7F)<<8)+EEPROM_ReadByte(leer+1));
                LCD_PutChar(' ');
                LCD_PrintNumber(((uint16)EEPROM_ReadByte(leer+2)<<8)+EEPROM_ReadByte(leer+3));
                if(leer!=0) leer=leer-4;
            }            
        break;
        case 0b0111: //Pulsador borrado de memoria
            Timer_Stop();
            LCD_ClearDisplay();
            LCD_PrintString("Borrando");
            LCD_Position(1,0);
            LCD_PrintString("memoria...");
            for(i=0;i<63;i++){         //Borra los 64 bloques en que 
                EEPROM_EraseSector(i); //está dividida la memoria
            }
            start=0;
            leer=0;
            LED_Write(0);
            CyDelay(1000);
            LCD_ClearDisplay();
            LCD_PrintString("Memoria borrada");
            CyDelay(500);
        break;
        default:
        break;
    }
    while(Funcion_Read()!=0b1111); //Espera hasta que se suelte el pulsador
    CyDelay(100);
    Funcion_ClearInterrupt(); //Borra solicitudes de interrupción
    ISR_Funcion_ClearPending();
}

void Leer_Sensor(void){ //Función de lectura del sensor I2C
    I2C_MasterSendStart(Sensor_Address,I2C_WRITE_XFER_MODE);
    I2C_MasterSendStop(); //Inicia comunicación con el sensor
    I2C_MasterSendStart(Sensor_Address,I2C_WRITE_XFER_MODE);
    I2C_MasterWriteByte(0x03); //Envía secuencia para lectura de datos:
    I2C_MasterWriteByte(0x00); //Primero comando, luego dirección de
    I2C_MasterWriteByte(0x04); //inicio de lectura y finalmente la
    I2C_MasterSendStop();      //cantidad de daos a leer
    CyDelay(2); //Espera más de 1.5ms para recibir respuesta
    I2C_MasterSendStart(Sensor_Address,I2C_READ_XFER_MODE);
    for (j=0;j<7;j++){ //Lee y almacena 7  datos 
        Tabla[j]=I2C_MasterReadByte(I2C_ACK_DATA);    
    }
    Tabla[j]=I2C_MasterReadByte(I2C_NAK_DATA); //Lee dato 8
    I2C_MasterSendStop(); //Envía bit de parada al sensor
}

CY_ISR(INT_Timer){//Alamacena datos en memoria cada 5 minutos  
    if (vacio==n_datos){//Si está lleno lo informa
        Timer_Stop();
        CyPins_SetPin(LED_1);
    }else{ //Si no alamcena el dato
        Leer_Sensor();
        EEPROM_WriteByte(Tabla[4],vacio); //Almacena temperatura
        EEPROM_WriteByte(Tabla[5],vacio+1);
        EEPROM_WriteByte(Tabla[2],vacio+2); //Almacena humedad
        EEPROM_WriteByte(Tabla[3],vacio+3);
        vacio=vacio+4;
    }                                                                                                                               
    ISR_Timer_ClearPending();
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    I2C_Start();
    LCD_Start();  
    ISR_Funcion_StartEx(Int_Funcion);
    ISR_Timer_StartEx(INT_Timer);
    start=0;
    leer=0;
    for(i=0;i<n_datos;i=i+4){ //Verifica ocupación de la memoria
        if((EEPROM_ReadByte(i)==0)&&(EEPROM_ReadByte(i+1)==0)&&(EEPROM_ReadByte(i+2)==0)&&(EEPROM_ReadByte(i+3)==0)){
            vacio=i;
            i=n_datos;
        }else{
            if(i==(n_datos-4)) vacio=n_datos;
        }
    }
    
    for(;;)
    {             
        Leer_Sensor();        
        LCD_ClearDisplay();
        LCD_PrintString("Temp.: ");
        temperatura=(uint16)Tabla[4]&0x7F; //Concatena parte alta con baja de  
        temperatura=(temperatura<<8)+Tabla[5]; // valor leído para 16 bits
        itoa(temperatura,TablaConv,10); //Convierte dato a cadena
        if((Tabla[4]&0x80)==0x80) LCD_PutChar('-'); //Visualiza signo
        for(j=0;j<sizeof(TablaConv);j++){ //Visualiza los datos en la LCD
            if (TablaConv[j+1]>='0'&&TablaConv[j+1]<='9'){
                LCD_PutChar(TablaConv[j]);
            }else{
                if (j==0) LCD_PutChar('0');
                LCD_PutChar('.');
                LCD_PutChar(TablaConv[j]);
                j=sizeof(TablaConv)-1;
            }
        } 
        LCD_PutChar(0xDF); //Visualiza símbolo de grados
        LCD_PutChar('C');
        LCD_Position(1,0);
        LCD_PrintString("Humedad: ");
        humedad=(uint16)Tabla[2];
        humedad=(humedad<<8)+Tabla[3];
        itoa(humedad,TablaConv,10); //Convierte dato a cadena
        for(j=0;j<sizeof(TablaConv);j++){ //Visualiza los datos en la LCD
            if (TablaConv[j+1]>='0'&&TablaConv[j+1]<='9'){
                LCD_PutChar(TablaConv[j]);
            }else{
                if (j==0) LCD_PutChar('0');
                LCD_PutChar('.');
                LCD_PutChar(TablaConv[j]);
                j=sizeof(TablaConv)-1;
            }
        }
        LCD_PutChar('%');
        CyDelay(1000);
    }
}

/* [] END OF FILE */
