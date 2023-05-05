//
// Created by utnso on 10/6/22.
//

#ifndef SHARED_ESTRUCTURAS_H
#define SHARED_ESTRUCTURAS_H
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdio.h>
#include <semaphore.h>
#include <malloc.h>
#include <config_utils.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    uint32_t PID; // 4 bytes
    uint8_t cantidad_instrucciones;
    t_list* instrucciones; //12 bytes
    uint8_t cantidad_segmentos;
    t_list* tam_segmentos;
}t_proceso; //Puede ser que haya que agregarle mas parametros como el segmento

typedef struct {
    uint8_t idLength;
    char* id; // el id seria el nombre de la instruccion
	uint8_t param1Length;
    char* param1;
	uint8_t param2Length;
    char* param2;
    uint8_t param3Length;
    char* param3;
} instr_t;

typedef struct {
    uint8_t idLength;
    char* id; // el id seria el nombre de la instruccion
    uint8_t param1Length;
    uint8_t cantidad_parametros;
    char* param1;
    uint8_t param2Length;
    char* param2;
    uint8_t param3Length;
    char* param3;
} t_instr;


//Registros de CPU
typedef struct{

    char registroAX[4];
    char registroBX[4];
    char registroCX[4];
    char registroDX[4];
    char registroEAX[8];
    char registroEBX[8];
    char registroECX[8];
    char registroEDX[8];
    char registroRAX[16];
    char registroRCX[16];
    char registroRBX[16];
    char registroRDX[16];
}registros_cpu;
//Tabla de Segmentos

typedef struct
{
    uint32_t pid;
    t_list* segmentos;
} t_tablaSegmentos;

typedef struct
{
    uint32_t base;
    uint32_t limite;
    uint32_t id;
} t_segmento;

typedef struct
{
    uint32_t tamanio;
    void* datos;
} t_datos;



typedef struct {
    int fd;
    char *server_name;
} t_procesar_conexion_args;

typedef struct {
    t_queue cola;
    char* nombre;
    int indiceSemaforo;
    int tiempoRetardo;
} t_atenderColaDispositivo_args;




//Estructura del pcb
typedef struct{
	uint32_t id;
	t_list* instr;
    registros_cpu* registrosCpu;
	uint32_t programCounter;
    t_list* tablaSegmentos;
} t_pcb;



typedef struct{ //Usada cuando hay PF
    uint32_t  segmento;
    uint32_t pagina;
    t_pcb* pcb;
}pcb_page_fault;

typedef struct {
    char* nombreDispositivo;
    int tiempoRetardo;
    t_queue* cola; //Aca guardo las PCB de los procesos
    int indiceSemaforo;
}cola_dispositivo;


// ESTOS SON LOS TIPOS QUE VAN EN LA PCB {SANTI}

t_proceso *crearNuevoProceso();
void closure_mostrarListaInstrucciones(instr_t* element);
void liberarInstruccion(instr_t* instruccion);

bool esInstruccionSinParametros(instr_t* instruccion);
bool esInstruccionConUnParametro(instr_t* instruccion);
bool esInstruccionConDosParametros(instr_t* instruccion);
bool esInstruccionConTresParametros(instr_t* instruccion);

void mostrarIntArray(uint32_t *array, char*message, t_log* logger);

void simularRetardoSinMensaje(int retardo);
void simularRetardo(char* message,char* messageFinal, t_log* logger, int retardo);
t_list * convertirIntArrayALista(uint32_t * array);
uint32_t* convertirIntListaToArray(t_list* lista);

#endif //SHARED_ESTRUCTURAS_H
