FROM gcc:latest

# Copiar los archivos fuente al contenedor
COPY . /app/

# Compilar el programa tcpserver.c
WORKDIR /app
RUN gcc -o tcpserver tcpserver.c -lpthread

# Exponer el puerto 5556
EXPOSE 5556

# Ejecutar el servidor al iniciar el contenedor
CMD ["./tcpserver"]
