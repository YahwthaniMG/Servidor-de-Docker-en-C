#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>

#define PORT 6666
#define BUFFER_SIZE 6144
#define MAX_CLIENTS 10

void cifrado_xor(char *cadena, char *clave, int long_cadena, int long_clave) {
	for (int pos = 0; pos < long_cadena; pos++) {
		cadena[pos] = cadena[pos] ^ clave[pos % long_clave];	
	}
}

void xor_cifrado(char *palabra, char *clave) {
    int palabra_len = strlen(palabra);
    int clave_len = strlen(clave);
    
    for (int i = 0; i < palabra_len; i++) {
        palabra[i] ^= clave[i % clave_len];
    }
}

int hex_to_int(char c) {
    if (c >= 97)
        c = c - 32;
    int first = c / 16 - 3;
    int second = c % 16;
    int result = first * 10 + second;
    if (result > 9) result--;
    return result;
}

void convertToHex(const char *str, char *hexOutput) {
    while (*str) {
        sprintf(hexOutput, "%02x", (unsigned char)*str);
        str++;
        hexOutput += 2; 
    }
}

int hex_to_ascii(char *hex_string, char *ascii_string) {
    int length = strlen(hex_string);
    int i;
    char buf = 0;
    int ascii_index = 0;
    for (i = 0; i < length; i++) {
        if (i % 2 != 0) {
            ascii_string[ascii_index++] = hex_to_int(buf) * 16 + hex_to_int(hex_string[i]);
        } else {
            buf = hex_string[i];
        }
    }
    ascii_string[ascii_index] = '\0'; // Null-terminate the ASCII string
    return ascii_index; // Return the length of the ASCII string
}

int main() {
    char clave[] = "DesBunduquia";
    int long_clave = strlen(clave);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    int sockfd, len, num_clients = 0;
    printf("Escuchando en el puerto número: %d\n", PORT);
    
    // Crear socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("Fallo en la creación del socket");
        exit(EXIT_FAILURE);
    }
    
    // Configurar dirección del servidor
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    
    // Enlazar dirección del servidor al socket
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Fallo en el enlace del socket");
        exit(EXIT_FAILURE);
    }
    
    // Obtener e imprimir la dirección IP en la que está escuchando
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(servaddr.sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("Escuchando en la IP: %s\n", ip_str);
    
    while (1) {
        // Recibir nombre del archivo del cliente
        len = sizeof(cliaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, (socklen_t *)&len);
        if (n < 0) {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        printf("Recibiendo del cliente: %s\n", buffer);
        char ascii_string[strlen(buffer) / 2 + 1]; // +1 for null-terminator
        hex_to_ascii(buffer, ascii_string);
        printf("Hexadecimal string: %s\n", buffer);
        int long_cadena = strlen(ascii_string);
        cifrado_xor(ascii_string, clave, long_cadena, long_clave);
        printf("%s\n",ascii_string);
        
        // Crear un proceso hijo para atender al cliente
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            long file_size;
            size_t line_size = 0;
            size_t total_size = 0;
            char *line = NULL;
            // Código del proceso hijo
            FILE *file = fopen(ascii_string, "r");
            if (file == NULL) {
                perror("Error al abrir el archivo");
                exit(EXIT_FAILURE);
            }
            char text[BUFFER_SIZE];
            while (fgets(text, BUFFER_SIZE, file) != NULL) {
                int palabra_len = strlen(text);
                xor_cifrado(text, clave);
                // Calcular el tamaño necesario para la cadena hexadecimal
                int hex_len = palabra_len * 2 + 1; // Cada byte se convierte en 2 caracteres hexadecimales, más 1 para el terminador nulo
                char *hex_output = malloc(hex_len);
                if (hex_output == NULL) {
                    fprintf(stderr, "Error al asignar memoria\n");
                    return 1;
                }
                // Rellenar la cadena hexadecimal
                for (int i = 0; i < palabra_len; i++) {
                    sprintf(hex_output + i * 2, "%02X", (unsigned char)text[i]);
                }
                sendto(sockfd, hex_output, strlen(hex_output), 0, (struct sockaddr *)&cliaddr, sizeof(struct sockaddr_in));
            }
            char f[10]="FIN";
            int palabra_len = strlen(f);
            xor_cifrado(f, clave);
            printf("XOR %sFin\n", f);
            // Calcular el tamaño necesario para la cadena hexadecimal
            int hex_len = palabra_len * 2 + 1; // Cada byte se convierte en 2 caracteres hexadecimales, más 1 para el terminador nulo
            char *hex_output = malloc(hex_len);
            if (hex_output == NULL) {
                fprintf(stderr, "Error al asignar memoria\n");
                return 1;
            }
            // Rellenar la cadena hexadecimal
            for (int i = 0; i < palabra_len; i++) {
                sprintf(hex_output + i * 2, "%02X", (unsigned char)f[i]);
            }
            sendto(sockfd, hex_output, strlen(hex_output), 0, (struct sockaddr *)&cliaddr, sizeof(struct sockaddr_in));
            printf("Archivo enviado al cliente.\n");
            fclose(file);
            exit(0); // Salir del proceso hijo
        } else {
            // Código del proceso padre
            num_clients++;
            if (num_clients >= MAX_CLIENTS) {
                // Esperar a que un proceso hijo termine
                int status;
                waitpid(-1, &status, 0);
                num_clients--;
            }
        }
    }
    
    close(sockfd);
    return 0;
}
