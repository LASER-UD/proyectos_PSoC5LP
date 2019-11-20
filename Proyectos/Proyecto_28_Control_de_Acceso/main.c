/* =======================================================================
 * Proyecto 28:
 * Programa que muestra el algoritmo que permite operar un sistema de
 * control de acceso que almacena en la memoria EEPROM del PSoC5LP las
 * claves de cuatro dígitos de hasta 100 usuarios, cuenta con un teclado
 * matricial de 16 teclas que permite la interacción del usuario con el
 * sistema, una LCD para visualizar información al usuario del sistema, un
 * buzzer para generar sonidos asociados a la tareas realizadas y una 
 * salida temporizada que se encarga de generar la señal de ingreso o 
 * apertura de puerta controlada por un solenoide. La posición 00 es
 * utilizada siempre por el administrador del sistema, teniendo la
 * posibilidad de crear nuevos usuarios o borrar los ya existentes. 
 * La operación del sistema será la siguiente:
 *      Al encender el sistema se muestra un menú que le indica al usuario
 *      las posibles funciones a realizar: Tecla # para solicitar ingreso, 
 *      Tecla B para borrar un usuario, Tecla C para crear un usuario y 
 *      Tecla D para cambiar clave. La Tecla * se utiliza para cancelar
 *      alguna de las cuatro opciones anteriores.
 *      Al seleccionar la opción de solicitud de ingreso el usuario debe
 *      digitar el número de usuario asignado (valor entre 00 y 99) seguido
 *      por la clave de cuatro dígitos almacenada. La confirmación de
 *      solicitud de ingreso se realiza con la Tecla A. Si la clave es
 *      correcta se da ingreso por cinco segundos activando el solenoide,
 *      si no lo es se muestra un mensaje junto con un sonido para 
 *      indicarlo, de igual manera si el usuario no existe esta información
 *      es visualizada en la LCD.
 *      Al seleccionar la opción de borrado de usuario, únicamente el 
 *      administrador puede hacerlo, se ingresa el valor de usuario 
 *      asignado al administrador (00), su clave y el usuario de dos 
 *      dígitos a borrar (valor entre 01 y 99). La confirmación de 
 *      solicitud de borrado de usuario se realiza con la Tecla A. Si la
 *      clave es correcta se genera un mensaje indicando el borrado del
 *      usuario, si no lo es se muestra un mensaje junto con un sonido para
 *      indicarlo, de igual manera si el usuario no existe esta información
 *      se visualiza en la LCD.
 *      Al seleccionar la opción crear usuario, únicamente el administrador
 *      lo puede hacer, se ingresa el valor de usuario asignado al 
 *      administrador (00) y su clave, el sistema busca un usuario libre
 *      en la memoria EEPROM del PSoC y lo asigna luego de la confirmación
 *      de solicitud de creado de usuario que se realiza con la Tecla A. 
 *      Si la clave es correcta se genera un mensaje indicando el número
 *      de usuario asignado, si no lo es se muestra un mensaje junto con
 *      un sonido para indicarlo.
 *      Al seleccionar la opción de cambio de clave el usuario debe digitar
 *      el número de usuario asignado (valor entre 00 y 99) seguido por la
 *      clave de cuatro dígitos almacenada y la nueva clave de cuatro
 *      dígitos. La confirmación de cambio de clave se realiza con la
 *      Tecla A. Si la clave es correcta se genera un mensaje indicando el
 *      cambio, si no lo es se muestra un mensaje junto con un sonido para
 *      indicarlo, de igual manera si el usuario no existe esta información
 *      se visualiza en la LCD.
* ========================================================================
*/

#include "project.h"

uint8 i;
volatile uint8 bt=0xFF,cont=0;
uint8 Tabla[10];

void Click(uint8 time){ //Función para generar sonido de click al oprimir
    Buzzer_Write(1);    //una tecla
    CyDelay(time);
    Buzzer_Write(0);
}

void Vis_Tecla(void){ //Función para visualizar brevemente la tecla oprimida
    Click(20);
    LCD_PutChar(Tabla[cont]+0x30);
    CyDelay(200);
    LCD_Position(0,cont);
    LCD_PutChar('*');
    cont++; //Incrementa registro de conteo de cantidad de teclas oprimidas
}

