#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

struct Site	{ //struct that hold site infomation

	char host[5120];
	char info[5120];
};

int main (int argc, char **argv)	{

	int sockfd ,n;	// variables for making the socket
	int len = sizeof(struct sockaddr);
	char recvline[50000] = {0};	//holds response from the server
	char url[2000];
        char port[2000];
	char *sendx = "GET / HTTP/1.1\r\nHost: ";
	char *sendy = "\r\nConnection: Close\r\n\r\n";
	char *send = "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: Close\r\n\r\n";
	char sendz[2000];
	struct Site sites[100];	//creats 100 site objects
	int sitePos = 0;
	struct hostent *host;
	struct in_addr addr;
	struct sockaddr_in servaddr;

	FILE *in ,*inf;
	char line[512];
	int count = 0;
	int scount = 0;

        inf = fopen("list.txt","a+");	//opens file if it doesnt exist

	fclose(inf);

	in = fopen("list.txt","r");	//opens list file for reading

	while(fgets(line,sizeof(line),in) && count < 5 && strlen(line) > 1)	{	//gets contents of the list file

		int sp = 0;

		char *linepr = strstr(line," ");	//searches each line for white space dellimiter

		sp = linepr - line;
		char urlf[512];
		char infof[512];
		strncpy(urlf,line,sp);
                strncpy(infof,line + sp + 1 ,strlen(line) - sp - 2);
	        strcpy(sites[scount].host,urlf);	//puts file contents into Site objects
        	strcpy(sites[scount].info,infof);

                bzero(line,512);

                bzero(urlf,512);

		*linepr = 0;

		scount++;

		count++;
	}

	fclose(in);

	while(1)	{

		for(int i = 0; i < sizeof(recvline); i++)	{

			recvline[i] = '\0';
		}

	        printf("URL: ");

        	scanf("%s",url);	//gets user url

		if(strstr(url,"quit"))	{

			printf("Cya\n");
			return 0;
		}

	        printf("Port: ");

	        scanf("%s",port);	//gets user port

        	int p = atoi(port);

		sockfd = socket(AF_INET,SOCK_STREAM,0);	//creates tcp client socket
		bzero(&servaddr,sizeof(servaddr));

		servaddr.sin_family = AF_INET;

		servaddr.sin_port = htons(p);

		host = gethostbyname(url);	//resolves hostname

		if(host == NULL)	{

			printf("Action: Not cached\n");
			continue;
		}

	        servaddr.sin_addr.s_addr = *(u_long *) host->h_addr_list[0];	//gets host name ip address

		int w = connect(sockfd,(struct sockaddr * )&servaddr,sizeof(servaddr));		//connects to server

		if( w < 0 )	{printf("error"); return 0;}

		snprintf(sendz,sizeof(sendz), "%s%s%s" , sendx,url,sendy);

		int y = write(sockfd,sendz,strlen(sendz));	//writes get rewuest to tserver

	        sleep(1);

		int z = read(sockfd, recvline,50000);	//rerad server response

		int dateP = 0;
		char *date = strstr(recvline,"Date: ");	//	grabs server message date
		dateP = date - recvline;

		char datex[512];
		strncpy(datex,recvline + dateP + 8, 30);

	        int reP = 0;
        	char *rePx = strstr(recvline,"HTTP/");
        	reP = rePx - recvline;

		char response[512];

		strncpy(response, recvline + reP + 9, 3);

        	printf("Response: %s\n",response); // print the respond code from server

		int pos = 0;
                int space1 = 0;
                int space2 = 0;

		char day[512];

                char month[512];

                char year[512];

                char hour[512];

                char min[512];

                char sec[512];

		while(pos < 4)	{	//	break apart server respond code

		        char *spacex = strstr(datex + space1 ," ");

			space1 = spacex - datex;

                        char *spacey = strstr(datex + space1 + 1," ");
        	        space2 = spacey - datex;

			char w[512];

			bzero(w,512);

		        strncpy(w,datex + space1 + 1 , space2 - space1 - 1);
			space1 = space2;

			if(pos == 0 )	{

				strcpy(day,w);
			}

                	if(pos == 1 )   {

                        	strcpy(month,w);
                	}

                	if(pos == 2 )   {

                        	strcpy(year,w);
                	}

                	if(pos == 3 )   {

                        	strncpy(hour,w,2);
                        	strncpy(min,w+3,2);
                        	strncpy(sec,w+6,2);
                	}

			pos++;
		}


		if(strcmp(month,"Feb") == 0)	{

				strcpy(month,"02");
		}

        	if(strcmp(month,"Mar") == 0)    {

                	        strcpy(month,"02");
        	}

        	if(strcmp(month,"Apr") == 0)    {

                	        strcpy(month,"02");
        	}

	        char action[200];
		char info[200];

		int found = 0;

		for(int i = 0; i < 99; i++)     {

                	if(sites[i].host != NULL)       {

                        	if(strcmp(sites[i].host,url) == 0)	{

					found = 1;

					printf("Action: Page found in cache, filename – %s\n",sites[i].info);
				}
                	}
        	}

		if(found == 0 && strcmp(response,"200") == 0)	{

	        	snprintf(action,sizeof(action), "Action: Page cached as filename - %s%s%s%s%s%s\n" , year,month,day,hour,min,sec);

        		snprintf(info,sizeof(info), "%s%s%s%s%s%s" , year,month,day,hour,min,sec);

	        	printf("%s",action); // print the received text from server

			FILE *htmlf = fopen(info,"w");

        		fprintf(htmlf, "%s\n", recvline);

			fclose(htmlf);


			if(scount < 5)	{

				scount++;

				strcpy(sites[scount].host,url);
        			strcpy(sites[scount].info,info);
			}

			else{

				int min = 9999999;

				int place = 0;

                                char hms[512];
				for(int i = 0; i < 100; i++)	{

					if(sites[i].host != NULL && strlen(sites[i].host) > 5)  {

						bzero(hms,512);

						strncpy(hms,sites[i].info + 8, 6);

						if(atoi(hms) < min)	{

							min = atoi(hms);

							place = i;
						}
					}
				}

			remove(sites[place].info);

	        	strcpy(sites[place].host,url);

	        	strcpy(sites[place].info,info);
		}

		FILE *out = fopen("list.txt","w");

		for(int i = 0; i < 100; i++)	{

			if(sites[i].host != NULL && strlen(sites[i].host) > 5)	{

				fprintf(out, "%s %s\n", sites[i].host,sites[i].info);
			}
		}
		sitePos++;

		fclose(out);
	}

	if(found == 0 && strcmp(response,"200") != 0){

		printf("Action: Not cached\n");
	}

	}

	return 0;
}
