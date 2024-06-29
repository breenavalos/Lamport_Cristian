#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define IP "192.168.100.5" // Dirección IP del servidor
#define PUERTO 8080 // Puerto de conexión

// Función para obtener la hora de la máquina en formato HH:MM:SS
void DameHoraMaquina(char *hora) {
    time_t tiempo = time(0); // Obtener el tiempo actual
    struct tm *tlocal = localtime(&tiempo); // Convertir a hora local
    strftime(hora, 128, "%H:%M:%S", tlocal); // Formatear la hora
}

// Función para ajustar la hora del sistema
void set_system_time(const char *new_time) {
    char command[100];
    // Construir el comando para establecer la nueva hora
    sprintf(command, "sudo date --set='%s'", new_time);
    // Ejecutar el comando con permisos de superusuario
    system(command);
}

// Función para ajustar la hora local si la hora remota es mayor
void AjustarHora(char *hora_local, const char *hora_remota) {
    int hora_l, minuto_l, segundo_l;
    int hora_r, minuto_r, segundo_r;

    // Parsear la hora local
    sscanf(hora_local, "%d:%d:%d", &hora_l, &minuto_l, &segundo_l);
    // Parsear la hora remota
    sscanf(hora_remota, "%d:%d:%d", &hora_r, &minuto_r, &segundo_r);

    // Ajustar la hora local si la hora remota es mayor
    if (hora_r > hora_l || (hora_r == hora_l && minuto_r > minuto_l) ||
        (hora_r == hora_l && minuto_r == minuto_l && segundo_r > segundo_l)) {
        // Copiar la hora remota a la hora local
        strcpy(hora_local, hora_remota);
        // Incrementar el segundo
        segundo_l = segundo_r+1;
        minuto_l = minuto_r;
        hora_l = hora_r;
        // Formatear la nueva hora local
        sprintf(hora_local, "%02d:%02d:%02d", hora_l, minuto_l, segundo_l);
        // Llamar a la función para establecer la hora del sistema
        set_system_time(hora_local);
        //printf("Hora local: %s, hora remota: %s\n", hora_local, hora_remota);
    }
}

// Función para enviar y recibir mensajes con ajuste de hora
void EnviaRecibeMensaje(int sock) {
    char buffer[256]; // Buffer para almacenar los mensajes
    char hora_local[128]; // Almacenar la hora local
    char hora_remota[128]; // Almacenar la hora remota

    // Obtener hora de la máquina
    DameHoraMaquina(hora_local); // Obtener la hora local

    // Enviar mensaje con la hora local
    sprintf(buffer, "%s Mensaje:Hola! con hora %s", hora_local, hora_local); // Crear mensaje a enviar
    write(sock, buffer, strlen(buffer)); // Enviar mensaje
    printf("Mensaje enviado: %s\n", buffer); // Mostrar mensaje enviado

    // Recibir respuesta y hora del servidor
    bzero(buffer, 256); // Limpiar el buffer
    read(sock, buffer, 255); // Leer el mensaje del socket
    sscanf(buffer, "%s %s", hora_remota, buffer); // Extraer la hora remota y el mensaje
    printf("Mensaje recibido de B: %s, hora remota: %s\n", buffer, hora_remota); // Mostrar mensaje recibido

    // Ajustar hora local según la hora remota recibida
    AjustarHora(hora_local, hora_remota); // Ajustar la hora local
}

int main(int argc, char *argv[]) {
    struct sockaddr_in c_sock;
    int idsockc;

    idsockc = socket(AF_INET, SOCK_STREAM, 0); // Crear socket
    printf("idsockc %d\n", idsockc);

    c_sock.sin_family = AF_INET; // Tipo de familia del socket (IPv4)
    c_sock.sin_port = htons(PUERTO); // Configurar el puerto
    c_sock.sin_addr.s_addr = inet_addr(IP); // Configurar la dirección IP
    memset(c_sock.sin_zero, 0, 8); // Rellenar con ceros

    // Conectar al servidor
    if (connect(idsockc, (struct sockaddr *)&c_sock, sizeof(c_sock)) < 0) {
        perror("Error de conexión");
        exit(1);
    }

    EnviaRecibeMensaje(idsockc); // Enviar y recibir mensajes
    close(idsockc); // Cerrar el socket

    return 0;
}
