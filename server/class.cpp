#include <iostream>
#include <winsock2.h>
#include <cstring>
#include <string>
#include <WS2tcpip.h>

using namespace std;

class SimpleServer {
private:
    //�������
    WSADATA wsaData;

public:
    //�޼ҵ�
    int Run() {
        int iniResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iniResult != 0)
        {
            printf("Can't Initialize winsock! Quitiing");
            return -1;
        }

        SOCKET listeningSock = socket(AF_INET, SOCK_STREAM, 50001); // 3�� ° ���� 0�� OK
        if (listeningSock == INVALID_SOCKET)
        {
            printf("Can't create a socket! Quitting");
            WSACleanup();
            return -1;
        }

        sockaddr_in hint{}; // �⺻ �ʱ�ȭ ����
        hint.sin_family = AF_INET;
        hint.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        hint.sin_port = htons(50001);

        int bindResult = bind(listeningSock, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
        if (bindResult == SOCKET_ERROR)
        {
            printf("Can't bind a socket! Quitting");
            closesocket(listeningSock);
            WSACleanup();
            return -1;
        }

        int listenResult = listen(listeningSock, SOMAXCONN);
        if (listenResult == SOCKET_ERROR)
        {
            printf("Can't listen a socket! Quitting");
            closesocket(listeningSock);
            WSACleanup();
            return -1;
        }

        sockaddr_in clientSockInfo;
        int clientSize = sizeof(clientSockInfo);

        // connection queue�� ���� �տ� �ִ� Ŭ���̾�Ʈ ��û�� accept�ϰ�, client ������ ��ȯ�մϴ�.
        SOCKET clientSocket = accept(listeningSock, reinterpret_cast<sockaddr*>(&clientSockInfo), &clientSize);
        if (clientSocket == INVALID_SOCKET)
        {
            printf("Can't accept a socket! Quitting");
            closesocket(listeningSock);
            WSACleanup();
            return -1;
        }

        char host[NI_MAXHOST];	         // Ŭ���̾�Ʈ�� host �̸�
        char service[NI_MAXHOST];        // Ŭ���̾�Ʈ�� PORT ��ȣ
        ZeroMemory(host, NI_MAXHOST);    // memset(host, 0, NI_MAXHOST)�� ����
        ZeroMemory(service, NI_MAXHOST);

        // clientSockInfo�� ����� IP �ּҸ� ���� ������ ������ ����ϴ�. host �̸��� host��, ��Ʈ ��ȣ�� service�� ����˴ϴ�.
        // getnameinfo()�� ���� �� 0�� ��ȯ�մϴ�. ���� �� 0�� �ƴ� ���� ��ȯ�մϴ�.
        if (getnameinfo((sockaddr*)&clientSockInfo, sizeof(clientSockInfo), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
        {
            cout << host << " connected ON port " << service << endl;
        }
        else
        {
            inet_ntop(AF_INET, &clientSockInfo.sin_addr, host, NI_MAXHOST);
            cout << host << " connected on port " << ntohs(clientSockInfo.sin_port) << endl;
        }


        // While loop: Ŭ���̾�Ʈ�� �޼����� �޾Ƽ� ��� �� Ŭ���̾�Ʈ�� �ٽ� �����ϴ�.
        enum eBufSize { BUF_SIZE = 4096 };
        char buf[BUF_SIZE];

        while (true)
        {
            ZeroMemory(buf, BUF_SIZE);

            // Wait for client to send data
            // �޼����� ���������� ������ recv �Լ��� �޼����� ũ�⸦ ��ȯ�Ѵ�.
            int bytesReceived = recv(clientSocket, buf, BUF_SIZE, 0);
            if (bytesReceived == SOCKET_ERROR)
            {
                cerr << "Error in recv(). Quitting" << endl;
                break;
            }

            if (bytesReceived == 0)
            {
                cout << "Client disconnected " << endl;
                break;
            }

            // Echo message back to client
            cout << buf << endl;
            send(clientSocket, buf, bytesReceived + 1, 0);
        }
    }
};