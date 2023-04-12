#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <sstream>
#include <io.h>
#include <fcntl.h>
#include "src/DamasG.h"



// Función que imprime el tablero en consola.
void imprimir_tablero(Damas &estado) {
    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << "  0 1 2 3 4 5 6 7" << std::endl;
    for (int i = 0; i < 8; i++) {
        std::wcout << i << " ";
        for (int j = 0; j < 8; j++) {
            if (estado.tablero[i][j].color == VACIO) {
                std::wcout << "- ";
            } else if (estado.tablero[i][j].color == BLANCO) {
                if (estado.tablero[i][j].esReina) {
                    std::wcout << L"\u2665 ";
                } else {
                    std::wcout << L"\u25A1 ";
                }
            } else {
                if (estado.tablero[i][j].esReina) {
                    std::wcout << L"\u2663 ";
                } else {
                    std::wcout << L"\u25A0 ";
                }
            }
        }
        std::wcout << i << std::endl;
    }
    std::wcout << "  0 1 2 3 4 5 6 7" << std::endl;
}

// Función que pide al usuario un movimiento y lo devuelve.
Movimiento pedir_movimiento(Damas &estado) {
    int fila_inicial, columna_inicial, fila_final, columna_final;
    std::wcout << "Introduce las coordenadas de la ficha que deseas mover: ";
    std::cin >> fila_inicial >> columna_inicial;
    std::wcout << "Introduce las coordenadas de la casilla a la que deseas mover la ficha: ";
    std::cin >> fila_final >> columna_final;
    return {fila_inicial, columna_inicial, fila_final, columna_final};
}

// Función que juega el turno del usuario.
void jugar_turno_usuario(Damas &estado, bool &continuar) {
    imprimir_tablero(estado);
    Movimiento movimiento = pedir_movimiento(estado);
    if (!MiniMax::es_movimiento_valido(estado, movimiento)) {
        std::wcout << "Movimiento invalido, intenta de nuevo." << std::endl;
        jugar_turno_usuario(estado, continuar);
        return;
    }
    MiniMax::hacer_movimiento(estado, movimiento);
    if (MiniMax::ha_ganado(estado, BLANCO)) {
        std::wcout << "Ganaste el juego!" << std::endl;
        continuar = false;
        return;
    }
    if (MiniMax::ha_terminado(estado)) {
        std::wcout << "Empate!" << std::endl;
        continuar = false;
        return;
    }
}

// Función que juega el turno del bot.
void jugar_turno_bot(Damas &estado, bool &continuar) {
    imprimir_tablero(estado);
    MiniMax bot;
    Movimiento movimiento = bot.minimax(estado, estado.profundidad);
    std::wcout << "El bot mueve la ficha de (" << movimiento.fila_inicial << ", " << movimiento.columna_inicial << ") a ("
         << movimiento.fila_final << ", " << movimiento.columna_final << ")" << std::endl;
    MiniMax::hacer_movimiento(estado, movimiento);
    if (MiniMax::ha_ganado(estado, NEGRA)) {
        std::wcout<< "El bot gana el juego!" << std::endl;
        continuar = false;
        return;
    }
    if (MiniMax::ha_terminado(estado)) {
        std::wcout<< "Empate!" << std::endl;
        continuar = false;
        return;
    }
}

int main() {
    bool continuar = true;
    int profundidad;
    bool inicia;
    std::cout << "Bienvenido a las Damas!" << std::endl;
    std::cout << "Ingrese la profundidad del arbol de busqueda:";
    std::cin >> profundidad;
    std::cout << "Inicia el BOT: 1\nInicia el USUARIO: 0\n:";
    std::cin >> inicia;

    Damas estado(profundidad, (inicia ? NEGRA : BLANCO));

    while (continuar) {
        if (estado.turno == BLANCO) {
            jugar_turno_usuario(estado, continuar);
            estado.turno = NEGRA;
        } else {
            jugar_turno_bot(estado, continuar);
            estado.turno = BLANCO;
        }
    }

    return 0;
}