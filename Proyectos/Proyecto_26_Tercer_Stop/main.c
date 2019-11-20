/* =======================================================================
 * Proyecto 26:
 * Programa que muestra el algoritmo para controlar un tercer Stop para
 * automóvil, incluye señales de giro a la izquierda o a la derecha,
 * implementado con cuatro matrices de LED de 8x8. El sistema cuenta con 
 * cuatro entradas, dos de ellas para recibir las señales de giro a la
 * izquierda o a la derecha, otra para la señal de parada del vehículo y 
 * una cuarta para la señal de parqueo. 
 * El sistema opera de la siguiente manera:
 *      Cuando se recibe la señal de parada en las matrices se visualiza
 *      de inmediato la palabra STOP, luego de cinco segundos esta palabra
 *      parpadea cinco veces para luego iniciar el ciclo, es decir, 
 *      nuevamente cinco segundos estática y luego parpadeo cinco veces,
 *      mientras la señal correspondiente este presente.
 *  	Con la señal de cruce a la izquierda una flecha se desplaza del
 *      centro (desde la segunda matriz) hacia la izquierda constantemente,
 *      mientras la señal correspondiente este presente.
 *      Con la señal de cruce a la derecha una flecha se desplaza del
 *      centro (desde la segunda matriz) hacia la derecha constantemente,
 *      mientras la señal correspondiente este presente.
 *      Si se recibe la señal de parqueo se muestran las dos flechas
 *      parpadeando mientras la señal correspondiente este presente.
 *      Al recibir simultáneamente las señales de parada y de parqueo, se
 *      muestra la palabra STOP por medio segundo y luego las dos flechas
 *      por otro medio segundo, esto ocurre mientras las señales
 *      correspondientes estén presentes.
* ========================================================================
*/

#include "project.h"

uint8 i,j,k;
uint16 dato;

#define modo 0x0900
#define intensidad 0x0A00
#define escaneo 0x0B00
#define shutdown 0x0C00
#define test 0x0F00

const uint8 tabla_en[]={0xFF,0xFE,0xFD,0xFB,0xF7};

void Write_Matriz(uint8 n_matriz,uint16 datom){
    Control_Reg_Write(tabla_en[n_matriz]); //Selecciona matriz
    SPIM_WriteTxData(datom); //Envía dato a la matriz
    CyDelayUs(10); //Espera a que se ejecute comando
    Control_Reg_Write(tabla_en[0]);
}

void Ini_Matriz(uint8 n_ini){ //Comandos de configuración del controlador
uint16 i;
    Write_Matriz(n_ini,escaneo+0x07); //Habilita las 8 columnas de la matriz
    Write_Matriz(n_ini,modo+0x00); //Registro de modo de decodificación
    Write_Matriz(n_ini,test+0x00); //No realizar test a la matriz
    Write_Matriz(n_ini,shutdown+0x01); //Modo de operación normal (no shutdown)
    Write_Matriz(n_ini,intensidad+0x04); //Brillo de la matriz medio-bajo (00-0F)   
    for (i=0x0100;i<0x0900;i=i+0x0100){
        Write_Matriz(n_ini,i); //Limpiar todas las columnas de la matriz
    }
}

const uint8 tabla_stop [4] [8] ={ //Tabla con la palabra STOP
    {0x46,0xCF,0x89,0x99,0x99,0xF3,0x62,0x00}, // 'S'
    {0x03,0x81,0xFF,0xFF,0xFF,0x81,0x03,0x00}, // 'T'
    {0x3C,0x7E,0xC3,0x81,0xC3,0x7E,0x3C,0x00}, // 'O'
    {0x81,0xFF,0xFF,0x91,0x11,0x1F,0x0E,0x00} // 'P'
};

