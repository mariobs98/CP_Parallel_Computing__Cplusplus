#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>
#include "Ordenacion_sec.h"

using namespace std;

//Inicializa la matriz m de tama�o nxn en el rango (lv, uv)
void inicializematrix(int n,int *m,int lv,int uv)
{
	for(int i=0;i<n;i++)
		m[i]=(int) (((1.*rand())/RAND_MAX)*(uv-lv)+lv);
}

//Muestra la matriz m de tama�o nxn
void escribirmatrix(int n,int *m){
	for (int i=0; i<n+1; i++)
	{
		for(int j=0; j<n; j++)
			cout << m[i*n+j]<<" ";
		cout<<endl;
	}
	cout <<endl;
}

//Muestra el vector v de tama�o n
void escribirvector(int n,int *v)
{
	for(int i=0;i<n;i++)
		cout  <<v[i]<<" ";
	cout <<endl;
}

/*
c
c     mseconds - returns elapsed milliseconds since Jan 1st, 1970.
c
*/
long long mseconds(){
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000 + t.tv_usec/1000;
}
 
int ObtenerParametros(int argc, char *argv[], int *debug) {
	int i;
	*debug=0;
	if (argc<2)
		return 0;
	for (i=2; i<argc;) {
		if (strcmp(argv[i], "-d")==0) {
			*debug=1;
			i++;
		}
		else
			return 0;
	}
	return 1;
}

// **************************************************************************
// ***************************** IMPLEMENTACI�N *****************************
// **************************************************************************
/*
Implementaci�n secuencial
columnas -> N�mero de columnas de la matriz. El n�mero de filas de la matriz y tama�o del vector resultado es igual a columnas+1
m -> Matriz de tama�o nxn
v -> Vector resultado de tama�o columnas+1
debug -> Bandera para depuraci�n
ne -> N�mero del experimento en ejecuci�n (para depuraci�n)
*/
void sec(int columnas,int *m,int *v, int debug, int ne) {

    int i,
        totalSize=columnas*(columnas+1),        //totalsize = filas por columnas;
        highPos=columnas,   //Posicion mas alta de la fila
        posMediana=columnas/2,  //Posicion en la que se encuentra la mediana
        contador=0,
        medianas[columnas+1];   //Vector para guardar la mediana de cada fila

    //Ordeno las filas de la matriz con el metodo quicksort, a la vez guardo y muestro las medianas de cada fila.
    printf(" Medianas del experimento %d: \n", ne);
    for(i=0; i < totalSize; i+=columnas){
        //Distintos algoritmos de ordenacion
        //quickSort(m, i, highPos-1); //ordenacion de la fila de la matriz por metodo de quickSort
        //burbujaSort(m, i, highPos);

        sort(m, i, highPos);

        printf("%d ", medianas[contador]=m[(highPos-1)-posMediana]);   //print de la mediana de cada fila conforme la calculo

        highPos+=columnas;
        contador++;     //iterador del vector medianas
    }

    //calculo de multiplos
    for(i=0; i < totalSize; i++){   //recorro la matriz
        for(contador=0; contador<columnas+1;contador++){    //para cada elemento de la matriz, compruebo si es multiplo de alguna mediana
            if(medianas[contador] == 0){
                v[contador] = 0;
            }else {
                if (m[i] % medianas[contador] == 0) {   //comprobacion si es multiplo
                    v[contador] += 1; //si es multiplo, sumo a su posicion
                }
            }
        }
    }

}
// **************************************************************************
// *************************** FIN IMPLEMENTACI�N ***************************
// **************************************************************************

/*
Recibir� los siguientes par�metros (los par�metros entre corchetes son opcionales): fichEntrada [-d]
fichEntrada -> Obligatorio. Fichero de entrada con los par�metros de lanzamiento de los experimentos
-d -> Opcional. Si se indica, se mostrar�n por pantalla los valores iniciales, finales y tiempo de cada experimento
*/
int main(int argc,char *argv[]) {
	int debug,		  	  // Indica si se desean mostrar los tiempos y resultados parciales de los experimentos
		num_problems, 	  // N�mero de problemas
		columnas,	  	  // Columnas de la matriz. Debe ser impar
		filas, 		  	  // Filas de la matriz. Es igual al n�mero de columnas m�s uno.
		*m,			  	  // Matriz (ser� de tama�o nxn)
		*v,			  	  // Vector soluci�n (ser� de tama�o n)
		seed, 		  	  // Semilla para la generaci�n de n�meros aleatorios
		lower_value,  	  // L�mite inferior para los valores de la matriz
		upper_value; 	  // L�mite superior para los valores de la matriz
	long long ti,tf,tt=0; // Tiempos inicial, final y total
	FILE *f;			  // Fichero de entrada con los datos de los experimentos
	
	if (!ObtenerParametros(argc, argv, &debug)) {
		printf("Ejecuci�n incorrecta\nEl formato correcto es %s fichEntrada [-d]\n", argv[0]);
		return 0;
	}

	// Se lee el n�mero de experimentos a realizar
	f = fopen(argv[1], "r");
	fscanf(f, "%d", &num_problems);
	for(int i=0;i<num_problems;i++)
	{
		//Por cada experimento se leen
		fscanf(f, "%d",&columnas);		//El n�mero de columnas. Debe ser impar
		filas=columnas+1;				//El n�mero de filas es igual al n�mero de columnas +1
		fscanf(f, "%d",&seed);			//La semilla para la generaci�n de n�meros aleatorios
		fscanf(f, "%d",&lower_value);	//El valor m�nimo de para la generaci�n de valores aleatorios de la matriz m (se generan entre lower_value+1 y upper_value-1)
		fscanf(f, "%d",&upper_value);	//El valor m�ximo de para la generaci�n de valores aleatorios de la matriz m
		if (columnas%2==0) {
			printf("El experimento %d no puede realizarse por no ser el n�mero de columnas de la matriz impar\n", i+1);
			continue;
		}

		// Reserva de memoria para la matriz y el vector resultado
		m=new int[filas*columnas];
		v = new int[filas];

		srand(seed);
		inicializematrix(filas*columnas,m,lower_value,upper_value);
		if (debug) {
			printf("\nMatriz del experimento %d:\n", i);
			escribirmatrix(columnas,m);
		}

		ti=mseconds(); 
		sec(columnas,m,v,debug,i);
		tf=mseconds(); 
		tt+=tf-ti;

		if (debug)	{
			printf("\nTiempo del experimento %d: %Ld ms\n\nResultado del experimento %d:\n", i, tf-ti,i);
			escribirvector(filas,v);
		}
		delete[] v;
		delete[] m;
	}
	fclose(f);
	printf("\nTiempo total de %d experimentos: %Ld ms\n", num_problems, tt);
	
	return 0;
}
