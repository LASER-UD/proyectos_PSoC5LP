/* =======================================================================
 * Proyecto 27:
 * Programa que muestra el algoritmo que permite tomar mediciones de
 * temperatura y humedad relativa haciendo uso de un sensor I2C de
 * referencia AM2320, los datos leídos del sensor se visualizan en una LCD
 * alfanumérica de 16x2 que utiliza un controlador I2C de referencia
 * PCF8574, los datos se actualizan una vez por segundo. 
* ========================================================================
*/

#include "project.h"
#include <stdlib.h>

#define LCD_I2C_Address 0x3F //Dirección de la LCD
#define Sensor_Address 0x05C //Dirección del sensor AM2320

//Peso de cada comando de la LCD
#define LCD_I2C_CLEARDISPLAY 0x01
#define LCD_I2C_RETURNHOME 0x02
#define LCD_I2C_ENTRYMODESET 0x04
#define LCD_I2C_DISPLAYCONTROL 0x08
#define LCD_I2C_CURSORSHIFT 0x10
#define LCD_I2C_FUNCTIONSET 0x20
#define LCD_I2C_SETCGRAMADDR 0x40
#define LCD_I2C_SETDDRAMADDR 0x80

//Peso de bits para función display entry mode
#define LCD_I2C_ENTRYRIGHT 0x00
#define LCD_I2C_ENTRYLEFT 0x02
#define LCD_I2C_ENTRYSHIFTDECREMENT 0x00
#define LCD_I2C_ENTRYSHIFTINCREMENT 0x01

//Peso de bits para función display on/off control
#define LCD_I2C_DISPLAYOFF 0x00
#define LCD_I2C_DISPLAYON 0x04
#define LCD_I2C_CURSOROFF 0x00
#define LCD_I2C_CURSORON 0x02
#define LCD_I2C_BLINKOFF 0x00
#define LCD_I2C_BLINKON 0x01

//Peso de bits para función display/cursor shift
#define LCD_I2C_CURSORMOVE 0x00
#define LCD_I2C_DISPLAYMOVE 0x08
#define LCD_I2C_MOVERIGHT 0x04
#define LCD_I2C_MOVELEFT 0x00

//Peso de bits para función function set
#define LCD_I2C_4BITMODE 0x00
#define LCD_I2C_2LINE 0x08
#define LCD_I2C_5x7DOTS 0x00

//Peso de bits para el control de backlight
#define LCD_I2C_NOBACKLIGHT 0x00
#define LCD_I2C_BACKLIGHT 0x08

#define En 0x04  // Enable bit
#define Rs 0x01  // Register select bit

uint8 _cols,_rows,_backlightval,j,Tabla[8];
uint16 temperatura,humedad;
char TablaConv[4];

void LCD_I2C_Size(uint8 lcd_rows,uint8 lcd_cols){ //Función para almacenar
	_cols = lcd_cols;                             //tamaño de LCD utilizada
	_rows = lcd_rows;
	_backlightval = LCD_I2C_BACKLIGHT;
}

void I2C_M_Write_Byte(uint8 data){ //Función de envío de datos por el I2C
    I2C_MasterSendStart(LCD_I2C_Address,0); //Envía bit de inicio
    I2C_MasterWriteByte(data | _backlightval); //Envía dato al bus
    I2C_MasterSendStop(); //Envía bit de parada
}

void pulseEnable(uint8 _data){ //Genera pulso de Enable    
	I2C_M_Write_Byte(_data | En); //Pin Enable en alto
	CyDelayUs(1); //Pulso debe tener un ancho > 450ns
	I2C_M_Write_Byte(_data & ~En); //Pin Enable en bajo
	CyDelayUs(50);		//Comando necesita más de 37us para su ejecución
}

void Write4bits(uint8 value){ //Función de envío de 4 bits a la LCD
	I2C_M_Write_Byte(value);
	pulseEnable(value);
}

void send(uint8 value,uint8 mode){ //Escritura de cualquier comando o dato
	uint8 highnib=value&0xf0;
	uint8 lownib=(value<<4)&0xf0;
	Write4bits((highnib) | mode); //mode=0 comando, mode=1 dato
	Write4bits((lownib) | mode); 
}

void LCD_I2C_WriteControl(uint8 value){
    send(value,0);
}

 void LCD_I2C_WriteData(uint8 value){
	send(value,Rs);
}

void LCD_I2C_PrintString(char word[]){ //Visualiza una cadena de 
uint32 i;                              //caracteres en la LCD
uint8 size=strlen(word);    
    for (i=0;i<size;i++){
        LCD_I2C_WriteData(word[i]);
    }
}

void LCD_I2C_PutChar(char word){ //Visualiza un carácter en la LCD 
    LCD_I2C_WriteData(word);             
}

void LCD_I2C_ClearLCD(void){    
	LCD_I2C_WriteControl(LCD_I2C_CLEARDISPLAY);//Borrar display
	CyDelay(2); //Tiempo de ejecución del comando
    LCD_I2C_WriteControl(LCD_I2C_RETURNHOME); //Cursor al inicio
	CyDelay(2); //Tiempo de ejecución del comando
}

