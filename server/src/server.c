#include "server.h"

int main(void) {
	int32_t hs_ok, hs_error, protocolo_hs_ok;
	hs_ok = 0;
	hs_error = -1;
	protocolo_hs_ok = 1;

	logger = log_create("log.log", "Servidor", true, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	//dudoso, capaz esta de mas esto
	if (cliente_fd == -1){
		perror("Error de accept()");
		terminar_programa(server_fd, logger);
		exit(EXIT_FAILURE);
	}

	recibir_handshake(cliente_fd, hs_ok, hs_error, protocolo_hs_ok);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	terminar_programa(server_fd, logger);

	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

void terminar_programa (int server_fd, log_t *logger){
	close(server_fd);
	log_destroy(logger);
}

void recibir_handshake(int fd_conexion, int32_t hs_ok, int32_t hs_error, int32_t protocolo_hs_ok){
	size_t bytes;
	int32_t handshake;
	bytes = recv(fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
	if (handshake == protocolo_hs_ok) {
		bytes = send(fd_conexion, &hs_ok, sizeof(int32_t), 0);
	} else {
		bytes = send(fd_conexion, &hs_error, sizeof(int32_t), 0);
	}
}