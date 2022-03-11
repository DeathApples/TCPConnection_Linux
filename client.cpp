#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#include <fstream>


#define DEST_PORT   52000
#define MAX_LINE    1024
#define MAX_CHAR    256
#define IP_ADDRESS  "127.0.0.1"


int main(int argc, char **argv)
{
    std::fstream fstream; /* Поток ввода-вывода */

    int sockfd; /* Дескриптор сокета */
    int n; /* Количество переданных или прочитанных символов */
    char data[MAX_LINE]; /* Массив для отсылаемых данных */
    char path[MAX_CHAR]; /* Массив для сохраннеия пути к файлу */
    struct sockaddr_in servaddr; /* Структура для адреса сервера */

    /* Обнуление символьных массивов */
    bzero(data, MAX_LINE);

    /* Создание TCP сокета */
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror(NULL); /* Печать сообщения об ошибке */
        exit(1);
    }

    /* Заполнение структуры для адреса сервера: семейство протоколов TCP/IP, сетевой интерфейс — из аргумента командной строки, номер порта 7. 
    Поскольку в структуре содержится дополнительное ненужное поле, которое должно быть нулевым, перед заполнением обнуляем ее всю */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DEST_PORT);

    if(inet_aton(IP_ADDRESS, &servaddr.sin_addr) == 0){
        std::cout << "Invalid IP address" << std::endl;
        close(sockfd);
        exit(1);
    }

    /* Установка логического соединения через созданный сокет с сокетом сервера, адрес которого занесли в структуру */
    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    printf("Введите путь к файлу >> ");
    std::cin.getline(path, MAX_CHAR);

    /* Открытие изображения на бинарное чтение */
    fstream.open(path, std::fstream::binary | std::fstream::in);

    if (!fstream.is_open()) 
    {
        printf("\nError : Open File Failed \n");
		exit(1);
    }

    /* Считываем данные из файла и отправляем их по сети */
    while (!fstream.eof()) 
    {
        fstream.read(data, MAX_LINE);

        if( (n = write(sockfd, data, strlen(data) + 1)) < 0){
            perror("Can\'t write\n");
            close(sockfd);
            exit(1);
        }
    }

    /* Завершение соединения*/
    fstream.close();
    close(sockfd);

    return 0;
}