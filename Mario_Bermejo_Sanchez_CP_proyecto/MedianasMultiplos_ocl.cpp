#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>
#include <CL/cl.h>
#include "ModulosOpenCL.h"


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

int ObtenerParametros(int argc, char *argv[], int *debug, int *num_workitems, int *workitems_por_workgroups) {
	int i;
	*debug=0;
	*num_workitems=0;
	*workitems_por_workgroups=0;
	if (argc<2)
		return 0;
	for (i=2; i<argc;) {
		if (strcmp(argv[i], "-d")==0) {
			*debug=1;
			i++;
		}
		else if (strcmp(argv[i], "-wi")==0) {
			i++;
			if (i==argc)
				return 0;
			*num_workitems=atoi(argv[i]);
			i++;
			if (*num_workitems<=0)
				return 0;
		}
		else if (strcmp(argv[i], "-wi_wg")==0) {
			i++;
			if (i==argc)
				return 0;
			*workitems_por_workgroups=atoi(argv[i]);
			i++;
			if (*workitems_por_workgroups<=0)
				return 0;
		}
		else
			return 0;
	}
	return 1;
}

typedef struct {
	cl_platform_id *plataformas;
	cl_device_id *dispositivos;
	cl_context contexto;
	cl_command_queue cola;
	cl_program programa;
	cl_kernel kernel;
} EntornoOCL_t;

// **************************************************************************
// ***************************** IMPLEMENTACI�N *****************************
// **************************************************************************

cl_int InicializarEntornoOCL(EntornoOCL_t *entorno) {
    cl_uint num_plataformas,num_dispositivos;
    cl_int error;

    //plataforma
    ObtenerPlataformas(entorno->plataformas, num_plataformas);
    //dispositivos
    ObtenerDispositivos(entorno->plataformas[0], CL_DEVICE_TYPE_ALL, entorno->dispositivos, num_dispositivos);
    //contexto
    CrearContexto(entorno->plataformas[0], entorno->dispositivos, num_dispositivos, entorno->contexto);

    //cola
    entorno->cola = clCreateCommandQueue(entorno->contexto, entorno->dispositivos[0], CL_QUEUE_PROFILING_ENABLE, &error);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    //programa
    //si le metemos -g, desactiva la autovectorizacion
    CrearPrograma(entorno->programa, entorno->contexto, num_dispositivos, entorno->dispositivos, "", "kernel.cl");

    //kernel
    entorno->kernel = clCreateKernel(entorno->programa, "med_ocl", &error);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}
}

cl_int LiberarEntornoOCL(EntornoOCL_t *entorno) {
    cl_int error;

    error = clReleaseContext (entorno->contexto);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    error =clReleaseCommandQueue(entorno->cola);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    error = clReleaseProgram(entorno->programa);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    error = clReleaseKernel(entorno->kernel);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    error = clReleaseDevice(entorno->dispositivos[0]);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}
}

//Control de Work Items, si el numero de work items no es multiplo del numero de filas, se avisa del fallo, y se buscara un numero multiplo
//que se ajuste al experimento. Si wi es mayor que el numero de filas, se decrecera este valor, si no, se aumentara.
//El caso de introducir un numero mayor al numero de filas, es poco probable debido a que lanzamos este problema a la cpu del cluster con un
//hardware especifico limitado.
void ControlDeWI(int &columnas, int &num_workitems, int ne){

    if( ((columnas+1)%num_workitems) != 0){
        printf("El experimento %i, y los siguientes, pueden fallar debido a que el numero de work items no es multiplo del numero de filas\n", ne);
        printf("Filas: %i  -- Work Items: %i \n", (columnas+1), num_workitems);
        printf("Se ajustara el numero de workitems al siguiente multiplo encontrado posible\n");

        while( ((columnas+1)%num_workitems) != 0){
            if(num_workitems <= (columnas+1)) {
                num_workitems += 1;
            }else{
                num_workitems -= 1;
            }
        }
        printf("Work Items nuevo: %i \n\n", num_workitems);
    }
}

