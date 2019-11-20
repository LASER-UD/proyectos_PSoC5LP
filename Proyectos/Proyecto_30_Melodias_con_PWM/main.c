/* =======================================================================
 * Proyecto 30:
 * Programa que muestra el algoritmo que permite controlar un reproductor
 * de melodías utilizando el bloque PWM del PSoC, las melodías están
 * almacenadas en la memoria EEPROM por lo que será posible reemplazarlas,
 * para esto se realiza comunicación serial con un PC para descargar el
 * archivo con la melodía deseada. Se tienen dos pulsadores N.A. que
 * permiten navegar por un menú donde se muestra el nombre de las posibles
 * melodías a reproducir y generar la función de reproducción. Por la
 * limitación en el tamaño de la memoria EEPROM del PSoC5LP solo se pueden
 * almacenar hasta 8 melodías cada una con una longitud máxima de 256
 * datos, están almacenadas en el formato RTTTL (Ringing Tones Text
 * Transfer Language).
* ========================================================================
*/

#include "project.h"

uint8 i,num,duracion_ini,octava=6,bpm,nota,escala;
uint16 t_nota,duracion_fin;
volatile uint8 seleccion,max_mel;

const uint16 Tabla_Periodo[]={
    0,
    7645,7216,6811,6428,6068,5727,5405,5102,4816,4545,4290,4050,
    3822,3608,3405,3214,3034,2863,2703,2551,2408,2273,2146,2025,
    1911,1804,1703,1607,1517,1432,1351,1276,1204,1136,1073,1012,
    956,902,851,804,758,716,676,638,602,568,536,506,
    478,451,426,402,379,358,338,319,301,284,268,253
};

uint8 melodia_0[20]; //Espacio para almacenar titulo de melodia 0
uint8 melodia_1[20]; //Espacio para almacenar titulo de melodia 1
uint8 melodia_2[20]; //Espacio para almacenar titulo de melodia 2
uint8 melodia_3[20]; //Espacio para almacenar titulo de melodia 3
uint8 melodia_4[20]; //Espacio para almacenar titulo de melodia 4
uint8 melodia_5[20]; //Espacio para almacenar titulo de melodia 5
uint8 melodia_6[20]; //Espacio para almacenar titulo de melodia 6
uint8 melodia_7[20]; //Espacio para almacenar titulo de melodia 7

void Leer_nombres(void){ //Lee los nombres de las melodias almacenadas
uint8 j;
uint16 k;        
    for(k=0;k<0x800;k+=0x100){
        j=0;
        if(EEPROM_ReadByte(k+j)!=0){ //Verifica si hay nombre almacenado
            while(EEPROM_ReadByte(k+j)!=':'){ //Lee el nombre de la melodia
                switch(k){
                    case 0: //Lee nombre de la melodia 0 y lo almacena
                        melodia_0[j]=EEPROM_ReadByte(k+j);
                    break;
                    case 0x100: //Lee nombre de la melodia 1 y lo almacena
                        melodia_1[j]=EEPROM_ReadByte(k+j);
                    break;
                    case 0x200: //Lee nombre de la melodia 2 y lo almacena
                        melodia_2[j]=EEPROM_ReadByte(k+j);
                    break;
                    case 0x300: //Lee nombre de la melodia 3 y lo almacena
                        melodia_3[j]=EEPROM_ReadByte(k+j);
                    break;    
                    case 0x400: //Lee nombre de la melodia 4 y lo almacena
                        melodia_4[j]=EEPROM_ReadByte(k+j);
                    break;
                    case 0x500: //Lee nombre de la melodia 5 y lo almacena
                        melodia_5[j]=EEPROM_ReadByte(k+j);
                    break;
                    case 0x600: //Lee nombre de la melodia 6 y lo almacena
                        melodia_6[j]=EEPROM_ReadByte(k+j);
                    break;
                    case 0x700: //Lee nombre de la melodia 7 y lo almacena
                        melodia_7[j]=EEPROM_ReadByte(k+j);
                    break;    
                    default:
                    break;    
                }
                j++;                
            }
            switch(k){ //Almacena ':' asociado al nombre
                case 0:
                    melodia_0[j]=EEPROM_ReadByte(k+j);
                break;
                case 0x100:
                    melodia_1[j]=EEPROM_ReadByte(k+j);
                break;
                case 0x200:
                    melodia_2[j]=EEPROM_ReadByte(k+j);
                break;
                case 0x300:
                    melodia_3[j]=EEPROM_ReadByte(k+j);
                break;    
                case 0x400:
                    melodia_4[j]=EEPROM_ReadByte(k+j);
                break;
                case 0x500:
                    melodia_5[j]=EEPROM_ReadByte(k+j);
                break;
                case 0x600:
                    melodia_6[j]=EEPROM_ReadByte(k+j);
                break;
                case 0x700:
                    melodia_7[j]=EEPROM_ReadByte(k+j);
                    max_mel=k>>8;
                    seleccion=max_mel;
                break;    
                default:
                break;
            }
        }else{
            max_mel=(k>>8)-1;
            seleccion=max_mel;
            k=0x700;
        }
    }    
}

