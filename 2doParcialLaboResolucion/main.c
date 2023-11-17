#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int idEquipo;
    char nombreEquipo[30];
} stEquipo;

typedef struct
{
    int IdJugador;
    int NroCamisetaJugador;
    char nombreJugador[30];
    int PuntosAnotados;
} stJugador;

typedef struct nodoJugador
{
    stJugador jugador;
    struct nodoJugador* izq;
    struct nodoJugador* der;
} nodoJugador;

typedef struct nodoEquipo
{
    stEquipo equipos;
    nodoJugador* jugadores;
    struct nodoEquipo* siguiente;
} nodoEquipo;

typedef struct
{
    int idEquipo;
    char nombreEquipo[30];
    int IdJugador;
    int NroCamisetaJugador;
    char nombreJugador[30];
    int PuntosAnotados;
} stJugadorEquipo;

typedef struct nodoListaJugadorEquipo
{
    stJugadorEquipo jugador; /// a fines de recolectar toda la informacion de un jugador, la lista de goleadores tendra la data del equipo
    struct nodoListaJugadorEquipo* siguiente;
} nodoListaJugadorEquipo;

nodoListaJugadorEquipo* crearNodoListaJugadorEquipo(stJugadorEquipo dato)
{
    nodoListaJugadorEquipo* nuevo = (nodoListaJugadorEquipo*)malloc(sizeof(nodoListaJugadorEquipo));
    nuevo->jugador = dato;
    nuevo->siguiente = NULL;
    return nuevo;
}

///Crear Nodo
nodoJugador* crearnodoJugadorrbol(stJugador dato)
{
    nodoJugador* nuevo= (nodoJugador*)malloc(sizeof(nodoJugador));
    nuevo->jugador=dato;
    nuevo->izq=NULL;
    nuevo->der=NULL;
    return nuevo;
}

/// Función para insertar un nodo en el arbol
nodoJugador* insertarEnArbol(nodoJugador* A, stJugador dato)
{
    if (A == NULL)
    {
        return crearnodoJugadorrbol(dato);
    }

    if (dato.IdJugador < A->jugador.IdJugador)
    {
        A->izq = insertarEnArbol(A->izq, dato);
    }
    else if (dato.IdJugador > A->jugador.IdJugador)
    {
        A->der = insertarEnArbol(A->der, dato);
    }

    return A;
}

/// Función para insertar un nodo al final de la lista
nodoEquipo* insertarAlFinal( nodoEquipo* lista, stEquipo equipo)
{
    nodoEquipo* nuevoNodo = (nodoEquipo*)malloc(sizeof(nodoEquipo));
    nuevoNodo->equipos = equipo;
    nuevoNodo->jugadores = NULL;
    nuevoNodo->siguiente = NULL;

    if (lista == NULL)
    {
        return nuevoNodo;
    }

    nodoEquipo* actual = lista;
    while (actual->siguiente != NULL)
    {
        actual = actual->siguiente;
    }

    actual->siguiente = nuevoNodo;
    return lista;
}

/// Función para buscar un nodo por "dato" en el árbol de forma recursiva
nodoJugador* buscarJugadorPorCamiseta(nodoJugador* A, int nroCamiseta) /// modifique esta funcion para buscar por camiseta en todo el arbol, no se beneficia de la busqueda binaria
{
    nodoJugador* jugador = NULL;
    if(A)
    {
        if(A->jugador.NroCamisetaJugador == nroCamiseta)
        {
            jugador = A;
        }
        else
        {
            nodoJugador* jugadorIzq = buscarJugadorPorCamiseta(A->izq, nroCamiseta);
            if(jugadorIzq && jugadorIzq->jugador.NroCamisetaJugador == nroCamiseta)
            {
                jugador = jugadorIzq;
            }
            else
            {
                nodoJugador* jugadorDer = buscarJugadorPorCamiseta(A->der, nroCamiseta);
                if(jugadorDer && jugadorDer->jugador.NroCamisetaJugador == nroCamiseta)
                {
                    jugador = jugadorDer;
                }
            }
        }
    }
    return jugador;
}

/// 1
/// se encarga de buscar un equipo, por nombre, en la lsita de equipos
nodoEquipo* buscarEquipo(nodoEquipo* seg, char* nombreEquipo)
{
    while (seg && strcmpi(seg->equipos.nombreEquipo, nombreEquipo) != 0)
    {
        seg = seg->siguiente;
    }
    return seg;
}

