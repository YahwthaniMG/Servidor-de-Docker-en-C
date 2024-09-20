#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

#define PUERTO 5556
#define MAX_MSG_SIZE 2048
#define MAX_CLIENTS 10



int sd;
struct sockaddr_in sind, pin;
char sigue = 'S';
int num_lobbys = 0;
// Mutex para proteger la variable global
pthread_mutex_t mutex;

void aborta_handler(int sig) {
    printf("....abortando el proceso servidor %d\n", sig);
    close(sd);
    exit(1);
}


void ActivityLog(char *ins, char *hour, char *date, char *user){
    FILE *archivo = fopen("ActivityLog.txt", "a"); 
        if (archivo == NULL) {
            perror("Error al abrir el archivo");
            exit(1);
        }
        fprintf(archivo, "\n");
        fprintf(archivo, "%s %s %s %s", ins,hour,date,user);
        fclose(archivo);
}

void LobbyLog(char *ins, char *nombre, char *passwd){
    FILE *archivo = fopen("LobbysLog.txt", "a"); 
        if (archivo == NULL) {
            perror("Error al abrir el archivo");
            exit(1);
        }
        fprintf(archivo, "\n");
        fprintf(archivo, "%s %s %s", ins,nombre,passwd);
        fclose(archivo);
}

int Digit(char *numero){
    if (isdigit(*numero)){
        int num = *numero - '0';
        return num;
    }
    else{
        return num_lobbys;
    }
}

int UserDate( char *ins, char *userins, char *passwordins) {
    FILE *archivo;
    archivo = fopen("UserData.txt", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }
    char linea[MAX_MSG_SIZE];
    int encontrado = 0; 
    while (fgets(linea, MAX_MSG_SIZE, archivo) != NULL) {
        linea[strcspn(linea, "\n")] = 0;
        char *user = NULL;
        char *password = NULL;
        if (linea != NULL && linea[0] != '\0') {
            char *temp = strdup(linea);
            char *token = strtok(linea, " ");
            if (token != NULL) {
                user = strdup(token);
                token = strtok(NULL, " ");
                if (token != NULL) {
                    password = strdup(token); 
                }
            }
        }
        if (strcmp(user,userins)==0){
            encontrado+=2;
            if (strcmp(ins,"SignIn")==0){
                encontrado=76;
                fclose(archivo);
                return encontrado;
            }
        }
        if (strcmp(password,passwordins)==0){
            encontrado+=3;
            if (strcmp(ins,"SignIn")==0){
                encontrado=87;
                fclose(archivo);
                return encontrado;
            }
        }
        if (encontrado==5){
            if (strcmp(ins,"LogIn")==0){
                fclose(archivo);
                return encontrado;
            }
        }else{
            encontrado=0;
        }
    }
    fclose(archivo);
    if(strcmp(ins,"SignIn")==0){
        FILE *archivo = fopen("UserData.txt", "a"); 
        if (archivo == NULL) {
            perror("Error al abrir el archivo");
            exit(1);
        }
        fprintf(archivo, "\n");
        fprintf(archivo, "%s %s", userins, passwordins);
        fclose(archivo);
        encontrado=5;
        return encontrado;
    }
    return encontrado;
}

void xor_cifrado(char *palabra, char *clave, int palabra_len) {
    int clave_len = strlen(clave);
    for (int i = 0; i < palabra_len; i++) {
        palabra[i] ^= clave[i % clave_len];
    }
}

void hex_to_string(const char *hex, char *output, int output_len) {
    for (int i = 0; i < output_len; i++) {
        sscanf(hex + 2 * i, "%2hhx", &output[i]);
    }
}

void string_to_hex(const char *input, char *hex_output, int input_len) {
    for (int i = 0; i < input_len; i++) {
        sprintf(hex_output + i * 2, "%02X", (unsigned char)input[i]);
    }
    hex_output[input_len * 2] = '\0'; // Ensure null-termination
}

