#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 6666
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[BUFFER_SIZE];
    time_t current_time;
    struct tm *local_time;

    // Crear un socket TCP/IP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error al abrir el socket");
    }

    // Configurar la dirección del servidor
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Enlazar el socket con la dirección del servidor
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error en la operación de bind");
    }

    // Escuchar conexiones entrantes
    listen(sockfd, 5);
    printf("Servidor de tiempo escuchando en el puerto %d\n", PORT);

    clilen = sizeof(cli_addr);
    while (1) {
        // Aceptar una conexión entrante
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            error("Error en la operación de accept");
        }

        printf("Conexión establecida con el cliente: %s\n", inet_ntoa(cli_addr.sin_addr));

        // Leer la solicitud del cliente
        bzero(buffer, BUFFER_SIZE);
        int n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            error("Error al leer del socket");
        }
        
        printf("Solicitud recibida: %s\n", buffer);

        // Obtener la hora actual
        current_time = time(NULL);
        if (current_time == ((time_t)-1)) {
            error("Error al obtener la hora actual");
        }

        // Convertir la hora actual a tiempo local
        local_time = localtime(&current_time);
        if (local_time == NULL) {
            error("Error al convertir la hora a tiempo local");
        }

        // Formatear la hora actual en una cadena legible
        char time_str[BUFFER_SIZE];
        strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", local_time);

        // Mostrar la hora actual en el servidor
        printf("Hora actual del servidor: %s\n", time_str);


        // Enviar la hora actual al cliente
        snprintf(buffer, BUFFER_SIZE, "%ld", current_time);
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) {
            error("Error al escribir en el socket");
        }

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