/// recibe la lista de equipos y un registro
nodoEquipo* alta(nodoEquipo* lista, stJugadorEquipo registro)
{
    nodoEquipo* equipo = buscarEquipo(lista, registro.nombreEquipo); // busca el equipo
    stJugador jugador; // todos estos datos los podemos recolectar aca y son necesarios para agregar un jugador
    jugador.IdJugador = registro.IdJugador;
    strcpy(jugador.nombreJugador, registro.nombreJugador);
    jugador.NroCamisetaJugador = registro.NroCamisetaJugador;
    jugador.PuntosAnotados = registro.PuntosAnotados;

    if(equipo == NULL) // si no existe el equipo
    {
        stEquipo nuevoEquipo;
        nuevoEquipo.idEquipo = registro.idEquipo;
        strcpy(nuevoEquipo.nombreEquipo, registro.nombreEquipo);
        lista = insertarAlFinal(lista, nuevoEquipo); // se crea y se agrega el equipo a la lista
        equipo = buscarEquipo(lista, nuevoEquipo.nombreEquipo); // esto no seria necesario si los equipos se insertaran al principio
    }
    equipo->jugadores = insertarEnArbol(equipo->jugadores, jugador);
    return lista;
}

/// abre el archivo y desglosa los registros en la lista de arboles
nodoEquipo* leerArchivo(char* nombreArchivo)
{
    nodoEquipo* lista = NULL;
    FILE* fp = fopen(nombreArchivo, "rb");
    stJugadorEquipo registro;
    if (fp)
    {
        while (fread(&registro, sizeof(stJugadorEquipo), 1, fp))
        {
            lista = alta(lista, registro);
        }
        fclose(fp);
    }
    return lista;
}
/// fin 1

/// 2
int ultimoIdEquipo(nodoEquipo* lista) // existe para prevenir que el usuario ingrese ids
{
    int id = 0; // el id 0 no existe, lo considero invalido
    while (lista)
    {
        if (lista->equipos.idEquipo > id)
        {
            id = lista->equipos.idEquipo;
        }
        lista = lista->siguiente;
    }
    return id;
}

int ultimoIdJugadorDeUnEquipo(nodoJugador* arbol) // auxiliar a la siguiente funcion
{
    int id = 0; // el id 0 no existe, lo considero invalido
    if(arbol)
    {
        if (arbol->der)
        {
            id = ultimoIdJugadorDeUnEquipo(arbol->der);
        }
        else
        {
            id = arbol->jugador.IdJugador;
        }
    }
    return id;
}

int ultimoIdJugador(nodoEquipo* equipos) // existe para prevenir que el usuario ingrese ids
{
    int ultimoID = 0;
    if (equipos)
    {
        ultimoID = ultimoIdJugadorDeUnEquipo(equipos->jugadores); // el ultimo id del primer equipo, mi primer supuesto para comparar
        equipos = equipos->siguiente;
        while (equipos)
        {
            int otroID = ultimoIdJugadorDeUnEquipo(equipos->jugadores); // el ultimo id de otro equipo
            if(otroID > ultimoID) // cual es realmente el ultimo?
            {
                ultimoID = otroID;
            }
            equipos = equipos->siguiente;
        }
    }
    return ultimoID; // el ultimo id o 0 si la lista esta vacia
}

nodoEquipo* altaInput(nodoEquipo* lista)
{
    stJugadorEquipo registro;
    registro.IdJugador = ultimoIdJugador(lista) + 1;
    registro.idEquipo = ultimoIdEquipo(lista) + 1; // si el equipo existe no importa porque no crea un nuevo equipo
    registro.PuntosAnotados = 0; // un inicializador, si se da de alta a un jugador con puntaje existente, deberia pedirse por input
    printf("ingrese el nombre del nuevo jugador\n");
    fflush(stdin);
    gets(registro.nombreJugador);
    printf("ingrese el equipo del nuevo jugador\n");
    fflush(stdin);
    gets(registro.nombreEquipo);
    printf("ingrese el numero de camiseta\n");
    fflush(stdin);
    scanf("%d", &registro.NroCamisetaJugador);
    lista = alta(lista, registro);
    return lista;
}
/// fin 2

/// 3
void mostrarJugadores(nodoJugador* arbol) /// preorder
{
    if (arbol)
    {
        mostrarJugadores(arbol->izq);
        printf("Nombre: %s\n", arbol->jugador.nombreJugador);
        printf("Nro de camiseta: %d\n", arbol->jugador.NroCamisetaJugador);
        printf("Puntos anotados: %d\n", arbol->jugador.PuntosAnotados);
        printf("------------------------------\n\n");
        mostrarJugadores(arbol->der);
    }
}

void mostrarEquiposYJugadores(nodoEquipo* lista) /// muestra el nombre del equipo y todos sus jugadores
{
    while (lista)
    {
        printf("Equipo: %s\n\n", lista->equipos.nombreEquipo);
        mostrarJugadores(lista->jugadores);
        lista = lista->siguiente;
    }
}
/// fin 3

