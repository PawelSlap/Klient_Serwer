
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 4444

void CZAS(char czas[50]);

int  main()
{
	//zmienne obslugujace komunikacje klient serwer
	int sockett, bindd, newsocket;
	//char buffer[1024];
	char login[1024];
	char haslo[1024];
	char buffer[1024];
	char czas[50];

	char	bufforLogin[1024];
	char	bufforHaslo[1024];
	int 	bufforSaldo, pieniadze;

	int flaga1 = 1;
	int spr;

	char sciezka_log[54]= "/home/student/eclipse-workspace/serwer/src/LOG";


	//zmienna nowego proecesu
	pid_t childpid;


	FILE *plik;
	FILE *plik_log;

	//zmienna przechowujaca sciezki do plikow
	char sciezki[2][54]={
	{"/home/student/eclipse-workspace/serwer/src/U/User1"},
	{"/home/student/eclipse-workspace/serwer/src/U/User2"}
};
	//char (*wsk)[100];
	char s[54];


	struct sockaddr_in KlientAddres;
	struct sockaddr_in ServerAddress = { }; //wyzerowanie truktury  //przechowuje port i adres ip serwera
	ServerAddress.sin_family = AF_INET; //rodzina adresow ip
	ServerAddress.sin_port = htons( PORT ); //port
	ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//adres ip przeksztavony na zapis rozumiany przez komputer



	socklen_t rozmiar = sizeof(KlientAddres);

	sockett = socket(ServerAddress.sin_family, SOCK_STREAM, 0);//utworzenie gniazda, zwraca deskyptor
			if (sockett < 0)
			{
				printf("blad w utworzeniu gniazga  \n");
			}
	bindd = bind(sockett, (const struct sockaddr *)&ServerAddress, sizeof(ServerAddress));//przypisanie adresu do utworzonego dniazda
		if (bindd <0)
		{
			printf("blad w przypisaniu adresu \n");
			exit(1);
		}
		if (listen(sockett, 5) == 0) //okresla kolejke zgloszen klientow  do serwera 5-maksymalna liczba polaczonych klientow
		{
			printf("nasluchiwanie,,,,,,\n");
		}else{
			printf("blad");
		}
			while(1)
			{
				newsocket = accept(sockett,(struct sockaddr * restrict)&KlientAddres, &rozmiar); //realizuje polaczenie z klentem (tworzy nowe gniazdo)
				if(newsocket < 0)
				{
					exit(1);
				}
				printf("polaczono z  %s;%d\n", inet_ntoa(KlientAddres.sin_addr),ntohs(KlientAddres.sin_port));
			if ((childpid = fork()) == 0) //utowrzenie nowego procesu
			{
					close(sockett);
					do
					{
						bzero(login, sizeof(login));
						recv(newsocket, login, sizeof(login),0);
						//printf("%s\n", login);

						bzero(haslo, sizeof(haslo));
						recv(newsocket, haslo, sizeof(haslo),0);
						//printf("%s\n", haslo);
						int j, i;

						for(i=0; i<2; i++)
							{
							//bzero(buffer, sizeof(buffer));
							for(j=0; j<54; j++)
							{
								s[j]= sciezki[i][j];
							}

							plik = fopen(s, "r+");
							bzero(bufforLogin, sizeof(bufforLogin));
							bzero(bufforHaslo, sizeof(bufforHaslo));
							bufforSaldo = 0;
							fscanf(plik, "%s %s %d", bufforLogin, bufforHaslo, &bufforSaldo);
							fclose(plik);

							if(strcmp(login, bufforLogin) == 0 && strcmp(haslo, bufforHaslo) == 0)
							{
									flaga1 =0;
									send(newsocket, "odebraem", 1024, 0);


									CZAS(czas);
									plik_log=fopen(sciezka_log, "a");
									fprintf(plik_log,"%s Zalogowano: %s; %s; %d; \n", czas, bufforLogin, inet_ntoa(KlientAddres.sin_addr),ntohs(KlientAddres.sin_port));
									fclose(plik_log);

									send(newsocket, &bufforSaldo , sizeof(bufforSaldo), 0);
									while(1)
									{
										printf("... \n");
										bzero(buffer, sizeof(buffer));
										recv(newsocket, buffer, sizeof(buffer),0);

										if (strcmp(buffer, "wplac") == 0)
										{
											recv(newsocket, &pieniadze, sizeof(pieniadze),0);
											plik=fopen(s, "r+");
											fscanf(plik, "%s %s %d", bufforLogin, bufforHaslo, &bufforSaldo);
											fclose(plik);
											printf("%d\n", bufforSaldo);

											bufforSaldo=bufforSaldo+pieniadze;

											plik=fopen(s, "w+");
											spr = fprintf(plik, "%s %s %d", bufforLogin, bufforHaslo, bufforSaldo);
											send(newsocket, &bufforSaldo , sizeof(bufforSaldo), 0);
											printf("%d\n", bufforSaldo);
											fclose(plik);

											CZAS(czas);
											plik_log=fopen(sciezka_log, "a");
											fprintf(plik_log,"%s %s; %s; %d; Wpacono: %d Saldo: %d\n",czas , bufforLogin, inet_ntoa(KlientAddres.sin_addr),ntohs(KlientAddres.sin_port), pieniadze, bufforSaldo);
											fclose(plik_log);

											//break;
										}else if(strcmp(buffer, "wyplac") == 0)
										{
											recv(newsocket, &pieniadze, sizeof(pieniadze),0);

											plik=fopen(s, "r+");
											fscanf(plik, "%s %s %d", bufforLogin, bufforHaslo, &bufforSaldo);
											fclose(plik);
											printf("%d\n", bufforSaldo);

											if (pieniadze>bufforSaldo)
													{
											send(newsocket, "Nie masz tyle pieniedzy!", 1024, 0);
													}else{
											bufforSaldo=bufforSaldo-pieniadze;

											plik=fopen(s, "w+");
											spr=fprintf(plik, "%s %s %d", bufforLogin, bufforHaslo, bufforSaldo);
											fclose(plik);

											send(newsocket, &bufforSaldo , sizeof(bufforSaldo), 0);
											printf("%d\n", bufforSaldo);

											CZAS(czas);
											plik_log=fopen(sciezka_log, "a");
											fprintf(plik_log,"%s %s; %s; %d; Wyacono: %d Saldo: %d\n",czas , bufforLogin, inet_ntoa(KlientAddres.sin_addr),ntohs(KlientAddres.sin_port), pieniadze, bufforSaldo);
											fclose(plik_log);
													}

										}else if(strcmp(buffer, "wyloguj") == 0){
											printf("Rozlaczono  %s;%d\n", inet_ntoa(KlientAddres.sin_addr),ntohs(KlientAddres.sin_port));

											break;

											CZAS(czas);
											plik_log=fopen(sciezka_log, "a");
											fprintf(plik_log,"%s Wylogowano: %s; %s; %d; \n", czas, bufforLogin, inet_ntoa(KlientAddres.sin_addr),ntohs(KlientAddres.sin_port));
											fclose(plik_log);
										}

									}

							}
							//fclose(plik);

							}
						if (flaga1 == 1)
						{
							send(newsocket, "nieodebraem", 1024, 0);
						}

					}while(flaga1);

				}
			}
		close(newsocket);


	return 0;
}

void CZAS(char czas[50])
{
	bzero(czas, 50);
	time_t sekundy = time(NULL);
	struct tm *aktualny_czas;
	sekundy=time(NULL);
	aktualny_czas = localtime(&sekundy);
	strftime(czas,1000,"%Y-%m-%d %H:%M:%S", aktualny_czas);
}

