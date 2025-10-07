#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

bool superuser_is_exist() {
    return true;
}

int main() {
    setlocale(LC_ALL, "Russian");
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock" << std::endl;
        return 1;
    }

    // Создание сокета
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Прослушивание порта
    if (listen(server_fd, 1) == SOCKET_ERROR) {
        std::cerr << "Ошибка прослушивания: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "Сервер запущен. Ожидание подключения на порту " << PORT << "..." << std::endl;

    // Принятие подключения
    sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    SOCKET client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Ошибка принятия подключения: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    // --------------------------------------------------------------------------------------
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Клиент подключен! IP: " << client_ip << std::endl;
    std::cout << "Ожидание сообщений от клиента..." << std::endl;

    // ... после accept подключения ...

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Клиент подключен! IP: " << client_ip << std::endl;

    // === ЛОГИКА ПРОВЕРКИ ПОСЛЕ ПОДКЛЮЧЕНИЯ ===
    std::cout << "Проверка подключения..." << std::endl;

    // 1. Проверка связи (отправка тестового сообщения)
    const char* welcome_msg = "Добро пожаловать на сервер! Подключение установлено.";
    if (send(client_socket, welcome_msg, strlen(welcome_msg), 0) == SOCKET_ERROR) {
        std::cerr << "Ошибка отправки приветствия: " << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // 2. Ожидание подтверждения от клиента
    char ack_buffer[BUFFER_SIZE];
    memset(ack_buffer, 0, BUFFER_SIZE);
    int ack_received = recv(client_socket, ack_buffer, BUFFER_SIZE - 1, 0);

    if (ack_received <= 0) {
        std::cerr << "Клиент не подтвердил подключение" << std::endl;
        closesocket(client_socket);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    ack_buffer[ack_received] = '\0';
    std::cout << "Клиент подтвердил: " << ack_buffer << std::endl;

    // 3. Дополнительные проверки (например, аутентификация)
    // ... ваш код проверки ...

    std::cout << "Проверка завершена. Ожидание сообщений..." << std::endl;
    // === КОНЕЦ ЛОГИКИ ПРОВЕРКИ ===

    // Обмен данными
    char buffer[BUFFER_SIZE];
    while (true) {
        // ... основной цикл ...
    }

    // Обмен данными
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0); // -1 для безопасност

        if (bytes_received == SOCKET_ERROR) {
            std::cerr << "Ошибка приема данных: " << WSAGetLastError() << std::endl;
            break;
        }

        else if (bytes_received == 0) {
            std::cout << "Клиент отключился" << std::endl;
            break;
        }

        // Добавляем нулевой терминатор
        buffer[bytes_received] = '\0';

        std::cout << "Клиент: " << buffer << std::endl;
        
        // место, где нужно писать ответы для клиента <------------------------
        
        std::string response;

        if (strcmp(buffer, "hello") == 0) {
            response = "hi!";
        }
        else if (strcmp(buffer, "time") == 0) {
            // вернуть текущее время
        }
        else {
            response = "Неизвестная команда: ";
            response += buffer;
        }
        send(client_socket, response.c_str(), response.length(), 0);

        std::cout << "Ответ отправлен клиенту" << std::endl;
    }

    closesocket(client_socket);
    closesocket(server_fd);
    WSACleanup();
    std::cout << "Сервер завершил работу" << std::endl;
    return 0;
}
