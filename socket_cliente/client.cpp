#include <iostream>
#include <windows.h>
#include <winsock.h>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <cctype>
#include <ctime>
#pragma comment(lib, "ws2_32")

using namespace std;

class Client {
public:
    /* Inicializar WinSock en la aplicación Cliente */

    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN direccionServer;
    char buffer[1024];

    Client() {
        cout << "Conectando al servidor..." << endl << endl;

        // Inicializa la biblioteca Winsock con la versión 2.0.
        WSAStartup(MAKEWORD(2, 0), &WSAData);

        // Crea un socket para el cliente con IPv4 y protocolo TCP.
        server = socket(AF_INET, SOCK_STREAM, 0);

        // Configura la dirección IP y el puerto del servidor al que se conectará el cliente.
        direccionServer.sin_addr.s_addr = inet_addr("192.000.0.000"); // aqui debe ir  la IP donde esta el servidor
        direccionServer.sin_family = AF_INET;

        int puerto;
        cout << "\nIngrese puerto: " << endl;
        cin >> puerto;
        direccionServer.sin_port = htons(puerto); // Puerto 5005 (debe coincidir con el del servidor).

        // Establece la conexión con el servidor. Si es 0 la conexion se realizo con exito, sino informa error
        if(connect(server, (SOCKADDR *)&direccionServer, sizeof(direccionServer)) != 0){
            cout << "Error al conectar al servidor" << endl;
            WSACleanup();
            return;
        }else {
            //////////////////////////
            ingresarUsuarioSesion();
            /////////////////////////



        }
    }

    void traduccion(){

        string palabraIngles;

        cout << "ingrese palabra para traducir: ";
        cin >> palabraIngles;

        //minusculas(palabraIngles);

        cout << "test para ver si guardo la palabra: " + palabraIngles << endl;

        send(server, palabraIngles.c_str(), palabraIngles.length(), 0);

        memset(buffer, 0, sizeof(buffer)); // limpio el buffer despues de enviar la palabra

        int bytesRecibidos = recv(server, buffer, sizeof(buffer), 0);

        if (bytesRecibidos == -1){
            cout << "error al recibir traduccion" << endl;
        } else {
          string palabraTraducida(buffer, bytesRecibidos);

            cout << "traduccion: " + palabraTraducida << endl;
        }
    }

