#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>
#include<signal.h>
#include <stdio_ext.h>



struct AcceptSocket{
    int acceptSocketFD;
    struct sockaddr_in address;
};

#define serverPort 1999
#define ip_address "192.168.76.228"
char check_ip[10][30];
char Total_ips[10][30]={"192.168.76.228","192.168.76.108","192.168.76.143","192.168.76.117"};
int comunication_fd[10];
int count=0;
int flag=0;

//Function Declaration
struct sockaddr_in *createIPv4Address(const char *, int);
struct AcceptSocket *acceptIncomingConnection(int serverSocketFD);
void *receiveAndPrint(void *socketFD);
void startAcceptingIncomingConnections(int serverSocketFD);
void *sender(void *);
void add_connection();
void Communication_thread();
void signal_handler(int num);
void send_all();


void server(void *ptr){
    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0); //(Address Family I NET -> IPV4),(asking for TCP socket),(layer) it will return socket file descriptor 

    struct sockaddr_in *serverAddress = createIPv4Address(ip_address, serverPort);

    int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));
    
    if(result == -1){
        perror("Socket bind Fail:");
        exit(1);
    }
    free(serverAddress);
    printf("(+) Server Ready To Listen\n");
    int listenResult = listen(serverSocketFD, 10); //10 -> number of connections

    startAcceptingIncomingConnections(serverSocketFD);
    
    shutdown(serverSocketFD, SHUT_RD);
}

void client(void *ptr){
    
    sleep(1);
    int choice;
    while(1){
   
    
    printf("\n------------------------------------------------------------------------->");
    printf("\n1 : Add_connection \n2 : Communicate\n3 : Send All\n4 : ctrl+c to Terminate");
    printf("\n------------------------------------------------------------------------->");
    printf("\nEnter Your Choice : ");
    scanf("%d",&choice);
	__fpurge(stdin);
    switch (choice)
    {
    case 1: add_connection();
        break;
    case 2: Communication_thread();
        break;
    case 3 : send_all();
        break;
    		
    case 4 : break;
    
    default:
        break;
    }
    }
}

int main(){
    pthread_t server_thread, client_thread;

    signal(SIGINT,signal_handler);

    pthread_create(&server_thread, NULL, (void *)&server, NULL);
    pthread_create(&client_thread, NULL, (void *)&client, NULL);

    pthread_join(server_thread, NULL);
    pthread_join(client_thread, NULL);

    return 0;
}

struct sockaddr_in *createIPv4Address(const char *ip, int port){
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port); // Convert the port to network byte order
  

    if(strlen(ip) == 0){
        address->sin_addr.s_addr = INADDR_ANY;
    }
    else{
        inet_pton(AF_INET, ip, &(address->sin_addr));
    }

    return address;
}

struct AcceptSocket *acceptIncomingConnection(int serverSocketFD){
    struct sockaddr_in clientAddress;
    struct AcceptSocket *acceptSocket;
    socklen_t clientAddSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, (socklen_t*)&clientAddSize);

    printf("\nclient connected at ip %s : port %d\n",inet_ntoa(clientAddress.sin_addr),ntohs(clientAddress.sin_port));

    strcpy(*(check_ip + count),inet_ntoa(clientAddress.sin_addr)); 
    comunication_fd[count] = clientSocketFD;
    write(clientSocketFD,"Connection Established", 100);
    count++;
    acceptSocket = malloc(sizeof(struct AcceptSocket));
    acceptSocket->address = clientAddress;
    acceptSocket->acceptSocketFD = clientSocketFD;

    return acceptSocket;
}

void *receiveAndPrint(void *socketFD){  //server receiving thread
    int clientFD = *(int *)socketFD,res;
    char buf[1024];
    while(true){
        ssize_t amountReceive = recv(clientFD, buf, 1024, 0);
           
        if(memcmp(buf,"AAAA",4)==0){
        	flag++;
        }
        if(memcmp(buf,"BBBB",4)==0){
        	flag++;
        }
        if(memcmp(buf,"CCCC",4)==0){
        	flag++;
        }
        if(memcmp(buf,"DDDD",4)==0){
        	flag++;
        }
        if(flag == 3){
        	printf("\n All Nodes Data Valid \n");
        	flag=0;
        }
        if(amountReceive > 0){
            printf("%s", buf);
            memset(buf, 0, sizeof(buf));
        }
        if(amountReceive == 0){
      
            for(int i=0;i<count;i++){
                if(comunication_fd[i] == clientFD){
                    for(int j=i;j<count;j++){
                        comunication_fd[j] = comunication_fd[j+1];
                        strcpy(*(check_ip+j),*(check_ip+j+1));
                    }
                }
              }
              count--;
            break;
        }
    }
    close(clientFD);
    pthread_exit(NULL);
}