/// 4
int buscarMayorCantDeGoles(nodoJugador* arbol) /// busca la mayor cantidad de goles de entre los jugadores del arbol
{
    int goles = -1; /// como minimo es 0, por tanto -1 es invalido y será pisado a menos que no hayan jugadores
    if(arbol) /// como hay que buscar en todo el arbol no hay ningun beneficio por hacerlo en preorder, inorder o posorder
    {
        int golesIzq = buscarMayorCantDeGoles(arbol->izq);
        int golesDer = buscarMayorCantDeGoles(arbol->der);

        if(arbol->jugador.PuntosAnotados > golesIzq && arbol->jugador.PuntosAnotados > golesDer) /// primero descarto si raiz tiene el mayor dato
        {
            goles = arbol->jugador.PuntosAnotados;
        }
        else if (golesIzq > golesDer) /// sino comparo los datos que provienen de las ramas
        {
            goles = golesIzq;
        }
        else
        {
            goles = golesDer;
        }
    }
    return goles;
}

/// funcion auxiliar para no repetir tantas asignaciones y copias
stJugadorEquipo generarRegistro(stEquipo equipo, stJugador jugador)
{
    stJugadorEquipo nuevo;
    nuevo.idEquipo = equipo.idEquipo;
    nuevo.IdJugador = jugador.IdJugador;
    strcpy(nuevo.nombreEquipo, equipo.nombreEquipo);
    strcpy(nuevo.nombreJugador, jugador.nombreJugador);
    nuevo.NroCamisetaJugador = jugador.NroCamisetaJugador;
    nuevo.PuntosAnotados = jugador.PuntosAnotados;
    return nuevo;
}

nodoListaJugadorEquipo* pasarGoleadoresDeUnEquipoALista(nodoListaJugadorEquipo* lista, nodoJugador* arbol, int mayorCantDeGoles, stEquipo equipo)
{ /// funcion auxiliar que recorre cada arbol
    if(arbol)
    {
        lista = pasarGoleadoresDeUnEquipoALista(lista,arbol->izq,mayorCantDeGoles,equipo);
        if(arbol->jugador.PuntosAnotados == mayorCantDeGoles)
        {
            stJugadorEquipo nuevo = generarRegistro(equipo, arbol->jugador);
            nodoListaJugadorEquipo* nuevoNodo = crearNodoListaJugadorEquipo(nuevo);
            nuevoNodo->siguiente = lista;
            lista = nuevoNodo;
        }
        lista = pasarGoleadoresDeUnEquipoALista(lista,arbol->der,mayorCantDeGoles,equipo);
    }
    return lista;
}

nodoListaJugadorEquipo* pasarGoleadoresALista(nodoEquipo* lista)
{ /// por cada equipo pasara a la lista los jugadores con mas goles de cada equipo
    nodoListaJugadorEquipo* goleadores = NULL;
    while (lista)
    {
        int mayorCantDeGoles = buscarMayorCantDeGoles(lista->jugadores);
        goleadores = pasarGoleadoresDeUnEquipoALista(goleadores, lista->jugadores, mayorCantDeGoles, lista->equipos);
        lista = lista->siguiente;
    }
    return goleadores;
}
/// fin 4

/// 5
int sumaGoles(nodoJugador* arbol)
{
    int goles = 0;
    if(arbol)
    {
        goles = arbol->jugador.PuntosAnotados + sumaGoles(arbol->izq) + sumaGoles(arbol->der);
    }
    return goles;
}

int contarJugadores(nodoJugador* arbol)
{
    int jugadores = 0;
    if(arbol)
    {
        jugadores = 1 + contarJugadores(arbol->izq) + contarJugadores(arbol->der);
    }
    return jugadores;
}

void listarGoleadores(nodoJugador* arbol, int mayorCantDeGoles)
{
    if(arbol)
    {
        listarGoleadores(arbol->izq, mayorCantDeGoles);
        if(arbol->jugador.PuntosAnotados == mayorCantDeGoles)
        {
            printf("%s  | ", arbol->jugador.nombreJugador);
        }
        listarGoleadores(arbol->der, mayorCantDeGoles);
    }
}