    void nuevaTraduccion(){
        string traduccionNueva;

        cout << "ingrese nueva traduccion: ";
        cin >> traduccionNueva;

        send(server, traduccionNueva.c_str(), traduccionNueva.length(), 0);

        memset(buffer, 0, sizeof(buffer)); // limpio el buffer despues de enviar la traduccion
    }

void subMenu(){
    do{
        int op;
        string salida;
            cout << "INGRESE UNA OPCION: \n";
            cout << "1) ALTA \n";
            cout << "2) DESBLOQUEO \n";
            cout << "3) SALIR AL MENU ANTERIOR \n";
            cin >> op;
            send(server, (char *)&op, sizeof(op), 0);

            switch(op){
            case 1:
                cout << "Alta: \n";
                ingresarUsuario();
                break;
            case 2:
                cout << "Desbloqueo: \n";
                desbloquear();
                break;
            case 3:
                cout << "escriba /salir para ir al menu anterior: ";
                cin >> salida;

                if(salida == "/salir"){
                    menuAdmin();
                }
            }


    }while(1);
}

void ingresarUsuario(){

            string usuario, contrasena;

            cout << "ingrese su usuario: ";
            cin >> usuario;

            //limpio el buffer
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            cout << "ingrese su contrasena: ";
            getline(cin, contrasena);
            cin.ignore(); //ignora el salto de linea mmm bueno soluciono error de que me comia una letra pero debo ingresar dos veces enter

            cout << contrasena << endl;

            string credencial = usuario + "|" + contrasena;

            send (server, credencial.c_str(), credencial.length(), 0); //envio el user al server

            memset(buffer, 0, sizeof(buffer)); //limpio el buffer

}

//ingresar usuario solo para iniciar sesion
void ingresarUsuarioSesion() {
    string usuario, contrasena;

    cout << "Ingrese su usuario: ";
    cin >> usuario;

    // Limpia el búfer
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Ingrese su contrasena: ";
    getline(cin, contrasena);
    cin.ignore(); // Ignora el salto de línea

    string credencial = usuario + "|" + contrasena;

    send(server, credencial.c_str(), credencial.length(), 0); // Envía el usuario y contraseña al servidor

    // Recibe el rol del servidor
    int bytesRecibidos = recv(server, buffer, sizeof(buffer), 0);

    if (bytesRecibidos == -1) {
        cout << "Error al recibir el rol del servidor" << endl;
    } else {
        string rol(buffer, bytesRecibidos);

        if (rol == "ADMIN") {
            menuAdmin(); // Muestra el menú de administrador
        } else {
            menu(); // Muestra el menú de usuario estándar
        }

        memset(buffer, 0, sizeof(buffer)); // Limpia el buffer

        // Resto de la lógica de tu función
    }
}

//usuarios bloqueados
void desbloquear(){
    string usuario;

    cout << "Ingrese usuario para desbloquear: ";
    cin >> usuario;

    //limpio el buffer
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    send (server, usuario.c_str(), usuario.length(), 0);

    //recibe la respuesta del servidor
    int bytesRecibidos = recv(server, buffer, sizeof(buffer), 0);

    if (bytesRecibidos == -1) {
        cout << "Error al recibir la respuesta del servidor" << endl;
    } else {
        string respuesta(buffer, bytesRecibidos);
        cout << "respuesta " + respuesta << endl;
        if (respuesta == "El usuario fue desbloqueado") {
            cout << "Tu usuario esta desbloqueado\n" << endl;
        } else if (respuesta == "No se puede desbloquear") {
            cout << "Este usuario no se puede desbloquear\n";
        } else {
            cout << "Respuesta inesperada del servidor\n";
        }
    }
}


//CERRAR SESION
void cerrarSesion(){
    //FALTA PONER UN MENSAJE QUE RECIBA DEL SERVER QUE SE CERRO LA CONEXION
    cout << "se cerro la conexion" << endl;
    closesocket(server);

    WSACleanup();
    exit(0);

}

void menuAdmin(){
    do{
        int option;

        cout << "MENU DEL ADMINISTRADOR \n";

        cout << "2- Nueva Traduccion(rol ADMIN)\n";
        cout << "3- Usuarios(rol ADMIN)\n";
        cout << "4- Ver registro de actividades(rol ADMIN)\n";
        cout << "0- Cerrar sesion(ambos roles)\n";
        std::cin >> option;

        //limpiar el buffer
        limpiarBuffer();

        send(server, (char *)&option, sizeof(option), 0);

        switch(option){

            case 2:
            std::cout << "Nueva traduccion\n";
            nuevaTraduccion();
            break;

            case 3:
            std::cout << "Usuarios del sistema\n";
            subMenu();
            break;

            case 4:
            cout << "Ver registro de actividades\n";
            mostrarServerLog();
            break;

            case 0:
            cerrarSesion();
            break;

         }


    }while(1);
}

void menu(){
    //MENU
    do{
        int op;


        std::cout << "BIENVENIDO INGRESE UNA OPCION: \n";

        std::cout << "1- Traducir(rol CONSULTA)\n";
        std::cout << "0- Cerrar sesion(ambos roles)\n";
        std::cin >> op;


        //limpiar el buffer
        limpiarBuffer();


        send(server, (char *)&op, sizeof(op), 0);


         switch(op){
            case 1:
            std::cout << "traduccion\n";
           traduccion();
            break;

            case 0:
            cerrarSesion();
            break;

         }


            } while(true);
}

void logEvent(const std::string& message) {
    std::ofstream logFile("server.log", std::ios::app);  // Abre el archivo en modo de apertura para agregar

    if (logFile.is_open()) {
        // Obtiene la fecha y hora actual
        std::time_t currentTime = std::time(nullptr);
        std::tm* timeInfo = std::localtime(&currentTime);
        char timeBuffer[80];
        std::strftime(timeBuffer, sizeof(timeBuffer), "[%Y-%m-%d %H:%M:%S] ", timeInfo);

        // Escribe el registro en el archivo
        logFile << timeBuffer << message << "\n";

        logFile.close();  // Cierra el archivo
    } else {
        std::cerr << "Error: No se pudo abrir el archivo de registro 'server.log'.\n";
    }
}

// Función para limpiar el búfer de entrada
void limpiarBuffer() {
    // Limpiar el búfer ignorando todos los caracteres hasta el salto de línea
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//funcion para mostrar contenido del server log
void mostrarServerLog(){

    int bytesRead;

    while ((bytesRead = recv(server, buffer, sizeof(buffer), 0)) > 0) {
    std::cout.write(buffer, bytesRead);
}

}






};
