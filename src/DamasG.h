//
// Created by chris on 4/12/2023.
//

#ifndef DAMASMINIMAX_DAMASG_H
#define DAMASMINIMAX_DAMASG_H

#define BLANCO 0
#define NEGRA 1
#define VACIO 2

struct Movimiento {
    int fila_inicial, columna_inicial;
    int fila_final, columna_final;
};

struct Ficha {
    bool esReina = false;
    int color = VACIO;  // 0 = Blanca, 1 = Negra, 2 = Vacío.
};

class Damas {
public:
    int turno;  //  0 = Blanca, 1 = Negra.
    int profundidad;
    std::vector<std::vector<Ficha>> tablero;

    explicit Damas(int profundidad, int turno);
};

Damas::Damas(int profundidad, int turno) : profundidad(profundidad), turno(turno), tablero(8, std::vector<Ficha>(8)) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i + j) % 2 == 1) {
                if (i < 3) {
                    tablero[i][j].color = NEGRA;
                } else if (i > 4) {
                    tablero[i][j].color = BLANCO;
                } else {
                    tablero[i][j].color = VACIO;
                }
            } else {
                tablero[i][j].color = VACIO;
            }
        }
    }
}

struct MiniMax {
    // Función principal que llama al algoritmo minimax.
    Movimiento minimax(Damas &estado, int profundidad);

    int minimax_recursivo(Damas &estado, int profundidad, int jugador_actual, int jugador_maximizador);

    // Función auxiliar para generar todos los movimientos posibles para el jugador actual.
    static std::vector<Movimiento> generar_movimientos(Damas &estado);
    static std::vector<Movimiento> generar_movimientos_simple(Damas &estado, Ficha ficha, int fila, int columna);
    static std::vector<Movimiento> generar_movimientos_captura(Damas &estado, Ficha &ficha_actual, int fila, int columna);

    // Función auxiliar para evaluar el estado actual del tablero.
    int evaluar_estado(Damas &estado);

    // Función auxiliar que simula el movimiento de una ficha en el tablero.
    static void hacer_movimiento(Damas &estado, Movimiento movimiento);

    // Función auxiliar que deshace el movimiento de una ficha en el tablero.
    void deshacer_movimiento(Damas &estado, Movimiento movimiento);

    // Función auxiliar que devuelve la ficha en una casilla dada del tablero.
    static Ficha obtener_ficha(Damas &estado, int fila, int columna);

    // Función auxiliar que determina si un movimiento es válido o no.
    static bool es_movimiento_valido(Damas &estado, Movimiento movimiento);

    // Función auxiliar que determina si un jugador ha ganado.
    static bool ha_ganado(Damas &estado, int jugador);

    // Función auxiliar que determina si el juego ha terminado.
    static bool ha_terminado(Damas &estado);
};

bool MiniMax::ha_terminado(Damas &estado) {
    // Si no hay fichas del color del jugador actual en el tablero, el juego ha terminado.
    bool hay_fichas_jugador_actual = false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Ficha ficha = estado.tablero[i][j];
            if (ficha.color == estado.turno) {
                hay_fichas_jugador_actual = true;
                break;
            }
        }
        if (hay_fichas_jugador_actual) {
            break;
        }
    }
    if (!hay_fichas_jugador_actual) {
        return true;
    }

    // Si el jugador actual no puede realizar ningún movimiento, el juego ha terminado.
    std::vector<Movimiento> movimientos = generar_movimientos(estado);
    if (movimientos.empty()) {
        return true;
    }

    // Si el juego no ha terminado, devuelve falso.
    return false;
}

