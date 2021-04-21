
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 4444


int  main()
{
	int sockett, connectt;
	int flaga1 = 1;
	char buffer[1024];
	char buffer1[1024];
	int salod = 0;
	int pieniadze = 0;
	int spr;
	struct sockaddr_in ServerAddress = { }; //wyzerowanie truktury  //przechowuje port i adres ip serwera
	ServerAddress.sin_family = AF_INET; //rodzina adresow ip
	ServerAddress.sin_port = htons( PORT ); //port
	ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//adres ip przeksztavony na zapis rozumiany przez komputer

	sockett = socket(ServerAddress.sin_family, SOCK_STREAM, 0);//utworzenie gniazda klienta
		if (sockett < 0)
		{
			printf("blad w utworzeniu gniazga klienta \n");
		}
	connectt = connect(sockett, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)); //utworzenie polaczenia z gniazdem serwera (automatyczne nadanie nazwy gniazdu)
		if (connectt < 0)
		{
			printf("problem z polaczeniem do serwera \n");
			exit(1);
		}
		printf("Podaj swoj login i haslo \n");
	//while(1)

		do{
		printf("login: \t");
		scanf("%s", buffer);
		send(sockett, buffer, sizeof(buffer), 0);

		printf("haslo: \t");
		scanf("%s", buffer);
		send(sockett, buffer, sizeof(buffer), 0);

		bzero(buffer, sizeof(buffer));
		recv(sockett, buffer, sizeof(buffer), 0);
		//printf("%s\n", buffer);

 	if (strcmp(buffer, "odebraem") == 0)
	{
 		printf("Zalogowano \n");
 		//bzero(buffer, sizeof(buffer));
 		recv(sockett, &salod, sizeof(salod), 0);
 		printf("Saldo: %d\t", salod);
 		flaga1 = 0;
 		//send(sockett, "wplac", 1024, 0);


 		while(1)
 		{
 			bzero(buffer1, sizeof(buffer1));
 			printf("\nPodaj komende: \t");
 			scanf("%s", buffer1);
 			spr = send(sockett, buffer1, sizeof(buffer1), 0);

 			if(((strcmp(buffer1, "wplac") == 0) || (strcmp(buffer1, "wyplac") == 0)) && spr>0)
 				{
 				printf("ile: \t");
 				scanf("%d", &pieniadze);
 				send(sockett, &pieniadze, sizeof(pieniadze), 0);

 					if((strcmp(buffer1, "wyplac") == 0) && (pieniadze>salod))
 					{
 						bzero(buffer, sizeof(buffer));
 						recv(sockett, buffer, sizeof(buffer), 0);
 						printf("%s", buffer);
 					}else{
						spr=recv(sockett, &salod, sizeof(salod), 0);
						printf("Saldo: %d\t", salod);
 					}
 						//flaga1 = 0;
 				}else if((strcmp(buffer1, "wyloguj") == 0) && spr>0){
 					printf("wylogowano \n");
 					break;
 				}else{
 					printf("Zla wiadomosc badz blad w wysylaniu wiadomosci /n");
 				}
 		}

	}else{
		printf("Blad, ponow probe logowania \n");
	}
		}while(flaga1);
		//break;


	return 0;
}