CY_ISR (INT_KB){ //Rutina de interrupción para determinar tecla oprimida
    uint8 tecla,columna;
    tecla=Filas_Read(); //Lee valor de filas   
    columna=Columnas_Read(); //Lee valor de contador de las columnas
    tecla=tecla<<4;
    tecla=tecla|columna; //Concatena los dos datos obtenidos
    if(bt==0xFF){ //Determina si está esperando tecla de funciones
        switch(tecla){ //Determina la tecla oprimida
            case 0xD7: //Tecla B
                Click(20);
                bt=1; //Selecciona borrado de un usuario
                LCD_ClearDisplay();
                LCD_PrintString("Borrar usuario?");
                LCD_Position(1,0);
                LCD_PrintString("Ingrese usuario");    
            break;
            case 0xB7: //Tecla C
                Click(20);
                bt=2; //Selecciona crear un usuario
                LCD_ClearDisplay();
                LCD_PrintString("Crear usuario?");
                LCD_Position(1,0);
                LCD_PrintString("Usuario y clave");    
            break;
            case 0x77: //Tecla D
                Click(20);
                bt=3; //Selecciona cambiar clave
                LCD_ClearDisplay();
                LCD_PrintString("Cambiar clave?");
                LCD_Position(1,0);
                LCD_PrintString("Usuario y clave");    
            break;
            case 0x7B: //Tecla #
                Click(20);
                bt=0; //Selecciona solicitud de ingreso
                LCD_ClearDisplay();
                LCD_PrintString("Ingresar?");
                LCD_Position(1,0);
                LCD_PrintString("Usuario y clave"); 
            break;
            default:
            break;    
        }
    }else{//o teclas numéricas
        if(cont==0) LCD_ClearDisplay();
        switch(tecla){ //Determina la tecla oprimida            
            case 0xEE: //Tecla 1
                Tabla[cont]=1;
                Vis_Tecla();
            break;
            case 0xED: //Tecla 2
                Tabla[cont]=2;
                Vis_Tecla();
            break;
            case 0xEB: //Tecla 3
                Tabla[cont]=3;
                Vis_Tecla();
            break;       
            case 0xE7: //Tecla A
                Click(20);
                switch(bt){
                    case 0:
                        if (cont==6){
                            Tabla[0]=(Tabla[0]*0x0A+Tabla[1])*2;
                            Tabla[1]=Tabla[2]*0x10+Tabla[3];
                            Tabla[2]=Tabla[4]*0x10+Tabla[5];
                            LCD_Position(1,0);
                            if ((EEPROM_ReadByte(Tabla[0])==0)&&(EEPROM_ReadByte(Tabla[0]+1)==0)){
                                LCD_PrintString("Usuario no existe");
                                CyDelay(2000);
                            }else{    
                                if ((EEPROM_ReadByte(Tabla[0])==Tabla[1])&&(EEPROM_ReadByte(Tabla[0]+1)==Tabla[2])){
                                    LCD_PrintString("OK");
                                    Acceso_Write(1);
                                    Timer_Start();                                
                                    CyDelay(2000);
                                }else{
                                    LCD_PrintString("Clave errada");
                                    for (i=0;i<5;i++){
                                        Click(200);
                                        CyDelay(10);
                                    }                                
                                }
                            }
                        }
                        LCD_ClearDisplay();
                        LCD_PrintString("Selecc. funcion");
                        LCD_Position(1,3);
                        LCD_PrintString("#  B  C  D");
                    break;
                    case 1:
                        if (cont==8){
                            Tabla[0]=(Tabla[0]*0x0A+Tabla[1])*2;
                            Tabla[1]=Tabla[2]*0x10+Tabla[3];
                            Tabla[2]=Tabla[4]*0x10+Tabla[5];
                            LCD_Position(1,0);
                            if ((Tabla[0]==0)&&(EEPROM_ReadByte(0)==Tabla[1])&&(EEPROM_ReadByte(1)==Tabla[2])){                                
                                Tabla[1]=(Tabla[6]*0x0A+Tabla[7])*2;
                                EEPROM_WriteByte(0,Tabla[1]);
                                EEPROM_WriteByte(0,Tabla[1]+1);
                                LCD_PrintString("Usuario elimin.");                                                                                                                                                                                                       
                                CyDelay(2000);
                            }else{
                                LCD_PrintString("Clave errada");
                                for (i=0;i<5;i++){
                                    Click(200);
                                    CyDelay(10);
                                }                                
                            }
                        }
                        LCD_ClearDisplay();
                        LCD_PrintString("Selecc. funcion");
                        LCD_Position(1,3);
                        LCD_PrintString("#  B  C  D");
                    break;
                    case 2:
                        if (cont==6){
                            Tabla[0]=(Tabla[0]*0x0A+Tabla[1])*2;
                            Tabla[1]=Tabla[2]*0x10+Tabla[3];
                            Tabla[2]=Tabla[4]*0x10+Tabla[5];
                            LCD_Position(1,0);
                            if ((Tabla[0]==0)&&(EEPROM_ReadByte(0)==Tabla[1])&&(EEPROM_ReadByte(1)==Tabla[2])){                                
                                for(i=2;i<200;i=i+2){
                                    if((EEPROM_ReadByte(i)==0)&&(EEPROM_ReadByte(i+1)==0)){
                                        EEPROM_WriteByte(0x12,i);
                                        EEPROM_WriteByte(0x34,i+1);
                                        LCD_PrintString("Usuario # ");
                                        if((i/2)<10) LCD_PutChar('0');
                                        LCD_PrintNumber(i/2);
                                        i=198;
                                    }
                                }                                                                                                                               
                                CyDelay(5000);
                            }else{
                                LCD_PrintString("Clave errada");
                                for (i=0;i<5;i++){
                                    Click(200);
                                    CyDelay(10);
                                }                                
                            }
                        }
                        LCD_ClearDisplay();
                        LCD_PrintString("Selecc. funcion");
                        LCD_Position(1,3);
                        LCD_PrintString("#  B  C  D");
                    break;
                    case 3:
                        if (cont==10){
                            Tabla[0]=(Tabla[0]*0x0A+Tabla[1])*2;
                            Tabla[1]=Tabla[2]*0x10+Tabla[3];
                            Tabla[2]=Tabla[4]*0x10+Tabla[5];
                            LCD_Position(1,0);
                            if ((EEPROM_ReadByte(Tabla[0])==0)&&(EEPROM_ReadByte(Tabla[0]+1)==0)){
                                LCD_PrintString("Usuario no existe");
                                CyDelay(2000);
                            }else{
                                if ((EEPROM_ReadByte(Tabla[0])==Tabla[1])&&(EEPROM_ReadByte(Tabla[0]+1)==Tabla[2])){                                
                                    Tabla[1]=Tabla[6]*0x10+Tabla[7];
                                    Tabla[2]=Tabla[8]*0x10+Tabla[9];
                                    EEPROM_WriteByte(Tabla[1],Tabla[0]);
                                    EEPROM_WriteByte(Tabla[2],Tabla[0]+1);
                                    LCD_PrintString("Clave modificada");                                                                                                                                                                                                       
                                    CyDelay(2000);
                                }else{
                                    LCD_PrintString("Clave errada");
                                    for (i=0;i<5;i++){
                                        Click(200);
                                        CyDelay(10);
                                    }                                
                                }
                            }
                        }
                        LCD_ClearDisplay();
                        LCD_PrintString("Selecc. funcion");
                        LCD_Position(1,3);
                        LCD_PrintString("#  B  C  D");
                    break;
                    break;
                    default:
                    break;    
                } 
                bt=0xFF;
                cont=0;
            break;            
            case 0xDE: //Tecla 4
                Tabla[cont]=4;
                Vis_Tecla();  
            break;
            case 0xDD: //Tecla 5
                Tabla[cont]=5;
                Vis_Tecla();
            break;
            case 0xDB: //Tecla 6
                Tabla[cont]=6;
                Vis_Tecla();
            break;
            case 0xBE: //Tecla 7
                Tabla[cont]=7;
                Vis_Tecla();
            break;
            case 0xBD: //Tecla 8
                Tabla[cont]=8;
                Vis_Tecla();
            break;
            case 0xBB: //Tecla 9
                Tabla[cont]=9;
                Vis_Tecla();
            break;            
            case 0x7E: //Tecla * para cancelar
                Click(20);
                bt=0xFF;
                cont=0;
                LCD_ClearDisplay();
                LCD_PrintString("Selecc. funcion");
                LCD_Position(1,3);
                LCD_PrintString("#  B  C  D");
            break;
            case 0x7D: //Tecla 0
                Tabla[cont]=0;
                Vis_Tecla();
            break;
            default:
            break;
        }
    }
    while(Filas_Read()!=0x0F);
    CyDelay(100);
    Filas_ClearInterrupt();
}

CY_ISR(INT_Timer){  //Interrupción del Timer al finalizar el tiempo de
    Acceso_Write(0);//cinco segundos asignado para la activación del
    Timer_Stop();   //solenoide que permite el ingreso
    ISR_Timer_ClearPending();
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    LCD_Start();
    EEPROM_Start();
    ISR_KB_StartEx(INT_KB);
    ISR_Timer_StartEx(INT_Timer);
    LCD_PrintString("Selecc. funcion"); //Visualiza mensaje inicial
    LCD_Position(1,3);
    LCD_PrintString("#  B  C  D");
    
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