bool MiniMax::ha_ganado(Damas &estado, int jugador) {
    // Si no hay fichas del jugador en el tablero, ha perdido.
    bool hay_fichas_jugador = false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Ficha ficha = estado.tablero[i][j];
            if (ficha.color == jugador) {
                hay_fichas_jugador = true;
                break;
            }
        }
        if (hay_fichas_jugador) {
            break;
        }
    }
    if (!hay_fichas_jugador) {
        return false;
    }

    // Si el otro jugador no tiene fichas, el jugador ha ganado.
    int otro_jugador = (jugador == 0) ? 1 : 0;
    bool hay_fichas_otro_jugador = false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Ficha ficha = estado.tablero[i][j];
            if (ficha.color == otro_jugador) {
                hay_fichas_otro_jugador = true;
                break;
            }
        }
        if (hay_fichas_otro_jugador) {
            break;
        }
    }
    if (!hay_fichas_otro_jugador) {
        return true;
    }

    // Si el jugador tiene todas las fichas del otro jugador, ha ganado.
    int fichas_otro_jugador = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Ficha ficha = estado.tablero[i][j];
            if (ficha.color == otro_jugador) {
                fichas_otro_jugador++;
            }
        }
    }
    if (fichas_otro_jugador == 0) {
        return true;
    }

    // Si el juego no ha terminado o el jugador no ha ganado, devuelve falso.
    return false;
}

bool MiniMax::es_movimiento_valido(Damas &estado, Movimiento movimiento) {
    // Comprueba si el movimiento se realiza dentro de los límites del tablero.
    if (movimiento.fila_inicial < 0 || movimiento.fila_inicial > 7 ||
        movimiento.columna_inicial < 0 || movimiento.columna_inicial > 7 ||
        movimiento.fila_final < 0 || movimiento.fila_final > 7 ||
        movimiento.columna_final < 0 || movimiento.columna_final > 7) {
        return false;
    }

    // Comprueba si el movimiento se realiza en una casilla vacía.
    if (estado.tablero[movimiento.fila_final][movimiento.columna_final].color != VACIO) {
        return false;
    }

    // Comprueba si el jugador mueve su propia ficha.
    Ficha ficha_inicial = estado.tablero[movimiento.fila_inicial][movimiento.columna_inicial];
    if (ficha_inicial.color != estado.turno) {
        return false;
    }

    // Comprueba si el movimiento es en diagonal.
    int fila_diff = movimiento.fila_final - movimiento.fila_inicial;
    int columna_diff = movimiento.columna_final - movimiento.columna_inicial;
    if (abs(fila_diff) != abs(columna_diff)) {
        return false;
    }

    // Comprueba si el movimiento es válido para una ficha normal.
    if (!ficha_inicial.esReina) {
        if (estado.turno == BLANCO && fila_diff > 0) {
            return false;
        }
        if (estado.turno == NEGRA && fila_diff < 0) {
            return false;
        }
        if (abs(fila_diff) == 1 && abs(columna_diff) == 1) {
            return true;
        }
        if (abs(fila_diff) == 2 && abs(columna_diff) == 2) {
            Ficha ficha_intermedia = estado.tablero[movimiento.fila_inicial + fila_diff / 2][movimiento.columna_inicial + columna_diff / 2];
            if (ficha_intermedia.color == VACIO) {
                return false;
            }
            if (ficha_intermedia.color == estado.turno) {
                return false;
            }
            return true;
        }
    }

    // Comprueba si el movimiento es válido para una ficha reina.
    if (ficha_inicial.esReina) {
        int fila_dir = (fila_diff > 0) ? 1 : -1;
        int columna_dir = (columna_diff > 0) ? 1 : -1;
        for (int i = 1; i < abs(fila_diff); i++) {
            Ficha ficha_intermedia = estado.tablero[movimiento.fila_inicial + i * fila_dir][movimiento.columna_inicial + i * columna_dir];
            if (ficha_intermedia.color != VACIO) {
                return false;
            }
        }
        return true;
    }

    // Si el movimiento no se ha considerado válido, devuelve falso.
    return false;
}

Ficha MiniMax::obtener_ficha(Damas &estado, int fila, int columna) {
    return estado.tablero[fila][columna];
}