void informarEstadisticasPorEquipo(nodoEquipo* lista)
{
    while (lista) // por cada equipo
    {
        int mayorCantDeGoles = buscarMayorCantDeGoles(lista->jugadores); // capturo la mayor cantidad de goles de todos los jugadores
        int totalDeGoles = sumaGoles(lista->jugadores); // capturo el total de goles
        int totalDeJugadores = contarJugadores(lista->jugadores); // capturo el total de jugadores
        float promedioDeGoles = (float)totalDeGoles / totalDeJugadores; // calculo el promedio de goles
        printf("Equipo: %s\n\n", lista->equipos.nombreEquipo); // e informo estadisticas
        printf("Total de goles: %d\n", totalDeGoles);
        printf("Promedio de goles: %.2f\n", promedioDeGoles);
        printf("Goleadores: ");
        listarGoleadores(lista->jugadores, mayorCantDeGoles);
        printf("\n--------------------\n\n");
        lista = lista->siguiente;
    }
}
/// fin 5

/// 6
void pasarJugadoresDeUnaCamisetaAUnArchivo(char* nombreArchivo, nodoEquipo* lista, int nroCamiseta)
{
    FILE* fp = fopen(nombreArchivo, "wb"); // wb para que el archivo siempre este actualizado
    if(fp)
    {
        while (lista)
        {
            nodoJugador* jugador = buscarJugadorPorCamiseta(lista->jugadores, nroCamiseta);
            if (jugador)
            {
                stJugadorEquipo registro = generarRegistro(lista->equipos, jugador->jugador);
                fwrite(&registro, sizeof(stJugadorEquipo), 1, fp);
            }
            lista = lista->siguiente;
        }
        fclose(fp);
    }
}
/// fin 6

/// funciones auxiliares

void mostrarArchivo(char* nombreArchivo)
{
    FILE* fp = fopen(nombreArchivo, "rb");
    if (fp)
    {
        stJugadorEquipo registro;
        while (fread(&registro, sizeof(stJugadorEquipo), 1, fp))
        {
            printf("------------------------------\n\n");
            printf("idEquipo: %d | idJugador: %d\nEquipo: %s | Jugador: %s\nnroCamiseta: %d | puntosAnotados: %d\n\n",
                   registro.idEquipo,registro.IdJugador,registro.nombreEquipo,registro.nombreJugador,registro.NroCamisetaJugador,registro.PuntosAnotados);
        }
        printf("---------------FIN---------------\n\n");
        fclose(fp);
    }
}

/// se podria haber modularizado? si...
/// son mas de debug que otra cosa

void mostrarListaDeGoleadores(nodoListaJugadorEquipo* goleadores)
{
    while (goleadores)
    {
        printf("------------------------------\n\n");
        printf("idEquipo: %d | idJugador: %d\nEquipo: %s | Jugador: %s\nnroCamiseta: %d | puntosAnotados: %d\n\n",
               goleadores->jugador.idEquipo,goleadores->jugador.IdJugador,goleadores->jugador.nombreEquipo,goleadores->jugador.nombreJugador,goleadores->jugador.NroCamisetaJugador,goleadores->jugador.PuntosAnotados);
        goleadores = goleadores->siguiente;
    }
}

///======================================================================================

int main()
{
    printf("ESTADO INICIAL DEL ARCHIVO\n\n");
    system("pause");
    mostrarArchivo("registroJugador.dat");
    system("pause");
    system("cls");
    /// 1
    nodoEquipo* lista = leerArchivo("registroJugador.dat");
    mostrarEquiposYJugadores(lista);
    system("pause");
    system("cls");
    /// 2
    printf("Cargar un jugador desde el teclado\n\n");
    lista = altaInput(lista);
    printf("Jugador agregado al final\n\n");
    system("pause");
    /// 3
    mostrarEquiposYJugadores(lista);
    system("pause");
    system("cls");
    /// 4
    printf("Listado de Goleadores de cada equipo\n\n");
    system("pause");
    nodoListaJugadorEquipo* goleadores = pasarGoleadoresALista(lista);
    mostrarListaDeGoleadores(goleadores);
    system("pause");
    system("cls");
    /// 5
    printf("Informe de estadisticas\n\n");
    system("pause");
    informarEstadisticasPorEquipo(lista);
    system("pause");
    system("cls");
    /// 6
    printf("jugadores con la camiseta 11\n\n");
    pasarJugadoresDeUnaCamisetaAUnArchivo("camisetas11.bin", lista, 11);
    mostrarArchivo("camisetas11.bin");
    system("pause");
    system("cls");
    printf("jugadores con la camiseta 9\n\n");
    pasarJugadoresDeUnaCamisetaAUnArchivo("camisetas9.bin", lista, 9);
    mostrarArchivo("camisetas9.bin");
    system("pause");
    system("cls");
    printf("jugadores con la camiseta 17\n\n");
    pasarJugadoresDeUnaCamisetaAUnArchivo("camisetas17.bin", lista, 17);
    mostrarArchivo("camisetas17.bin");
    system("pause");
    system("cls");

    return 0;
}

///======================================================================================