void startAcceptingIncomingConnections(int serverSocketFD){
    while(true){
        struct AcceptSocket *clientSocket = acceptIncomingConnection(serverSocketFD);

        if(clientSocket != NULL){
            pthread_t id;
            pthread_create(&id, NULL, receiveAndPrint, (void *)&clientSocket->acceptSocketFD);
        }
    }
}

void *sender(void* fd){
 
    int clientFD = *(int *)fd;
    char str[512];
    char buff[1024];
    printf("\nType \"exit\" to stop the communication:\n");
    while (1){
    
        fgets(str, sizeof(str), stdin);
        printf("Send Data -> ");
        if(memcmp(str, "exit", 4) == 0){
            break;
        }
        sprintf(buff,"%s : %s",ip_address,str);
        write(clientFD, buff, sizeof(buff));
        memset(buff, 0, sizeof(buff));
    }
    pthread_exit(NULL);
}

void add_connection(){
    int choice,flag=false;
    pthread_t recv_thread;
    printf("\n------------------------------------------------------------------------->");
    printf("\n0 : Exit");
            for(int i = 0; Total_ips[i][0] != '\0'; i++){
                for(int j = 0; j < 1; j++){
                    if(strcmp(ip_address,&Total_ips[i][j])){
                        printf("\n%d : %s",i+1,&Total_ips[i][j]);
                    }
                }
            }
            printf("\n------------------------------------------------------------------------->");
            printf("\nSelect Ip : ");
            scanf("%d",&choice);
            
            if((choice == 0) || (choice > 4) ||  strcmp(*(Total_ips+(choice-1)),ip_address)==0){
            printf("\nEnter valid input");
            return;}
            flag = false;
            
            for(int i = 0;i < count;i++){
            
            	if((strcmp(*(check_ip+i),*(Total_ips+(choice-1))))==0){
                    printf("Connection Already Established");
                    flag = true;
                }
                
            }
           
            if(flag == false){
                int socketFD = socket(AF_INET, SOCK_STREAM, 0); //(Address Family I NET -> IPV4),(asking for TCP socket),(layer) it will return socket file descriptor 
                struct sockaddr_in *address = createIPv4Address(&Total_ips[choice-1][0], serverPort);
                
                if(connect(socketFD, (struct sockaddr *)address, sizeof(struct sockaddr)) != 0){
                    perror("Connection Fail");
                    return;
                
                }
        
                char rx_buffer[1024];
                read(socketFD, rx_buffer, 1024);
		        printf("Data from server:\t\t%s\n",rx_buffer);
               
                strcpy(*(check_ip + count),*(Total_ips+(choice-1)));
                comunication_fd[count] = socketFD;
                count++;
                pthread_create(&recv_thread, NULL, receiveAndPrint, (void *)(&socketFD));
                free(address);
                
                sleep(1);
            }
}

void Communication_thread(){
    int choice;
    pthread_t send_thread;
    printf("\n------------------------------------------------------------------------->");
    printf("\n0 : Exit");
    for(int i = 0; check_ip[i][0] != '\0' ; i++){
                for(int j = 0; j < 1; j++){
                    printf("\n%d : %s\n",i+1,&check_ip[i][j]);
                }
            }
            printf("------------------------------------------------------------------------->");
            printf("\nEnter Your Choice : ");
            
            scanf("%d",&choice);
        
            if(choice != 0 && choice <= count){
            	pthread_create(&send_thread, NULL, sender, (void *)&comunication_fd[choice - 1]);
            	pthread_join(send_thread, NULL);
            }
            __fpurge(stdin);
            
}
void signal_handler(int num){
    printf("\nsignal handler");
    for(int i=0;i<count;i++){
        close(comunication_fd[i]);
    }
    exit(0);
}

void send_all(){

    char str[10]="CCCC";
    char buff[1024];
    sprintf(buff,"%s \n\r",str);
    for(int i = 0;i<count;i++){
        write(comunication_fd[i], buff, sizeof(buff));
    }
    printf("Send to all nodes");
    return;

}
