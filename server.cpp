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


#define SOUR_PORT   52000
#define MAX_LINE    1024


int main()
{
    std::fstream fstream; /* Поток ввода-вывода */

    int sockfd, newsockfd; /* Дескрипторы для слушающего и присоединенного сокетов */
    int clilen; /* Длина адреса клиента */
    int n; /* Количество принятых символов */
    char data[MAX_LINE]; /* Буфер для приема информации */
    struct sockaddr_in servaddr, cliaddr; /* Структуры для размещения полных адресов сервера и клиента */

    /* Создаем TCP-сокет */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror(NULL);
        exit(1);
    }

    /* Заполнение структуры для адреса сервера: семейство протоколов TCP/IP, сетевой интерфейс — любой, номер порта 52000. 
    Поскольку в структуре содержится дополнительное ненужное поле, которое должно быть нулевым, обнуляем ее всю перед заполнением */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family= AF_INET;
    servaddr.sin_port= htons(SOUR_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Настраиваем адрес сокета */
    if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    /* Переводим созданный сокет в пассивное (слушающее) состояние. Глубину очереди для установленных соединений описываем значением 5 */
    if(listen(sockfd, 5) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    /* Основной цикл сервера */
    //while(true){

        /* Открываем или создаём файл для сохранения изображения */
        fstream.open("Image.png", std::fstream::binary | std::fstream::out);

        if (!fstream.is_open()) 
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }

        /* Ожидание полностью установленного соединения на слушающем сокете. При нормальном завершении в структуре cliaddr будет лежать полный адрес клиента, 
        установившего соединение, а в переменной clilen — его фактическая длина.
        Вызов вернет дескриптор присоединенного сокета, через который будет происходить общение с клиентом. 
        Информация о клиенте у нас в дальнейшем никак не используется, поэтому вместо второго и третьего параметров можно было поставить значения NULL. */
        if((newsockfd = accept(sockfd, (struct sockaddr *) NULL, (socklen_t *) NULL)) < 0){
            perror(NULL);
            close(sockfd);
            exit(1);
        }

        /* В цикле принимаем информацию от клиента до тех пор, пока не произойдет ошибка (вызов read() вернет отрицательное значение) 
        или клиент не закроет соединение (вызов read() вернет значение 0). 
        Максимальную длину одной порции данных от клиента ограничим 1024 символами.
        В операциях чтения и записи пользуемся дескриптором присоединенного сокета, т. е. значением, которое вернул вызов accept().*/
        while((n = read(newsockfd, data, MAX_LINE)) > 0){

            /* Принятые данные записываем в файл */
            fstream.write(data, n);
        }

        /* Если при чтении возникла ошибка — завершение работы */
        if(n < 0){
            perror(NULL);
            close(sockfd);
            close(newsockfd);
            exit(1);
        }

        /* Закрытие дескриптора присоединенного сокета и ожидание нового соединения */
        close(newsockfd);
        fstream.close();
    //}
}