void Reproducir(void){ //Reproduce la melodía seleccionada
uint16 posicion=0;    
    posicion=seleccion*0x100;
    i=0;
    while(EEPROM_ReadByte(posicion+i)!=':') i++; // Salta el nombre
    i++; //Salta ':'
    if(EEPROM_ReadByte(posicion+i)=='d'){ //Determina duración
        i++; i++; //Salta "d="
        duracion_ini=EEPROM_ReadByte(posicion+i)-0x30;
        i++; i++; //Salta la coma
    }        
    if(EEPROM_ReadByte(posicion+i)=='o'){ //Determina octava
        i++; i++; //Salta "o="
        num=EEPROM_ReadByte(posicion+i)-0x30;            
        if(num>=3&&num<=7) octava=num;
        i++; i++; //Salta la coma
    }        
    if(EEPROM_ReadByte(posicion+i)=='b'){ //Determina tempo o bpm
        i++; i++; // Salta "b="
        num=0;
        while(isdigit(EEPROM_ReadByte(posicion+i))){
            num=(num*10)+(EEPROM_ReadByte(posicion+i)-0x30);
            i++;
        }
        bpm=num;
        i++; //Salta la coma
    }
    // bmp representa el número de 1/4 de notas por minuto
    t_nota=(60*1000/bpm)*2;  //Tiempo de una nota completa en ms    
    while(EEPROM_ReadByte(posicion+i)!=0){ //Lee las notas de la melodia            
        num = 0; //Determina la duracion de la nota
        while(isdigit(EEPROM_ReadByte(posicion+i))){ 
            num=(num * 10)+(EEPROM_ReadByte(posicion+i)-0x30);
            i++;
        }
        if(num) duracion_fin=t_nota/num;
        else duracion_fin=t_nota/duracion_ini;
        nota=0;
        switch(EEPROM_ReadByte(posicion+i)){ //Determina la nota
            case 'c':
                nota=1;
            break;
            case 'd':
                nota=3;
            break;
            case 'e':
                nota=5;
            break;
            case 'f':
                nota=6;
            break;
            case 'g':
                nota=8;
            break;
            case 'a':
                nota=10;
            break;
            case 'b':
                nota=12;
            break;
            case 'p':
            default:
                nota=0;
        }
        i++;            
        if(EEPROM_ReadByte(posicion+i)=='#'){ //Verifica si dato es '#'
            nota++;
            i++;
        }
        if(EEPROM_ReadByte(posicion+i)=='.'){ //Verifica si dato es '.'
            duracion_fin += duracion_fin/2;
            i++;
        }
        if(isdigit(EEPROM_ReadByte(posicion+i))){ //Verifica la escala
            escala=EEPROM_ReadByte(posicion+i)-0x30;
            i++;
        }else{
          escala=octava;
        }
        if(EEPROM_ReadByte(posicion+i)==',') i++; //Salta la coma para leer
                                                  //la siguiente nota                                
        PWM_WritePeriod((Tabla_Periodo[(escala-3)*12+nota])); //Reproduce nota
        PWM_WriteCompare(Tabla_Periodo[(escala-3)*12+nota]/2);
        CyDelay(duracion_fin*2);            
    }
    PWM_WritePeriod(0);
    PWM_WriteCompare(0);
}

void sel_mel_0(void){ //Función para leer melodía 0
uint8 j=0;    
    while(melodia_0[j]!=':'){ 
        LCD_PutChar(melodia_0[j]);
        j++;
    }
}

CY_ISR(Int_Rx){
    Timer_Start(); //Habilita el Timer al recibir el primer carácter
}

