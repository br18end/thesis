#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT 5353 // numero de puerto para recibir conexiones

int main(int argc, char const *argv[]) {
	struct sockaddr_in serverAddress;  // direccion del servidor
	serverAddress.sin_family = AF_INET; // familia de direcciones de sockets (TCP/IP v4)
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Pasar la direccion a donde se conecta
  serverAddress.sin_port = htons(PORT); // se convierte a  network byte order  
  int cliente = socket(AF_INET, SOCK_STREAM, 0); // se crea socket

  // se solicita conexion con el servidor
  if (connect(cliente, (struct sockaddr *)&serverAddress,
	      sizeof(struct sockaddr)) == -1) {
    perror("Error connect");
    exit(1);
  }
	printf("Estoy conectado\n");

  while(1){
    char mensaje[256];
    scanf("%s", mensaje);

    send(cliente,mensaje,strlen(mensaje),0);
  }
	return 0;
}