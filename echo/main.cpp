#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ECHO_PORT 7777
#define BUFFER_SIZE 1024

int main() {
    int listen_sock;
    sockaddr_in *server_addr = new sockaddr_in;
    
    // 1. Подготовка адресной структуры сервера
    memset(server_addr, 0, sizeof(sockaddr_in));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(ECHO_PORT);
    server_addr->sin_addr.s_addr = INADDR_ANY; // Слушать на всех интерфейсах
    
    // 2. Создание сокета для IPv4 TCP
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        delete server_addr;
        return EXIT_FAILURE;
    }
    
    // 3. Привязка сокета к адресу
    if (bind(listen_sock, (const sockaddr *)server_addr, sizeof(sockaddr_in)) < 0) {
        std::cerr << "Ошибка bind" << std::endl;
        close(listen_sock);
        delete server_addr;
        return EXIT_FAILURE;
    }
    
    // 4. Перевод сокета в режим ожидания соединений
    if (listen(listen_sock, 10) < 0) {
        std::cerr << "Ошибка listen" << std::endl;
        close(listen_sock);
        delete server_addr;
        return EXIT_FAILURE;
    }
    
    std::cout << "Echo-сервер запущен на порту " << ECHO_PORT << std::endl;
    
    // 5. Бесконечный цикл обработки входящих соединений
    while (true) {
        sockaddr_in *client_addr = new sockaddr_in;
        socklen_t len = sizeof(sockaddr_in);
        
        // 5.2 Принять клиентское соединение
        int work_sock = accept(listen_sock, (sockaddr *)client_addr, &len);
        if (work_sock < 0) {
            std::cerr << "Ошибка accept" << std::endl;
            delete client_addr;
            continue;
        }
        
        std::cout << "Подключился клиент: " 
                  << inet_ntoa(client_addr->sin_addr) 
                  << ":" << ntohs(client_addr->sin_port) << std::endl;
        
        // 5.3 Информационный обмен с клиентом
        char buffer[BUFFER_SIZE];
        while (true) {
            // Получаем данные от клиента
            int n = recv(work_sock, buffer, BUFFER_SIZE, 0);
            
            if (n < 0) {
                std::cerr << "Ошибка recv" << std::endl;
                break;
            }
            
            if (n == 0) {
                std::cout << "Клиент отключился" << std::endl;
                break;
            }
            
            std::cout << "Получено " << n << " байт: " 
                      << std::string(buffer, n) << std::endl;
            
            // Отправляем данные обратно (эхо)
            if (send(work_sock, buffer, n, 0) < 0) {
                std::cerr << "Ошибка send" << std::endl;
                break;
            }
        }
        
        // 5.4 Закрыть рабочий сокет
        close(work_sock);
        delete client_addr;
        
        std::cout << "Соединение закрыто" << std::endl;
    }
    
    // Закрытие основного сокета (никогда не достигнем, но для полноты)
    close(listen_sock);
    delete server_addr;
    
    return EXIT_SUCCESS;
}