void MiniMax::deshacer_movimiento(Damas &estado, Movimiento movimiento) {
    // Revertir el movimiento en el tablero
    estado.tablero[movimiento.fila_inicial][movimiento.columna_inicial] = obtener_ficha(estado, movimiento.fila_final, movimiento.columna_final);
    estado.tablero[movimiento.fila_final][movimiento.columna_final] = {false, VACIO};

    // Cambiar el turno
    estado.turno = 1 - estado.turno;
}

void MiniMax::hacer_movimiento(Damas &estado, Movimiento movimiento) {
    // Realizar el movimiento en el tablero
    estado.tablero[movimiento.fila_final][movimiento.columna_final] = obtener_ficha(estado, movimiento.fila_inicial, movimiento.columna_inicial);
    estado.tablero[movimiento.fila_inicial][movimiento.columna_inicial] = {false, VACIO};

    // Verificar si la ficha se convierte en reina
    Ficha &ficha = estado.tablero[movimiento.fila_final][movimiento.columna_final];
    if (ficha.color == BLANCO && movimiento.fila_final == 0) {
        ficha.esReina = true;
    } else if (ficha.color == NEGRA && movimiento.fila_final == 7) {
        ficha.esReina = true;
    }

    // Verificar si se capturó una ficha
    int fila_captura = (movimiento.fila_inicial + movimiento.fila_final) / 2;
    int columna_captura = (movimiento.columna_inicial + movimiento.columna_final) / 2;
    if (std::abs(movimiento.fila_inicial - movimiento.fila_final) == 2) {
        estado.tablero[fila_captura][columna_captura] = {false, VACIO};
    }

    // Cambiar el turno
    estado.turno = 1 - estado.turno;
}

int MiniMax::evaluar_estado(Damas &estado) {
    int puntuacion = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Ficha ficha = obtener_ficha(estado, i, j);
            if (ficha.color == BLANCO) {
                puntuacion += ficha.esReina ? 3 : 1;
            } else if (ficha.color == NEGRA) {
                puntuacion -= ficha.esReina ? 3 : 1;
            }
        }
    }
    return puntuacion;
}

std::vector<Movimiento> MiniMax::generar_movimientos(Damas &estado) {
    std::vector<Movimiento> movimientos;
    int jugador = estado.turno;
    int otro_jugador = 1 - jugador;

    // Recorrer el tablero
    for (int fila = 0; fila < 8; fila++) {
        for (int columna = 0; columna < 8; columna++) {
            Ficha ficha_actual = obtener_ficha(estado, fila, columna);

            // Si la ficha es del jugador en turno
            if (ficha_actual.color == jugador) {
                // Verificar si es posible realizar una captura
                std::vector<Movimiento> movimientos_captura = generar_movimientos_captura(estado, ficha_actual, fila, columna);
                if (!movimientos_captura.empty()) {
                    movimientos.insert(movimientos.end(), movimientos_captura.begin(), movimientos_captura.end());
                } else {
                    // Verificar si es posible realizar un movimiento simple
                    std::vector<Movimiento> movimientos_simple = generar_movimientos_simple(estado, ficha_actual, fila, columna);
                    movimientos.insert(movimientos.end(), movimientos_simple.begin(), movimientos_simple.end());
                }
            }
        }
    }
    return movimientos;
}