void *handle_client(void *arg) {
    int sd_actual = *(int *)arg;
    char *msg = malloc(MAX_MSG_SIZE * sizeof(char));
    char *json = malloc(MAX_MSG_SIZE * sizeof(char));
    char clave[] = "DesBunduquia";
    int long_clave = strlen(clave);

    memset(msg, 0, MAX_MSG_SIZE);
    memset(json, 0, MAX_MSG_SIZE);

    int n = recv(sd_actual, msg, MAX_MSG_SIZE - 1, 0);
    if (n == -1) {
        perror("recv");
        exit(1);
    }

    msg[n] = '\0';
    
    // Calcula la longitud de la cadena cifrada
    int hex_len = strlen(msg);
    int palabra_len = hex_len / 2;

    char *palabra_cifrada = malloc(palabra_len + 1);
    if (palabra_cifrada == NULL) {
        fprintf(stderr, "Error al asignar memoria\n");
        free(msg);
        close(sd_actual);
        return NULL;
    }

    // Convertir el mensaje hexadecimal de vuelta a la cadena cifrada
    hex_to_string(msg, palabra_cifrada, palabra_len);
    palabra_cifrada[palabra_len] = '\0';

    // Aplicar XOR de nuevo para descifrar
    xor_cifrado(palabra_cifrada, clave, palabra_len);

    // Imprimir la oración original
    printf("Descifrado: %s\n", palabra_cifrada);

    
    char *instruction = NULL;
    char *user = NULL;
    char *password = NULL;
    char *nameuser=NULL;
    if (strcmp(palabra_cifrada,"")==0) {
    }
    else{
        char *token = strtok(palabra_cifrada, " ");
        if (token != NULL) {
            instruction = strdup(token);
            token = strtok(NULL, " ");
            if (token != NULL) {
                user = strdup(token); 
                token = strtok(NULL, " ");
                if (token != NULL) {
                    password = strdup(token);
                    if (instruction=="Activity"){
                        token= strtok(NULL, " ");
                        if (token !=NULL){
                        nameuser=strdup(token);
                        }
                    }
                }
            }
        }
    }
    if (instruction != NULL){
        if (strcmp(instruction, "LogIn") == 0) {
            int ans1=UserDate(instruction, user, password);
            char strNumero[20];
            sprintf(strNumero, "%d", ans1);
            int respuesta_len = strlen(strNumero);
             // Cifrar la respuesta usando XOR
            xor_cifrado(strNumero, clave, respuesta_len);
            // Convertir la respuesta cifrada a hexadecimal
            char respuesta_hex[respuesta_len * 2 + 1];
            string_to_hex(strNumero, respuesta_hex, respuesta_len);
            snprintf(json, MAX_MSG_SIZE, "{'%s':'%s'}", respuesta_hex,respuesta_hex);
        }
        else if(strcmp(instruction, "SignIn") == 0){
            int ans=UserDate(instruction, user, password);
            char strNumero[20];
            sprintf(strNumero, "%d", ans);
            int respuesta_len = strlen(strNumero);
             // Cifrar la respuesta usando XOR
            xor_cifrado(strNumero, clave, respuesta_len);
            // Convertir la respuesta cifrada a hexadecimal
            char respuesta_hex[respuesta_len * 2 + 1];
            string_to_hex(strNumero, respuesta_hex, respuesta_len);
            snprintf(json, MAX_MSG_SIZE, "{'%s':'%s'}", respuesta_hex,respuesta_hex);
        }else if(strcmp(instruction, "Activity")==0){
            ActivityLog(instruction,user, password, nameuser);
            char strNumero[20] = "Vacio";
            int respuesta_len = strlen(strNumero);
             // Cifrar la respuesta usando XOR
            xor_cifrado(strNumero, clave, respuesta_len);
            // Convertir la respuesta cifrada a hexadecimal
            char respuesta_hex[respuesta_len * 2 + 1];
            string_to_hex(strNumero, respuesta_hex, respuesta_len);
            snprintf(json, MAX_MSG_SIZE, "{'%s':'%s'}", respuesta_hex,respuesta_hex);
        }else if(strcmp(instruction, "Lobbys") == 0){
            LobbyLog(instruction, user, password);
            char strNumero[20];
            num_lobbys+=1;
            sprintf(strNumero, "%d", num_lobbys);
            int respuesta_len = strlen(strNumero);
             // Cifrar la respuesta usando XOR
            xor_cifrado(strNumero, clave, respuesta_len);
            // Convertir la respuesta cifrada a hexadecimal
            char respuesta_hex[respuesta_len * 2 + 1];
            string_to_hex(strNumero, respuesta_hex, respuesta_len);
            snprintf(json, MAX_MSG_SIZE, "{'%s':'%s'}", respuesta_hex,respuesta_hex);
        }else if(strcmp(instruction, "Actualizacion") == 0){
            int num = Digit(password);
            // Bloquear el mutex antes de modificar la variable global
            pthread_mutex_lock(&mutex);
            num_lobbys = num;
            printf("Hilo %ld incrementó num_lobbys a %d\n", (long)arg, num_lobbys);
            // Desbloquear el mutex después de modificar la variable global
            pthread_mutex_unlock(&mutex);
            char strNumero[20];
            sprintf(strNumero, "%d", num_lobbys);
            int respuesta_len = strlen(strNumero);
             // Cifrar la respuesta usando XOR
            xor_cifrado(strNumero, clave, respuesta_len);
            // Convertir la respuesta cifrada a hexadecimal
            char respuesta_hex[respuesta_len * 2 + 1];
            string_to_hex(strNumero, respuesta_hex, respuesta_len);
            snprintf(json, MAX_MSG_SIZE, "{'%s':'%s'}", respuesta_hex,respuesta_hex);
        }else{
            char strNumero[20] = "Vacio";
            int respuesta_len = strlen(strNumero);
             // Cifrar la respuesta usando XOR
            xor_cifrado(strNumero, clave, respuesta_len);
            // Convertir la respuesta cifrada a hexadecimal
            char respuesta_hex[respuesta_len * 2 + 1];
            string_to_hex(strNumero, respuesta_hex, respuesta_len);
            snprintf(json, MAX_MSG_SIZE, "{'%s':'%s'}", respuesta_hex,respuesta_hex);
        }
    }
    
    int sent;
    if ((sent = send(sd_actual, json, strlen(json), 0)) == -1) {
        perror("send");
        exit(1);
    }

    close(sd_actual);
    free(msg);
    free(json);
    free(instruction);
    free(user);
    free(password);
    free(nameuser);
    pthread_exit(NULL);
}


int main() {
    // Inicializar el mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Error al inicializar el mutex\n");
        return 1;
    }
    
    if (signal(SIGINT, aborta_handler) == SIG_ERR) {
        perror("Could not set signal handler");
        return 1;
    }

    sind.sin_family = AF_INET;
    sind.sin_addr.s_addr = INADDR_ANY;
    sind.sin_port = htons(PUERTO);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (bind(sd, (struct sockaddr *)&sind, sizeof(sind)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sd, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(1);
    }

    while (sigue == 'S') {
        int addrlen;
        int sd_actual;
        pthread_t thread;

        if ((sd_actual = accept(sd, (struct sockaddr *)&pin, &addrlen)) == -1) {
            perror("accept");
            exit(1);
        }

        if (pthread_create(&thread, NULL, handle_client, &sd_actual) != 0) {
            perror("pthread_create");
            close(sd_actual);
        }
    }

    close(sd);
    printf("\nConexion cerrada\n");
    // Destruir el mutex
    pthread_mutex_destroy(&mutex);
    return 0;
}
