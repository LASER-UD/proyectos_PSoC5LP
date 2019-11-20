/* =======================================================================
 * Proyecto 6:
 * Programa que permite realizar el conteo de objetos o productos, que se
 * movilizan por una banda transportadora hacia la zona de empaque en una
 * fábrica típica (la banda transportadora se supone esta implementada ya
 * en el proceso), se utiliza una LCD alfanumérica para visualizar la 
 * información del conteo seleccionado y del valor del conteo actual, la
 * señal a contar se genera por un sensor de tipo óptico (pareja emisor
 * receptor infrarrojo separados 25cm), conectado directamente a un pin
 * del PSoC5LP, además se utiliza un arreglo de 8 pulsadores N.A para 
 * seleccionar el valor de conteo máximo de acuerdo a los siguientes
 * valores:
 *         Pulsador#  Conteo hasta         Pulsador#  Conteo hasta
 *             1           25                  5           250
 *             2           50                  6           500
 *             3          100                  7           750
 *             4          200                  8          1000
 * Al iniciar el proceso se muestra en la LCD la opción de seleccionar un
 * ciclo con el valor por defecto (100 conteos) o iniciarlo con otro valor,
 * si pasados 15 segundos no se modifica o confirma, el ciclo inicia
 * automáticamente con el valor por defecto. Al finalizar cualquiera de
 * las combinaciones el sistema genera una señal sonora indicando el final
 * del conteo, esta señal esta activa por cinco (5) segundos, para luego
 * mostrar en la LCD la opción de iniciar un nuevo ciclo con el valor 
 * actual seleccionado o iniciarlo con otro valor, si pasados 10 segundos
 * no se modifica o confirma, el ciclo inicia automáticamente con el valor
 * actual.
 * =======================================================================
*/

#include "project.h"

uint8 i;
volatile uint8 estado,stop;
volatile uint16 cont,conteo_max;

CY_ISR(INT_SW){
    switch(SW_Read()){ //Lee valor de pulsadores para determinar cual
                        //solicitó la interrupción.
        case 0xFE: //Verifica si se oprimió pulsador 1
            conteo_max=25; //Cambia a 25 el valor del conteo máximo
            estado=1;
            break;
        case 0xFD: //Verifica si se oprimió pulsador 2
            conteo_max=50; //Cambia a 50 el valor del conteo máximo
            estado=1;
            break;     
        case 0xFB: //Verifica si se oprimió pulsador 3
            conteo_max=100; //Cambia a 100 el valor del conteo máximo
            estado=1;
            break;
        case 0xF7: //Verifica si se oprimió pulsador 4
            conteo_max=200; //Cambia a 200 el valor del conteo máximo
            estado=1;
            break;
        case 0xEF: //Verifica si se oprimió pulsador 5
            conteo_max=250; //Cambia a 250 el valor del conteo máximo
            estado=1;
            break;
        case 0xDF: //Verifica si se oprimió pulsador 6
            conteo_max=500; //Cambia a 500 el valor del conteo máximo
            estado=1;
            break;            
        case 0xBF: //Verifica si se oprimió pulsador 7
            conteo_max=750; //Cambia a 750 el valor del conteo máximo
            estado=1;
            break;    
        case 0x7F: //Verifica si se oprimió pulsador 8
            conteo_max=1000; //Cambia a 1000 el valor del conteo máximo
            estado=1;
            break;    
        default: //Si se oprimieron más de dos pulsadores no se hace nada
            break;
    }
    while (SW_Read()!=0xFF); //Espera a que se suelte pulsador
    SW_ClearInterrupt(); //Borra posibles solicitudes de int. por rebote
}

CY_ISR(INT_Sensor){
    cont++; //Incrementa contador de objetos
    LCD_Position(1,12);
    LCD_PrintString("    "); //Borra ultimo valor escrito
    LCD_Position(1,12); ///Ubica cursor
    LCD_PrintDecUint16(cont); //Visualiza valor del contador
    if(cont==conteo_max){ //Verifica si se finalizó conteo
        cont=0; //Reinicia contador
        stop=1; //Variable para indicar fin de conteo
        ISR_Sensor_Disable(); //Deshabilita int. del sensor
        for(i=0;i<12;i++){ //Genera señal sonora por aprox. 5 segundos
            Buzzer_Write(1);
            CyDelay(210);
            Buzzer_Write(0);
            CyDelay(210);
        }
    }
    CyDelay(20);
    Sensor_ClearInterrupt(); //Borra peticiones de int. erróneas
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    LCD_Start(); //Inicializa LCD
    ISR_SW_StartEx(INT_SW); //Inicializa y conf. int. de los pulsadores
    ISR_Sensor_StartEx(INT_Sensor); //Inicializa y conf. int. del sensor
    ISR_Sensor_Disable(); //Deshabilita int. del sensor    
    estado=0; 
    stop=0;
    cont=0;
    conteo_max=100; //Valor de conteo máximo por defecto    
    LCD_ClearDisplay();
    LCD_PrintString("Seleccione ciclo de conteo");
    for(i=0;i<15;i++){ //Espera selección por 15 segundos          
        CyDelay(500);
        LCD_DisplayOff();
        CyDelay(500);
        LCD_DisplayOn();
        if (estado==1){ //Si se seleccionó valor
            i=14; //Sale del ciclo
            estado=0; //Inicializa variable            
        }            
    }
    ISR_SW_Disable(); //Deshabilita int. de los pulsadores
    ISR_Sensor_Enable(); //Habilita int. del sensor    
    LCD_ClearDisplay();
    LCD_PrintString("Cont. max.: "); 
    LCD_PrintDecUint16(conteo_max); //Visualiza valor máximo de conteo
    LCD_Position(1,0);
    LCD_PrintString("Cont. act.: ");
    LCD_PrintDecUint16(cont); //Visualiza valor del contador
    
    for(;;)
    {
        while(stop==0); //Espera hasta qe se finalice el conteo
        ISR_SW_Enable(); //Habilita int. de los pulsadores
        LCD_ClearDisplay();
        LCD_PrintString("Seleccione ciclo de conteo");
        for(i=0;i<10;i++){ //Espera selección por 10 segundos        
            CyDelay(500);
            LCD_DisplayOff();
            CyDelay(500);
            LCD_DisplayOn();
            if (estado==1){ //Si se seleccionó valor
                i=9; //Sale del ciclo
                estado=0; //Inicializa variable                
            }            
        }
        ISR_SW_Disable(); //Deshabilita int. de los pulsadores
        stop=0; //Variable que activa conteo
        ISR_Sensor_Enable(); //Habilita int. del sensor           
        LCD_ClearDisplay();
        LCD_PrintString("Cont. max.: "); 
        LCD_PrintDecUint16(conteo_max); //Visualiza valor máximo de conteo
        LCD_Position(1,0);
        LCD_PrintString("Cont. act.: ");
        LCD_PrintDecUint16(cont); //Visualiza valor del contador            
    }
}

/* [] END OF FILE */