CY_ISR(Int_Timer){ //Cuando pasan 100ms verifica la información almacenada
                   //en el buffer del bloque UART
uint8 j,size,dato;
uint16 direccion;    
    Timer_Stop();
    size=UART_GetRxBufferSize(); //Lee la cantidad de datos recibidos
    if(size>=20&&size<=250){ //Si se recibieron entre 20 y 200 datos continua
        dato=UART_GetChar();
        if(isdigit(dato)){ //Verifica que inicie en número                       
            dato=dato-0x030;
            direccion=dato*0x100;
            dato=UART_GetChar(); //Salta ':'
            for(j=0;j<=size;j++){ //Almacena la melodia recibida                  
                EEPROM_WriteByte(UART_GetChar(),direccion+j);
            }
            for(j=size+1;j<255;j++){ //Borra las posiciones restantes
                EEPROM_WriteByte(0,direccion+j);
            }
            LCD_ClearDisplay();
            LCD_Position(0,3);
            LCD_PrintString("Melodia ");
            LCD_PutChar((direccion>>8)+0x30);
            LCD_Position(1,3);
            LCD_PrintString("almacenada");
            Leer_nombres();
        }
    }
    UART_ClearRxBuffer();
}

CY_ISR(Int_Funcion){ //Determina la función a ejecutar
uint8 j;    
    switch (Funcion_Read()){ //Lee estado de pulsadores 
        case 0b10: //Visualiza los titulos de las melodias
            seleccion++; //Indica el titulo de la melodia a reproducir
            if(seleccion>max_mel) seleccion=0; //Máximo 8 melodias           
            LCD_ClearDisplay();
            switch(seleccion){ //Selecciona y visualiza la melodía
                case 0:
                    LCD_PrintString("0.");
                    sel_mel_0();
                break;
                case 1:
                    LCD_PrintString("1.");
                    j=0;
                    if(melodia_1[j]!=0){
                        while(melodia_1[j]!=':'){ 
                            LCD_PutChar(melodia_1[j]);
                            j++;
                        }
                    }else{
                        seleccion=0;
                        sel_mel_0();    
                    }
                break;
                case 2:
                    LCD_PrintString("2.");
                    j=0;
                    if(melodia_2[j]!=0){
                        while(melodia_2[j]!=':'){ 
                            LCD_PutChar(melodia_2[j]);
                            j++;
                        }
                    }else{
                        seleccion=0;
                        sel_mel_0();    
                    }
                break;
                case 3:
                    LCD_PrintString("3.");
                    j=0;
                    if(melodia_3[j]!=0){
                        while(melodia_3[j]!=':'){ 
                            LCD_PutChar(melodia_3[j]);
                            j++;
                        }
                    }else{
                        seleccion=0;
                        sel_mel_0();    
                    }
                break;
                case 4:
                    LCD_PrintString("4.");
                    j=0;
                    if(melodia_4[j]!=0){
                        while(melodia_4[j]!=':'){ 
                            LCD_PutChar(melodia_4[j]);
                            j++;
                        }
                    }else{
                        seleccion=0;
                        sel_mel_0();    
                    }
                break;
                case 5:
                    LCD_PrintString("5.");
                    j=0;
                    if(melodia_5[j]!=0){
                        while(melodia_5[j]!=':'){ 
                            LCD_PutChar(melodia_5[j]);
                            j++;
                        }
                    }else{
                        seleccion=0;
                        sel_mel_0();    
                    }
                break;
                case 6:
                    LCD_PrintString("6.");
                    j=0;
                    if(melodia_6[j]!=0){
                        while(melodia_6[j]!=':'){ 
                            LCD_PutChar(melodia_6[j]);
                            j++;
                        }
                    }else{
                        seleccion=0;
                        sel_mel_0();    
                    }
                break;
                case 7:
                    LCD_PrintString("7.");
                    j=0;
                    if(melodia_7[j]!=0){
                        while(melodia_7[j]!=':'){ 
                            LCD_PutChar(melodia_7[j]);
                            j++;
                        }
                    }else{
                        seleccion=0;
                        sel_mel_0();    
                    }
                break;    
            }
        break;
        case 0b01:
            Reproducir(); //Reproduce melodia seleccionada
        break;
        default:
        break;
    }
    while(Funcion_Read()!=0b11);
    CyDelay(100);
    Funcion_ClearInterrupt(); //Borra solicitudes de interrupción
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start();
    PWM_Start();
    EEPROM_Start();
    UART_Start();
    ISR_Funcion_StartEx(Int_Funcion);
    ISR_Rx_StartEx(Int_Rx);
    ISR_Timer_StartEx(Int_Timer);    
    Leer_nombres(); //Lee nombres y los almacena en la memoria
    LCD_ClearDisplay();
    LCD_Position(0,3);
    LCD_PrintString("Seleccione");
    LCD_Position(1,4);    
    LCD_PrintString("melodia");
    
    for(;;)
    {
    }
}

/* [] END OF FILE */
