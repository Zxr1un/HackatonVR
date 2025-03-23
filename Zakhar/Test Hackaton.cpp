#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>


#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6501
#define CLIENT_PORT 6502


const static int size1 = 30;

using namespace std;
using namespace chrono;

class Data {
public:
    static double value;
    static int seconds;
    static steady_clock::time_point start_time;

  

};
double Data::value = 0;
int Data::seconds = 0;
steady_clock::time_point Data::start_time = steady_clock::now();

class DataTransforms {
public:
    static vector<double> update_value(vector<double>& send_array){
        Data::value += 0.5;

        send_array[2] = Data::value;

        return send_array;
    }

    static vector<double> update_time(vector<double>& send_array) {
        auto now = steady_clock::now();
        Data::seconds = duration_cast<seconds>(now - Data::start_time).count();

        send_array[3] = Data::seconds;

        return send_array;
    }
};

class Client {
public:
    static void send_data(SOCKET sockfd, sockaddr_in& server_addr, vector<double>& send_array) {
        char buffer[size1 * 8];
        memcpy(buffer, send_array.data(), size1 * 8);
        sendto(sockfd, buffer, size1 * 8, 0, (sockaddr*)&server_addr, sizeof(server_addr));

    }

    static vector<double> get_data(SOCKET sockfd) {
        char buffer[size1 * 8];
        sockaddr_in sender_addr;
        int sender_len = sizeof(sender_addr);
        int received_bytes = recvfrom(sockfd, buffer, size1 * 8, 0, (sockaddr*)&sender_addr, &sender_len);

        vector<double> received_array(size1, 0.0);
        if (received_bytes == size1 * 8) {
            memcpy(received_array.data(), buffer, size1 * 8);
        }
        else {
            cerr << "gfvfhfhdsfhjkfsdjhk";
        }

        return received_array;
    }

};






struct object {
    double x, y, z;       // Положение
    double vx, vy, vz;    // Скорость
    double ax, ay, az;    // Ускорение
    double ax_prev, ay_prev, az_prev; // Ускорение на предыдущем шаге
    double mass;          // Масса
    double rad;           // Радиус
    string name;          // Имя

    object(double x = 0, double y = 0, double z = 0,
        double vx = 0, double vy = 0, double vz = 0,
        double mass = 1, double rad = 1, string name = "unknown")
        : x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
        mass(mass), rad(rad), name(name),
        ax(0), ay(0), az(0), ax_prev(0), ay_prev(0), az_prev(0) {
    }
};

class objects {
public:
    double G = 6.07 * pow(10, -11); // Гравитационная постоянная
    vector<object> vec;             // Вектор объектов
    int ammount = 0;                // Количество объектов
    double SpF = 1;              // Шаг времени

    // Метод для вычисления ускорений
    void compute_accelerations() {
        for (int i = 0; i < vec.size(); i++) {
            vec[i].ax = 0;
            vec[i].ay = 0;
            vec[i].az = 0;

            for (int j = 0; j < vec.size(); j++) {
                if (j != i) {
                    double dx = vec[j].x - vec[i].x;
                    double dy = vec[j].y - vec[i].y;
                    double dz = vec[j].z - vec[i].z;
                    double d2 = dx * dx + dy * dy + dz * dz;
                    double dist = sqrt(d2);

                    double a = G * vec[j].mass / d2;

                    vec[i].ax += a * dx / dist;
                    vec[i].ay += a * dy / dist;
                    vec[i].az += a * dz / dist;
                }
            }
        }
    }

    // Метод для обновления координат и скоростей (метод Верле)
    void physics() {
        // Шаг 1: Обновление координат
        for (int i = 0; i < vec.size(); i++) {
            vec[i].x += vec[i].vx * SpF + 0.5 * vec[i].ax * SpF * SpF;
            vec[i].y += vec[i].vy * SpF + 0.5 * vec[i].ay * SpF * SpF;
            vec[i].z += vec[i].vz * SpF + 0.5 * vec[i].az * SpF * SpF;
        }

        // Шаг 2: Вычисление новых ускорений
        compute_accelerations();

        // Шаг 3: Обновление скоростей
        for (int i = 0; i < vec.size(); i++) {
            vec[i].vx += 0.5 * (vec[i].ax + vec[i].ax_prev) * SpF;
            vec[i].vy += 0.5 * (vec[i].ay + vec[i].ay_prev) * SpF;
            vec[i].vz += 0.5 * (vec[i].az + vec[i].az_prev) * SpF;

            // Сохраняем текущие ускорения для следующего шага
            vec[i].ax_prev = vec[i].ax;
            vec[i].ay_prev = vec[i].ay;
            vec[i].az_prev = vec[i].az;
        }
    }

    // Добавление объекта
    void add(double x = 0,
        double y = 0,
        double z = 0,
        double vx = 0,
        double vy = 0,
        double vz = 0,
        double mass = 1,
        double rad = 1,
        string name = "unknown") {
        vec.push_back(object(x, y, z, vx, vy, vz, mass, rad, name));
        ammount++;
    }

    // Вывод информации об объектах
    void print() {
        for (int i = 0; i < vec.size(); i++) {
            cout << endl << endl << vec[i].name;
            cout << "\nPosition: " << vec[i].x << " " << vec[i].y << " " << vec[i].z << " ";
            cout << "\nSpeed:    " << vec[i].vx << " " << vec[i].vy << " " << vec[i].vz << " ";
            cout << endl;
        }
    }
};









int main()
{
    try {
        // Инициализация WinSock
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup failed: " << result << std::endl;
            return 1;
        }

        SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == INVALID_SOCKET) {
            cerr << "Ошибка";
            WSACleanup();
            return 1;
        }

        // Настройка адреса сервера
        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

        // Настройка адреса клиента
        sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(CLIENT_PORT);
        client_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd, (sockaddr*)&client_addr, sizeof(client_addr)) == SOCKET_ERROR) {
            cerr << "Ошибка привязки сокета к порту\n";
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        Client client;
        vector<double> send_array(size1, 0.0); // Массив для отправки данных
        

        objects all1;
        cout << all1.G << endl;
        all1.add(0, 5, 0, 1.2, 0, 0, pow(10, 0), 1, "Earth");
        all1.add(0, 0, 0, 0, 0, 0, pow(10, 11), 1, "Earth2");
        
        all1.SpF = 0.01;
        double time = 0;
        double step = 10;

        while (true) {
            vector<double> get_array = client.get_data(sockfd); // Получаем данные от VR Concept
            // Обновляем массив для отправки в зависимости от положения объекта
            send_array = DataTransforms::update_value(send_array);
            send_array = DataTransforms::update_time(send_array);
            
            // Отправляем обновленные данные
           

            // Пауза 1 секунда
            this_thread::sleep_for(seconds(0));

            

            for (int j = 0; j < 10; j++) {
                all1.physics();
            }
            
            send_array[4] = all1.vec[0].x;
            send_array[5] = all1.vec[0].y;
            send_array[6] = all1.vec[0].z;
            
            send_array[7] = all1.vec[1].x;
            send_array[8] = all1.vec[1].y;
            send_array[9] = all1.vec[1].z;


            
            send_array[10] = time;
            time += step;
            cout << time << endl;



            client.send_data(sockfd, server_addr, send_array);
        }

        closesocket(sockfd);
        WSACleanup();
    }
    catch (const exception& e) {
        cerr << "Произошла ошибка: " << e.what() << endl;
    }

    // Очистка WinSock
    WSACleanup();
    return 0;
}