void LCD_I2C_Position(uint8_t row,uint8 col){ //Ubica cursor fila, columna
uint8 row_offsets[]={0x00,0x40,0x14,0x54};
	if (row>_rows) row=_rows-1;
	LCD_I2C_WriteControl(LCD_I2C_SETDDRAMADDR | (col + row_offsets[row]));
}


void LCD_I2C_noBacklight(void){ //Backlight Off
	_backlightval=LCD_I2C_NOBACKLIGHT;
	I2C_M_Write_Byte(0);
}

void LCD_I2C_Backlight(void){ //Backlight On
    _backlightval=LCD_I2C_BACKLIGHT;
	I2C_M_Write_Byte(0);
}

//Llena las primeras 8 posiciones de CGRAM con caracteres personalizados
void LCD_I2C_createChar(uint8 location,uint8 charmap[]){ 
uint32 i;
	location&=0x07; //Cantidad máxima de posiciones personalizables
	LCD_I2C_WriteControl(LCD_I2C_SETCGRAMADDR | (location << 3));
	for (i=0;i<8;i++) {
		LCD_I2C_WriteData(charmap[i]);
	}
}

void load_custom_character(uint8 char_num,uint8 *rows){
	LCD_I2C_createChar(char_num,rows);
}

void LCD_I2C_Ini(void){ //Inicialización de la LCD
	CyDelay(50); //Tiempo de espera >15ms para estabilización de fuente DC
	I2C_M_Write_Byte(_backlightval); //Pone en '0' RS y R/W además enciende
	CyDelay(1000);                   //el backlight de la LCD
	//Configuración de la LCD en formato de 4 bits
	Write4bits(0x30); //Envia comando 3 veces
	CyDelayUs(4500); //Espera por más de 4.1ms
	Write4bits(0x30);
	CyDelayUs(4500); //Espera por más de 4.1ms
	Write4bits(0x30);
	CyDelayUs(150); //Espera por más de 100us
	Write4bits(0x20); //Finaliza enviando comando de interface de 4 bits
	//Configura: 2 línaes, letra de tamaño 5x7
	LCD_I2C_WriteControl(LCD_I2C_FUNCTIONSET | LCD_I2C_4BITMODE | LCD_I2C_2LINE | LCD_I2C_5x7DOTS);
	//Configura: Display On, cursor apagado y blinking deshabilitado
    LCD_I2C_WriteControl(LCD_I2C_DISPLAYCONTROL | LCD_I2C_DISPLAYON | LCD_I2C_CURSOROFF | LCD_I2C_BLINKOFF);
	//Borra LCD y ubica cursor al inicio
	LCD_I2C_ClearLCD();
	//Configura: escritura de izquierda a derecha
	LCD_I2C_WriteControl(LCD_I2C_ENTRYMODESET | LCD_I2C_ENTRYLEFT | LCD_I2C_ENTRYSHIFTDECREMENT);
}

int main(void)
{
    CyGlobalIntEnable; //Habilita interrupciones globales
    I2C_Start();
    LCD_I2C_Size(2,16); //Se utiliza LCD de 2x16
    LCD_I2C_Ini(); //Configura la LCD   
    
    for(;;)
    {      
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
        LCD_I2C_ClearLCD();
        LCD_I2C_PrintString("Temp.: ");
        temperatura=(uint16)Tabla[4]&0x7F; //Concatena parte alta con baja de  
        temperatura=(temperatura<<8)+Tabla[5]; // valor leído para 16 bits
        itoa(temperatura,TablaConv,10); //Convierte dato a cadena
        if((Tabla[4]&0x80)==0x80) LCD_I2C_PutChar('-'); //Visualiza signo
        for(j=0;j<sizeof(TablaConv);j++){ //Visualiza los datos en la LCD
            if (TablaConv[j+1]>='0'&&TablaConv[j+1]<='9'){
                LCD_I2C_PutChar(TablaConv[j]);
            }else{
                if (j==0) LCD_I2C_PutChar('0');
                LCD_I2C_PutChar('.');
                LCD_I2C_PutChar(TablaConv[j]);
                j=sizeof(TablaConv)-1;
            }
        } 
        LCD_I2C_PutChar(0xDF); //Visualiza símbolo de grados
        LCD_I2C_PutChar('C');
        LCD_I2C_Position(1,0);
        LCD_I2C_PrintString("Humedad: ");
        humedad=(uint16)Tabla[2];
        humedad=(humedad<<8)+Tabla[3];
        itoa(humedad,TablaConv,10); //Convierte dato a cadena
        for(j=0;j<sizeof(TablaConv);j++){ //Visualiza los datos en la LCD
            if (TablaConv[j+1]>='0'&&TablaConv[j+1]<='9'){
                LCD_I2C_PutChar(TablaConv[j]);
            }else{
                if (j==0) LCD_I2C_PutChar('0');
                LCD_I2C_PutChar('.');
                LCD_I2C_PutChar(TablaConv[j]);
                j=sizeof(TablaConv)-1;
            }
        }
        LCD_I2C_PutChar('%');
        CyDelay(1000);
    }
}

/* [] END OF FILE */
