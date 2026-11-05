#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DAYTIME_PORT 13
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    int sockfd;
    sockaddr_in *server_addr = new sockaddr_in;
    char buffer[BUFFER_SIZE];
    const char *server_ip = "127.0.0.1";
    
    // Позволяем передать IP-адрес сервера через аргумент командной строки
    if (argc > 1) {
        server_ip = argv[1];
    }

    // 1. Подготовка адресной структуры сервера
    memset(server_addr, 0, sizeof(sockaddr_in));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(DAYTIME_PORT);
    
    // Преобразуем IP из текста в бинарный формат
    if (inet_pton(AF_INET, server_ip, &server_addr->sin_addr) <= 0) {
        std::cerr << "Ошибка inet_pton: неверный адрес" << std::endl;
        delete server_addr;
        return EXIT_FAILURE;
    }

    // 2. Создание сокета для IPv4 UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        delete server_addr;
        return EXIT_FAILURE;
    }

    // 3. bind() - для клиента не обязателен, ОС сама назначит случайный порт
    // 4. connect() - для UDP без фиктивного соединения не делаем

    // 5. Информационный обмен (sendto / recvfrom)
    // Протокол daytime отвечает на любой пакет, отправим пустую строку
    char msg[] = "";
    socklen_t len = sizeof(sockaddr_in);
    
    if (sendto(sockfd, msg, sizeof(msg), 0, 
               (const sockaddr *)server_addr, len) < 0) {
        std::cerr << "Ошибка sendto" << std::endl;
        close(sockfd);
        delete server_addr;
        return EXIT_FAILURE;
    }

    // Ожидаем ответ от сервера
    sockaddr_in *from_addr = new sockaddr_in;
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, 
                     (sockaddr *)from_addr, &len);
    if (n < 0) {
        std::cerr << "Ошибка recvfrom" << std::endl;
        close(sockfd);
        delete server_addr;
        delete from_addr;
        return EXIT_FAILURE;
    }
    
    buffer[n] = '\0'; // Добавляем символ конца строки
    std::cout << "Сервер " << server_ip << " вернул время:" << std::endl;
    std::cout << buffer << std::endl;

    // 6. Закрытие сокета
    close(sockfd);
    delete server_addr;
    delete from_addr;

    return EXIT_SUCCESS;
}
