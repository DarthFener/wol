#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int wol(char *nome_pc) {

    char script_path[256];
    snprintf(script_path, sizeof(script_path), "./%s.sh", nome_pc);
    if (access(script_path, X_OK) == 0) {
        int result = system(script_path);
        if (result == -1) {
            perror("Errore nell'esecuzione dello script");
            return -1;
        }
        return 1;
    } else {
        printf("Lo script non esiste\n");
        return -1;
    }
}

void gestisci_post(int client_socket, char *post_data) {
    
        char *nome_pc_start = strstr(post_data, "nomePc=");
        if (nome_pc_start != NULL) {
            nome_pc_start += strlen("nomePc=");
            char *nome_pc_end = strchr(nome_pc_start, '&');
            if (nome_pc_end == NULL) {
                nome_pc_end = nome_pc_start + strlen(nome_pc_start);
            }
            char nome_pc[256];
            strncpy(nome_pc, nome_pc_start, nome_pc_end - nome_pc_start);
            nome_pc[nome_pc_end - nome_pc_start] = '\0';
            int status = wol(nome_pc);
            printf("%d", status);
            if(status == -1)
            {
                const char *response_header = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nErrore: Parametro nomePc non corrispondente a computer noti";
                const char *response_body = nome_pc;
                send(client_socket, response_header, strlen(response_header), 0);
                send(client_socket, response_body, strlen(response_body), 0);
                return;
            }
            const char *response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            const char *response_body = nome_pc;
            send(client_socket, response_header, strlen(response_header), 0);
            send(client_socket, response_body, strlen(response_body), 0);
        } else {
            const char *error_response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nErrore: Parametro nomePc non presente nella richiesta POST";
            send(client_socket, error_response, strlen(error_response), 0);
        }
    
}

int main() {

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Errore nel binding del socket");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 5) == -1) {
        perror("Errore nell'ascolto delle connessioni");
        exit(EXIT_FAILURE);
    }
    printf("Server in ascolto sulla porta %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Errore nell'accettare la connessione");
            continue;
        }
        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Richiesta ricevuta:\n%s\n", buffer);
            if (strstr(buffer, "POST /wol HTTP/1.1") != NULL) {
                char *post_data_start = strstr(buffer, "\r\n\r\n");
                if (post_data_start != NULL) {
                    post_data_start += strlen("\r\n\r\n");
                    gestisci_post(client_socket, post_data_start);
                } else {
                    const char *error_response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nErrore: Richiesta POST malformata";
                    send(client_socket, error_response, strlen(error_response), 0);
                }
            } else {
                const char *error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nErrore: Endpoint non valido";
                send(client_socket, error_response, strlen(error_response), 0);
            }
        }
        close(client_socket);
    }
    close(server_socket);
    return 0;
}