/*
columnas -> N�mero de columnas de la matriz. El n�mero de filas de la matriz y tama�o del vector resultado es igual a columnas+1
m -> Matriz de tama�o nxn
v -> Vector resultado de tama�o columnas+1
debug -> Bandera para depuraci�n
ne -> N�mero del experimento en ejecuci�n (para depuraci�n)
entorno -> Entorno OpenCL
num_workitems -> N�mero de work items que se usar�n para lanzar el kernel. Es opcional, se puede usar o no dentro de la funci�n
workitems_por_workgroups -> N�mero de work items que se lanzar�n en cada work group. Es opcional, se puede usar o no dentro de la funci�n
*/
cl_int ocl(int columnas,int *m,int *v, int debug, int ne, EntornoOCL_t entorno, int num_workitems, int workitems_por_workgroups) {

    //Control de errores, arriba explicado en la funcion
    ControlDeWI(columnas, num_workitems, ne);

    int i,
        totalSize=columnas*(columnas+1), // tamano total de la matriz.
        contador=0,
        tamOut=(columnas+1); //tamano del vector salida, filas.

    cl_int error;
    size_t NumWorkItems = num_workitems;
    cl_mem buff_in, buff_out, buff_matrizCopia;
    int *in, *out, *matrizCopia;
    in=m;  //meto en el buffer in la matriz m   (la direccion de memoria)
    out=new int[tamOut];
    matrizCopia=new int[totalSize];

    //memcpy(&matrizCopia, &m, totalsize*sizeof(int));  //No funciona
    for(int p=0;p<totalSize;p++){   //copio la matriz original  a una matriz copia para tener con que comparar en el kernel, debido a que varios WI
        matrizCopia[p] = m[p];      //que estan interactuando con la matriz original ordenando las filas y hay problemas
    }

    cl_event EventoExec, EventoEnvio, EventoRecepcion;

    //inicio la salida entera a 0, para evitar errores
    for(i=0; i<tamOut;i++){
        out[i]=0;
    }

    //creacion buffers
    buff_in=clCreateBuffer(entorno.contexto, CL_MEM_USE_HOST_PTR, totalSize*sizeof(cl_int), in, &error);   //cambiar copy por use
    if (error!=CL_SUCCESS){CodigoError(error);return error;}
    buff_out=clCreateBuffer(entorno.contexto, CL_MEM_USE_HOST_PTR, tamOut*sizeof(cl_int), out, &error);     //cambiar copy por use
    if (error!=CL_SUCCESS){CodigoError(error);return error;}
    buff_matrizCopia=clCreateBuffer(entorno.contexto, CL_MEM_USE_HOST_PTR, totalSize*sizeof(cl_int), matrizCopia, &error);   //cambiar copy por use
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    //pasar argumentos al kernel
    error=clSetKernelArg(entorno.kernel,0,sizeof(cl_mem),&buff_in);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}
    error=clSetKernelArg(entorno.kernel,1,sizeof(cl_mem),&buff_out);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}
    error=clSetKernelArg(entorno.kernel,2,sizeof(int),&columnas);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}
    error=clSetKernelArg(entorno.kernel,3,sizeof(cl_mem),&buff_matrizCopia);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    //al cambiar copy por use, comentar esto
//    error=clEnqueueWriteBuffer(entorno.cola, buff_in, false, 0, totalSize*sizeof(cl_int), in, 0, NULL, &EventoEnvio);
//    if (error!=CL_SUCCESS){CodigoError(error);return error;}
//    error=clEnqueueWriteBuffer(entorno.cola, buff_matrizCopia, false, 0, totalSize*sizeof(cl_int), matrizCopia, 0, NULL, &EventoEnvio);
//    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    //ejecucion kernel
    error = clEnqueueNDRangeKernel (entorno.cola, entorno.kernel, 1, NULL, &NumWorkItems, NULL, 0, NULL, &EventoExec);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    //al cambiar copy por use, comentar esto
//    error=clEnqueueReadBuffer(entorno.cola, buff_out, false, 0, tamOut*sizeof(cl_int), out, 0, NULL, &EventoRecepcion);
//    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    error=clFinish(entorno.cola);
    if (error!=CL_SUCCESS){CodigoError(error);return error;}

    //meter en salida el numero de multiplos
    for(i=0; i<tamOut;i++){
        v[i] = out[i];
    }

}
// **************************************************************************
// *************************** FIN IMPLEMENTACI�N ***************************
// **************************************************************************

