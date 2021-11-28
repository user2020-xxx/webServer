#include "server.h"
#include <iostream>
#include <sstream>
#include <string>
#include "httpParser.h"

#define _WIN32_WINNT 0x501

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using std::cerr;


void Server::startServer() {
    std::cout << "Server running" << std::endl;
    WSADATA wsaData; // ��������� ��������� ��� �������� ����������
// � ���������� Windows Sockets
// ����� ������������� ���������� ������� ���������
// (������������ Ws2_32.dll)
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // ���� ��������� ������ ��������� ����������
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << "\n";
    }

    struct addrinfo* addr = NULL; // ���������, �������� ����������
    // �� IP-������  ���������� ������

    // ������ ��� ������������� ��������� ������
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET; // AF_INET ����������, ��� �����
    // �������������� ���� ��� ������ � �������
    hints.ai_socktype = SOCK_STREAM; // ������ ��������� ��� ������
    hints.ai_protocol = IPPROTO_TCP; // ���������� �������� TCP
    hints.ai_flags = AI_PASSIVE; // ����� ����� ��������� �� �����,
    // ����� ��������� �������� ����������

    // �������������� ���������, �������� ����� ������ - addr
    // ��� HTTP-������ ����� ������ �� 8000-� ����� ����������
    result = getaddrinfo("127.0.0.1", "5000", &hints, &addr);

    // ���� ������������� ��������� ������ ����������� � �������,
    // ������� ���������� �� ���� � �������� ���������� ���������
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << result << "\n";
        WSACleanup(); // �������� ���������� Ws2_32.dll
    }

    // �������� ������
    int listen_socket = socket(addr->ai_family, addr->ai_socktype,
        addr->ai_protocol);

    if (listen_socket == INVALID_SOCKET) {
        cerr << "Error at socket: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        WSACleanup();
    }

    // ����������� ����� � IP-������
    result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

    // ���� ��������� ����� � ������ �� �������, �� ������� ���������
    // �� ������, ����������� ������, ���������� ��� ��������� addr.
    // � ��������� �������� �����.
    // ��������� DLL-���������� �� ������ � ��������� ���������.
    if (result == SOCKET_ERROR) {
        cerr << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        closesocket(listen_socket);
        WSACleanup();
    }

    // �������������� ��������� �����
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(listen_socket);
        WSACleanup();
    }

    const int max_client_buffer_size = 2048;
    char buf[max_client_buffer_size];
    int client_socket = INVALID_SOCKET;

    for (;;) {
        // ��������� �������� ����������
        client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            cerr << "accept failed: " << WSAGetLastError() << "\n";
            closesocket(listen_socket);
            WSACleanup();
        }

        result = recv(client_socket, buf, max_client_buffer_size, 0);
       
        std::stringstream response; // ���� ����� ������������ ����� �������
        std::stringstream response_body; // ���� ������

        if (result == SOCKET_ERROR) {
            // ������ ��������� ������
            cerr << "recv failed: " << result << "\n";
            closesocket(client_socket);
        }
        else if (result == 0) {
            // ���������� ������� ��������
            cerr << "connection closed...\n";
        }
        else if (result > 0) {
            // �� ����� ����������� ������ ���������� ������, ������� ������ ����� ����� ������
            // � ������ �������.
            buf[result] = '\0';
            std::string str(buf);
            HttpParser::parseRequest(str);
            // ���������� ����� ������� � ������� ������� send
            result = send(client_socket, response.str().c_str(),
                response.str().length(), 0);

            if (result == SOCKET_ERROR) {
                // ��������� ������ ��� �������� ������
                cerr << "send failed: " << WSAGetLastError() << "\n";
            }
            // ��������� ���������� � ��������
            closesocket(client_socket);
        }
    }

    // ������� �� �����
    closesocket(listen_socket);
    freeaddrinfo(addr);
    WSACleanup();
}

int main()
{
    Server server;
    server.startServer();
}
