#include "client.h"

int enviar_handshake(int fd_conexion, int32_t handshake){
	size_t bytes;
	int32_t result;

	bytes = send(fd_conexion, &handshake, sizeof(int32_t), 0);
	bytes = recv(fd_conexion, &result, sizeof(int32_t), MSG_WAITALL);

	return result;
}

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	//socket de conexion
	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	int32_t handshake = 1;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"

	log_info(logger,"Soy un log del cliente");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config
	log_info(logger,"Clave: %s", valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	if (enviar_handshake(conexion, handshake) == 0) {
	    log_info(logger, "Buenos días señor servidor");
	} else {
	    log_info(logger, "Uh, te re enojaste");
		exit(EXIT_FAILURE); //tengo mis dudas de este uso del exit()
	}

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "logCliente", true, LOG_LEVEL_INFO);
	// verificacion del tipo: "if (nuevo_logger == NULL) perror(...);" ya esta en log.c 

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");
	if (nuevo_config == NULL) {
    	// ¡No se pudo crear el config!
		perror("¡No se pudo crear el config!");
    	// Terminemos el programa
		exit(EXIT_FAILURE);
	}
	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (! string_is_empty(leido))
	{
		log_info(logger, "%s", leido);
		free(leido);
		leido = readline("> ");
	}
	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}


void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido = readline("Armando paquete:\n> ");
	t_paquete* paquete = crear_paquete();
	// Leemos y esta vez agregamos las lineas al paquete
	while (! string_is_empty(leido))
	{
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> ");
	}

	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);
	free(leido);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	  liberar_conexion(conexion);
	  log_destroy(logger);
	  config_destroy(config);
}