std::vector<Movimiento> MiniMax::generar_movimientos_simple(Damas &estado, Ficha ficha, int fila, int columna) {
    std::vector<Movimiento> movimientos;
    int fila_dir = (ficha.color == BLANCO) ? -1 : 1;
    int columna_dir_izq = (ficha.esReina) ? -1 : ((ficha.color == BLANCO) ? -1 : 1);
    int columna_dir_der = (ficha.esReina) ? 1 : ((ficha.color == BLANCO) ? -1 : 1);

    // Verificar si es posible realizar un movimiento hacia adelante a la izquierda
    if (fila + fila_dir >= 0 && fila + fila_dir < 8 && columna + columna_dir_izq >= 0 && columna + columna_dir_izq < 8) {
        Ficha ficha_destino = obtener_ficha(estado, fila + fila_dir, columna + columna_dir_izq);
        if (ficha_destino.color == VACIO) {
            Movimiento movimiento = {fila, columna, fila + fila_dir, columna + columna_dir_izq};
            movimientos.push_back(movimiento);
        }
    }

    // Verificar si es posible realizar un movimiento hacia adelante a la derecha
    if (fila + fila_dir >= 0 && fila + fila_dir < 8 && columna + columna_dir_der >= 0 && columna + columna_dir_der < 8) {
        Ficha ficha_destino = obtener_ficha(estado, fila + fila_dir, columna + columna_dir_der);
        if (ficha_destino.color == VACIO) {
            Movimiento movimiento = {fila, columna, fila + fila_dir, columna + columna_dir_der};
            movimientos.push_back(movimiento);
        }
    }

    // Si la ficha es una reina, verificar si es posible realizar un movimiento hacia atrás a la izquierda
    if (ficha.esReina) {
        if (fila - fila_dir >= 0 && fila - fila_dir < 8 && columna + columna_dir_izq >= 0 && columna + columna_dir_izq < 8) {
            Ficha ficha_destino = obtener_ficha(estado, fila - fila_dir, columna + columna_dir_izq);
            if (ficha_destino.color == VACIO) {
                Movimiento movimiento = {fila, columna, fila - fila_dir, columna + columna_dir_izq};
                movimientos.push_back(movimiento);
            }
        }

        // Verificar si es posible realizar un movimiento hacia atrás a la derecha
        if (fila - fila_dir >= 0 && fila - fila_dir < 8 && columna + columna_dir_der >= 0 && columna + columna_dir_der < 8) {
            Ficha ficha_destino = obtener_ficha(estado, fila - fila_dir, columna + columna_dir_der);
            if (ficha_destino.color == VACIO) {
                Movimiento movimiento = {fila, columna, fila - fila_dir, columna + columna_dir_der};
                movimientos.push_back(movimiento);
            }
        }
    }

    return movimientos;
}

std::vector<Movimiento>
MiniMax::generar_movimientos_captura(Damas &estado, Ficha &ficha_actual, int fila, int columna) {
    std::vector<Movimiento> movimientos;
    int color_actual = ficha_actual.color;
    int direccion = (color_actual == BLANCO) ? -1 : 1;
    int distancia = (ficha_actual.esReina) ? 8 : 2;
    int fila_destino, columna_destino;

    // Comprobar las capturas en diagonal
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            // Comprobar si hay una ficha en diagonal
            fila_destino = fila + direccion * i;
            columna_destino = columna + j;
            int fila_intermedia = fila + direccion * i / 2;
            int columna_intermedia = columna + j / 2;
            if (fila_destino < 0 || fila_destino >= 8 || columna_destino < 0 || columna_destino >= 8) {
                continue;
            }
            Ficha ficha_destino = obtener_ficha(estado, fila_destino, columna_destino);
            if (ficha_destino.color == color_actual || ficha_destino.color == VACIO) {
                continue;
            }
            // Comprobar si se puede capturar la ficha en diagonal
            if (obtener_ficha(estado, fila_intermedia, columna_intermedia).color == 1 - color_actual) {
                Movimiento movimiento = {fila, columna, fila_destino, columna_destino};
                // Si la ficha llega al otro extremo del tablero, coronarla
                if (fila_destino == 0 || fila_destino == 7) {
                    ficha_actual.esReina = true;
                }
                movimientos.push_back(movimiento);
            }
        }
    }
    return movimientos;
}