/*
Recibir� los siguientes par�metros (los par�metros entre corchetes son opcionales): fichEntrada [-d] [-wi work_items] [-wi_wg workitems_por_workgroup]
fichEntrada -> Obligatorio. Fichero de entrada con los par�metros de lanzamiento de los experimentos
-d -> Opcional. Si se indica, se mostrar�n por pantalla los valores iniciales, finales y tiempo de cada experimento
-wi work_items -> Opcional. Si se indica, se lanzar�n tantos work items como se indique en work_items (para OpenCL)
-wi_wg workitems_por_workgroup -> Opcional. Si se indica, se lanzar�n tantos work items en cada work group como se indique en WorkItems_por_WorkGroup (para OpenCL)
*/

int main(int argc,char *argv[]) {
	int i,
		debug,				   		// Indica si se desean mostrar los tiempos y resultados parciales de los experimentos
		num_workitems=0, 		   	// N�mero de work items que se utilizar�n
		workitems_por_workgroups=0, // N�mero de work items por cada work group que se utilizar�n
		num_problems,	  		   	// n�mero de problemas
		columnas,	  			   	// Columnas de la matriz. Debe ser impar
		filas, 		  			   	// Filas de la matriz. Es igual al n�mero de columnas m�s uno.
		*m,					   		// Matriz (ser� de tama�o nxn)
		*v,					   		// Vector soluci�n (ser� de tama�o n)
		seed, 				   		// Semilla para la generaci�n de n�meros aleatorios
		lower_value,  			   	// L�mite inferior para los valores de la matriz
		upper_value; 			   	// L�mite superior para los valores de la matriz
	long long ti,tf,tt=0;		   	// Tiempos inicial, final y total
	EntornoOCL_t entorno;		   	// Entorno para el control de OpenCL
	FILE *f;					   	// Fichero de entrada con los datos de los experimentos

	cl_int error;//recogida de error de funcion ocl
	
	if (!ObtenerParametros(argc, argv, &debug, &num_workitems, &workitems_por_workgroups)) {
		printf("Ejecuci�n incorrecta\nEl formato correcto es %s fichEntrada [-d] [-wi work_items] [-wi_wg workitems_por_workgroup]\n", argv[0]);
		return 0;
	}

	InicializarEntornoOCL(&entorno);

	// Se lee el n�mero de experimentos a realizar
	f = fopen(argv[1], "r");
	fscanf(f, "%d", &num_problems);
	
	for(i=0;i<num_problems;i++)
	{
		//Por cada experimento se leen
		fscanf(f, "%d",&columnas);		//El n�mero de columnas. Debe ser impar
		filas=columnas+1;				//El n�mero de filas es igual al n�mero de columnas +1
		fscanf(f, "%d",&seed);			//La semilla para la generaci�n de n�meros aleatorios
		fscanf(f, "%d",&lower_value);	//El valor m�nimo de para la generaci�n de valores aleatorios de la matriz m (se generan entre lower_value+1 y upper_value-1)
		fscanf(f, "%d",&upper_value);	//El valor m�ximo de para la generaci�n de valores aleatorios de la matriz m
		if (columnas%2==0) {
			printf("El experimento %d no puede realizarse por no ser la dimensi�n de la matriz impar\n", i);
			continue;
		}

		// Reserva de memoria para la matriz y el vector resultado
		m=new int[filas*columnas];
		v = new int[filas];

		//Simplemente una inicializacion a 0 de v para evitar posibles problemas
        for(int p=0; p<filas;p++){
            v[p]=0;
        }

		srand(seed);
		inicializematrix(filas*columnas,m,lower_value,upper_value);
		if (debug) {
			printf("\nMatriz del experimento %d:\n", i);
			escribirmatrix(columnas,m);
		}

		ti=mseconds(); 
		error = ocl(columnas,m,v,debug,i,entorno, num_workitems, workitems_por_workgroups);
		tf=mseconds(); 
		tt+=tf-ti;

		if (debug)	{
			printf("\nTiempo del experimento %d: %Ld ms\n\nResultado del experimento %d:\n", i, tf-ti,i);
			escribirvector(filas,v);
		}
		delete[] v;
		delete[] m;
	}
	LiberarEntornoOCL(&entorno);
	fclose(f);
	printf("\nTiempo total de %d experimentos: %Ld ms\n", num_problems, tt);
	
	return 0;
}
