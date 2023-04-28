//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>
char* ip_kernel;
char* puerto_kernel;
int fd_kernel;
int fd_memoria;
int fd_cpu;
int fd_filesystem;

t_log* logger_kernel;


void procesar_conexion(void *void_args) {
    t_procesar_conexion_args *args = (t_procesar_conexion_args *) void_args;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_trace(trace_logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            //----------------------------------------CONSOLA-----------------------
            case DEBUG:
            {
                break;
            }
            case GESTIONAR_CONSOLA_NUEVA:
            {
                t_list* instrucciones = recibirListaInstrucciones(cliente_socket);
                break;

            }
            case 100:
            {

                break;
            }
            case 1000:
            {
                break;
            }

                //--------------------------------CPU-------------------------------------------

            case WAIT:
            {
                pcb* pcbRecibida = recibir_pcb(cliente_socket);
                manejoDeRecursos(pcbRecibida,"WAIT");
                break;
            }
            case SIGNAL:{
                pcb* pcbRecibida = recibir_pcb(cliente_socket);
                manejoDeRecursos(pcbRecibida,"SIGNAL");
                break;
            }

            case 200:
            {
                break;
            }
            case 2000: {
                break;
            }
            case 20000: {
                break;
            }

            case 200000: {
                break;
            }

            case 30:
            {

                break;
            }

                //----------------------------------MEMORIA----------------------------------------
            case CREADA_ESTRUCTURA_PCB_NUEVO:
            {
                //FALTA ACLARAR COMO LLEGA EL DATO
               // log_info(logger_kernel, "PID: [%d] - Estado Anterior: NEW - Estado Actual: READY.", pcbReady->pid);
                break;
            }

            case 3000:
            {
                break;
            }

            case -1:
                log_error(error_logger, "Cliente desconectado de %s...", server_name);
                return;
            default:
                log_error(error_logger, "Algo anduvo mal en el server de %s", server_name);
                log_trace(trace_logger, "Cop: %d", cop);
                return;
        }
    }
    log_warning(warning_logger, "El cliente se desconecto de %s server", server_name);
    return;
}

void* crearServidor(){
    puerto_kernel=cfg_kernel->PUERTO_ESCUCHA;
    fd_kernel = iniciar_servidor(info_logger, "SERVER KERNEL", ip_kernel, puerto_kernel);
    if (fd_kernel == 0) {
        log_error(error_logger, "Fallo al crear el servidor, cerrando KERNEL");
        return (void* )EXIT_FAILURE;
    }
    while (server_escuchar(info_logger, "SERVER KERNEL", (uint32_t)fd_kernel));
}

int server_escuchar(t_log *logger, char *server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t atenderProcesoNuevo;
        t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&atenderProcesoNuevo, NULL,(void*) procesar_conexion,args);
        pthread_detach(atenderProcesoNuevo);
        return 1;
    }
    return 0;
}


void* conectarConCPU(){
    int comprobacion = generarConexionesConCPU();

    if(comprobacion){
    atenderCpu();
     }


}

bool atenderCpu(){
    if (fd_cpu == -1){
        return EXIT_FAILURE;
    }
    pthread_t atenderDispatch;
    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->fd = fd_cpu;
    args->server_name = "ATENDER_CPU";
    pthread_create(&atenderDispatch, NULL,(void*)procesar_conexion,args);
    pthread_detach(atenderDispatch);
    return true;
}


bool generarConexionesConCPU() {

    char *ip;

    ip = strdup(cfg_kernel->IP_CPU);
    log_trace(trace_logger, "Lei la IP [%s]", ip);

    char *puerto;
    puerto = strdup(cfg_kernel->PUERTO_CPU);

    log_trace(trace_logger, "Lei el PUERTO [%s]", puerto);

    fd_cpu = crear_conexion(
            info_logger,
            "SERVER CPU",
            ip,
            puerto
    );

    free(ip);
    free(puerto);

    return fd_cpu != 0;
}


void* conectarConMemoria(){
    bool comprobacion = generarConexionesConMemoria();
    atenderMemoria();

}


