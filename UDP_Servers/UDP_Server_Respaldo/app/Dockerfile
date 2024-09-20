FROM gcc:latest

# Copiar los archivos fuente al contenedor
COPY . /app/

# Compilar el programa udpserver.c
WORKDIR /app
RUN gcc udpserver.c -lnsl -o udpserver

# Exponer el puerto UDP
EXPOSE 6667/udp

# Ejecutar el servidor al iniciar el contenedor
CMD ["./udpserver"] 