const uint8 tabla_fi[]={ //Tabla con la flecha a la izquierda
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x18,0x3C,0x3C,0x7E,0x7E,0xFF,0xFF,0x3C,
    0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const uint8 tabla_fd[]={ //Tabla con la flecha a la derecha
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,
    0x3C,0xFF,0xFF,0x7E,0x7E,0x3C,0x3C,0x18,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};                                    

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    SPIM_Start();
    Ini_Matriz(1); //Configura la matriz 1
    Ini_Matriz(2); //Configura la matriz 2
    Ini_Matriz(3); //Configura la matriz 3
    Ini_Matriz(4); //Configura la matriz 4
                 
    for(;;)
    {
        switch(Signals_Read()){
            case 0b1110: //Visualiza flecha a la izquierda con desplazamiento
                for (j=0;j<(sizeof(tabla_fi)-15);j++){ //Lee tabla con mensaje a desplazar
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila
                        dato=(((i+1)*0x100)+ tabla_fi[j+i]);
                        Write_Matriz(1,dato); //Escribe dato en matriz 1
                    }
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila
                        dato=(((i+1)*0x100)+ tabla_fi[(j+8)+i]);
                        Write_Matriz(2,dato); //Escribe dato en matriz 2
                    }
                    CyDelay(50); //Tiempo de desplazamiento del mensaje
                }                        
            break;
            case 0b1101: //Visualiza mensaje letra a letra
                for (j=0;j<4;j++){
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                        dato=(((i+1)*0x0100)+tabla_stop[j][i]);
                        Write_Matriz(j+1,dato); //Escribe dato en matriz
                    }                
                }
                CyDelay(5000); //Mantiene el mensaje 5 segundos
                for (k=0;k<5;k++){
                    for (j=0;j<4;j++){
                        for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                            dato=(((i+1)*0x0100)+tabla_stop[j][i]);
                            Write_Matriz(j+1,dato); //Escribe dato en matriz
                        }                
                    }
                    CyDelay(500);
                    for (j=0;j<4;j++){
                        for(i=0;i<8;i++){                     
                            dato=(((i+1)*0x0100)+0); //Borra columna
                            Write_Matriz(j+1,dato); //Escribe dato en matriz
                        }                
                    }
                    CyDelay(300);
                }
            break;
            case 0b1011: //Visualiza flecha a la derecha con desplazamiento
                for (j=(sizeof(tabla_fi)-16);j>0;j--){ //Lee tabla con mensaje a desplazar
                    for(i=8;i>0;i--){ 
                        dato=((i*0x100)+ tabla_fd[(j+7)+(i-1)]);
                        Write_Matriz(4,dato); //Escribe dato en matriz 4
                    }
                    for(i=8;i>0;i--){ 
                        dato=((i*0x100)+ tabla_fd[(j-1)+(i-1)]);
                        Write_Matriz(3,dato); //Escribe dato en matriz 3
                    }
                    CyDelay(50); //Tiempo de desplazamiento del mensaje
                }  
            break;
            case 0b0111: //Visualiza ambas flechas
                for (j=0;j<2;j++){
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                        dato=(((i+1)*0x0100)+tabla_fi[(j*8)+(i+16)]);
                        Write_Matriz(j+1,dato); //Escribe dato en matriz
                    }                
                }
                Write_Matriz(2,0x0800);
                for (j=0;j<2;j++){
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                        dato=(((i+1)*0x0100)+tabla_fd[(j*8)+(i+16)]);
                        Write_Matriz(j+3,dato); //Escribe dato en matriz
                    }                
                }
                Write_Matriz(3,0x0100);
                CyDelay(500);
                for (j=0;j<4;j++){
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                        dato=(((i+1)*0x0100)+0);
                        Write_Matriz(j+1,dato);
                    }                
                }
                CyDelay(300);                    
            break;
            case 0b0101: //Visualiza STOP y ambas flechas
                for (j=0;j<4;j++){
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                        dato=(((i+1)*0x0100)+tabla_stop[j][i]);
                        Write_Matriz(j+1,dato);
                    }                
                }
                CyDelay(500);
                for (j=0;j<2;j++){
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                        dato=(((i+1)*0x0100)+tabla_fi[(j*8)+(i+16)]);
                        Write_Matriz(j+1,dato);
                    }                
                }
                Write_Matriz(2,0x0800); //Borra columna 8 de matriz 2
                for (j=0;j<2;j++){
                    for(i=0;i<8;i++){ //Lee los 8 datos de cada fila                    
                        dato=(((i+1)*0x0100)+tabla_fd[(j*8)+(i+16)]);
                        Write_Matriz(j+3,dato);
                    }                
                }
                Write_Matriz(3,0x0100); //Borra columna 1 de matriz 3
                CyDelay(500);
            break;    
            default:
            break;
        }
    }
}

/* [] END OF FILE */