bool generarConexionesConMemoria(){
    char* ip;

    ip = strdup(cfg_kernel->IP_MEMORIA);
    log_trace(trace_logger,"Lei la IP [%s]", ip);

    char* puerto;
    puerto = strdup(cfg_kernel->PUERTO_MEMORIA);

    log_trace(trace_logger,"Lei el PUERTO [%s]", puerto);

    fd_memoria = crear_conexion(
            info_logger,
            "SERVER MEMORIA",
            ip,
            puerto
    );


    free(ip);
    free(puerto);

    return fd_memoria != 0;

}


bool atenderMemoria(){
    if (fd_memoria == -1){
        return EXIT_FAILURE;
    }
    pthread_t atenderMemoria;
    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->fd = fd_memoria;
    args->server_name = "ATENDER_MEMORIA";
    pthread_create(&atenderMemoria, NULL,(void*)procesar_conexion,args);
    pthread_detach(atenderMemoria);
    return true;
}


void* conectarConFileSystem(){
    bool comprobacion = generarConexionesConFilesystem();

    if(comprobacion){
        atenderFilesystem();
    }

}


bool generarConexionesConFilesystem(){
    char* ip;

    ip = strdup(cfg_kernel->IP_FILESYSTEM);
    log_trace(trace_logger,"Lei la IP [%s]", ip);

    char* puerto;
    puerto = strdup(cfg_kernel->PUERTO_FILESYSTEM);

    log_trace(trace_logger,"Lei el PUERTO [%s]", puerto);

    fd_filesystem = crear_conexion(
            info_logger,
            "SERVER FILESYSTEM",
            ip,
            puerto
    );

    free(ip);
    free(puerto);

    return fd_filesystem != 0;

}


bool atenderFilesystem(){
    if (fd_filesystem == -1){
        return EXIT_FAILURE;
    }
    pthread_t atenderFilesystem;
    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->fd = fd_filesystem;
    args->server_name = "ATENDER_FILESYSTEM";
    pthread_create(&atenderFilesystem, NULL,(void*)procesar_conexion,args);
    pthread_detach(atenderFilesystem);
    return true;
}


void cortar_conexiones(){
    liberar_conexion(&fd_kernel);
    liberar_conexion(&fd_cpu);
    liberar_conexion(&fd_memoria);
    liberar_conexion(&fd_filesystem);
    log_info(logger_kernel,"CONEXIONES LIBERADAS");
}

void cerrar_servers(){
    close(fd_kernel);
    log_info(logger_kernel,"SERVIDORES CERRADOS");
}

void waitRecursoPcb(t_recurso * recurso, pcb* unaPcb) {
    recurso->instanciasRecurso--;
    if (recurso->instanciasRecurso < 0) {
        queue_push(recurso->cola, unaPcb);
    }
}


void signalRecursoPcb(t_recurso * recurso, pcb* unaPcb){
    recurso->instanciasRecurso++;
    if(!queue_is_empty(recurso->cola)){
        pcb* pcbLiberada = queue_pop(recurso->cola);
        // moverProceso_BloqReady(pcbLiberada);
    }
    enviar_paquete_pcb(unaPcb,fd_cpu,SIGNAL,logger_kernel);
}

void manejoDeRecursos(pcb* unaPcb,char* orden){
    int apunteProgramCounter = unaPcb->programCounter;
    instr_t* instruccion = list_get(unaPcb->instr,apunteProgramCounter);
    char* recursoSolicitado = instruccion->param2;
    //TODO hay que agregarle semaforo mutex a la cola bloqueado

    for(int i = 0 ; i < list_size(estadoBlockRecursos); i++){
        t_recurso* recurso = list_get(estadoBlockRecursos,i);
        if((strcmp(recurso->nombreRecurso,recursoSolicitado)) == 0){
            if((strcmp(orden,"WAIT")) == 0){
                waitRecursoPcb(recurso,unaPcb);
            }else{
                signalRecursoPcb(recurso,unaPcb);
            }
        }else{
            //moverProceso_ExecExit(unaPcb);
        }
        }
}

