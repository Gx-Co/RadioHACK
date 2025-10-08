#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "database.h"

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    setlocale(LC_ALL, "Russian");

    // Проверка и создание администратора при запуске сервера
    if (SUDB("admin", "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8") == 0) {
        std::cout << "Администратор не найден. Создание учетной записи администратора..." << std::endl;
        CUDB("admin", "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", "admin", "10");
    }

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

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Клиент подключен! IP: " << client_ip << std::endl;

    // === ЛОГИКА АВТОРИЗАЦИИ И СМЕНЫ ПАРОЛЯ ===
    bool authenticated = false;
    bool passwordChanged = false;
    char login[BUFFER_SIZE];
    char password[BUFFER_SIZE];

    // Запрос логина
    const char* ask_login = "Введите логин:";
    send(client_socket, ask_login, strlen(ask_login), 0);

    memset(login, 0, BUFFER_SIZE);
    int bytes_received = recv(client_socket, login, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        std::cerr << "Ошибка получения логина" << std::endl;
        closesocket(client_socket);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    login[bytes_received] = '\0';

    // Запрос пароля
    const char* ask_password = "Введите пароль:";
    send(client_socket, ask_password, strlen(ask_password), 0);

    memset(password, 0, BUFFER_SIZE);
    bytes_received = recv(client_socket, password, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        std::cerr << "Ошибка получения пароля" << std::endl;
        closesocket(client_socket);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    password[bytes_received] = '\0';

    // Преобразуем в std::string для передачи в функции базы данных
    std::string login_str(login);
    std::string password_str(password);

    // Проверка учетных данных
    if (SUDB(login_str, password_str)) {
        authenticated = true;
        std::cout << "Пользователь " << login_str << " успешно авторизован" << std::endl;

        // Если это администратор с дефолтным паролем - требуем смену
        if (login_str == "admin" && password_str == "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8") {
            std::cout << "Обнаружен пароль по умолчанию. Требуется смена пароля." << std::endl;

            const char* msg = "Требуется смена пароля администратора. Введите новый пароль:";
            send(client_socket, msg, strlen(msg), 0);

            char new_pass1[BUFFER_SIZE];
            memset(new_pass1, 0, BUFFER_SIZE);
            bytes_received = recv(client_socket, new_pass1, BUFFER_SIZE - 1, 0);
            if (bytes_received <= 0) {
                std::cerr << "Ошибка получения первого пароля" << std::endl;
                closesocket(client_socket);
                closesocket(server_fd);
                WSACleanup();
                return 1;
            }
            new_pass1[bytes_received] = '\0';

            const char* msg2 = "Повторите пароль:";
            send(client_socket, msg2, strlen(msg2), 0);

            char new_pass2[BUFFER_SIZE];
            memset(new_pass2, 0, BUFFER_SIZE);
            bytes_received = recv(client_socket, new_pass2, BUFFER_SIZE - 1, 0);
            if (bytes_received <= 0) {
                std::cerr << "Ошибка получения второго пароля" << std::endl;
                closesocket(client_socket);
                closesocket(server_fd);
                WSACleanup();
                return 1;
            }
            new_pass2[bytes_received] = '\0';

            // Сравниваем пароли
            if (strcmp(new_pass1, new_pass2) == 0) {
                // Меняем пароль
                CDDB("admin", std::string(new_pass1), "", "");
                const char* success_msg = "Пароль успешно изменен!";
                send(client_socket, success_msg, strlen(success_msg), 0);
                std::cout << "Пароль администратора успешно изменен" << std::endl;
                passwordChanged = true;
            }
            else {
                const char* error_msg = "Пароли не совпадают!";
                send(client_socket, error_msg, strlen(error_msg), 0);
                std::cout << "Пароли не совпадают. Соединение закрыто." << std::endl;
                closesocket(client_socket);
                closesocket(server_fd);
                WSACleanup();
                return 1;
            }
        }

        const char* success_msg = "Авторизация успешна!";
        send(client_socket, success_msg, strlen(success_msg), 0);
    }
    else {
        const char* error_msg = "Ошибка авторизации: неверный логин или пароль.";
        send(client_socket, error_msg, strlen(error_msg), 0);
        std::cout << "Неудачная попытка авторизации для пользователя: " << login_str << std::endl;
        closesocket(client_socket);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Получаем подтверждение от клиента об успешной авторизации
    char ack[BUFFER_SIZE];
    memset(ack, 0, BUFFER_SIZE);
    bytes_received = recv(client_socket, ack, BUFFER_SIZE - 1, 0);

    if (passwordChanged) {
        const char* ready_msg = "Теперь вы можете отправлять команды:";
        send(client_socket, ready_msg, strlen(ready_msg), 0);
    }
    else {
        const char* ready_msg = "Добро пожаловать! Вы можете отправлять команды:";
        send(client_socket, ready_msg, strlen(ready_msg), 0);
    }
    // === КОНЕЦ ЛОГИКИ АВТОРИЗАЦИИ ===

    std::cout << "Ожидание сообщений от клиента..." << std::endl;

    // Обмен данными
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

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

        std::string response;
        if (strcmp(buffer, "привет") == 0) {
            response = "Здравствуйте!";
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