Movimiento MiniMax::minimax(Damas &estado, int profundidad) {
    // Generar todos los movimientos posibles para el jugador actual.
    std::vector<Movimiento> movimientos_posibles = generar_movimientos(estado);

    // Inicializar los valores mínimo y máximo.
    int mejor_valor = std::numeric_limits<int>::min();
    int peor_valor = std::numeric_limits<int>::max();

    // Seleccionar el jugador que maximiza o minimiza el valor, dependiendo del turno.
    int jugador_maximizador = estado.turno;
    int jugador_minimizador = (estado.turno == 0) ? 1 : 0;

    // Inicializar el mejor movimiento con el primer movimiento posible.
    Movimiento mejor_movimiento = movimientos_posibles[0];

    // Para cada movimiento posible, simular el movimiento en una copia del estado actual,
    // evaluar el nuevo estado y actualizar el mejor movimiento si es necesario.
    for (Movimiento movimiento : movimientos_posibles) {
        // Crear una copia del estado actual.
        Damas estado_nuevo = estado;

        // Simular el movimiento en la copia del estado.
        hacer_movimiento(estado_nuevo, movimiento);

        // Evaluar el nuevo estado.
        int valor_actual = minimax_recursivo(estado_nuevo, profundidad - 1, jugador_minimizador, jugador_maximizador);

        // Actualizar el mejor movimiento si es necesario.
        if ((estado.turno == 0 && valor_actual > mejor_valor) || (estado.turno == 1 && valor_actual < mejor_valor)) {
            mejor_movimiento = movimiento;
            mejor_valor = valor_actual;
        }

        // Actualizar el peor valor si es necesario.
        if ((estado.turno == 0 && valor_actual < peor_valor) || (estado.turno == 1 && valor_actual > peor_valor)) {
            peor_valor = valor_actual;
        }
    }

    // Devolver el mejor movimiento encontrado.
    return mejor_movimiento;
}

int MiniMax::minimax_recursivo(Damas &estado, int profundidad, int jugador_actual, int jugador_maximizador) {
    // Si hemos llegado a la profundidad máxima o el juego ha terminado, evaluamos el estado actual del tablero.
    if (profundidad == 0 || ha_terminado(estado)) {
        return evaluar_estado(estado);
    }

    // Generamos los movimientos posibles para el jugador actual.
    std::vector<Movimiento> movimientos = generar_movimientos(estado);

    // Si no hay movimientos posibles, evaluamos el estado actual del tablero.
    if (movimientos.empty()) {
        return evaluar_estado(estado);
    }
    int mejor_valor;
    if (jugador_actual == jugador_maximizador) {
        mejor_valor = INT_MIN;
        for (auto movimiento : movimientos) {
            // Si el movimiento no es válido, lo saltamos.
            if (!es_movimiento_valido(estado, movimiento)) {
                continue;
            }

            // Hacemos el movimiento y llamamos recursivamente a la función minimax.
            hacer_movimiento(estado, movimiento);
            int valor = minimax_recursivo(estado, profundidad - 1, (jugador_actual + 1) % 2, jugador_maximizador);
            deshacer_movimiento(estado, movimiento);

            // Actualizamos el mejor valor.
            if (valor > mejor_valor) {
                mejor_valor = valor;
            }
        }
    } else {
        mejor_valor = INT_MAX;
        for (auto movimiento : movimientos) {
            // Si el movimiento no es válido, lo saltamos.
            if (!es_movimiento_valido(estado, movimiento)) {
                continue;
            }

            // Hacemos el movimiento y llamamos recursivamente a la función minimax.
            hacer_movimiento(estado, movimiento);
            int valor = minimax_recursivo(estado, profundidad - 1, (jugador_actual + 1) % 2, jugador_maximizador);
            deshacer_movimiento(estado, movimiento);

            // Actualizamos el mejor valor.
            if (valor < mejor_valor) {
                mejor_valor = valor;
            }
        }
    }
    return mejor_valor;
}

#endif //DAMASMINIMAX_DAMASG_H
