#include "Ordenacion.cl"

__kernel void med_ocl(__global int *input, __global int *output, int columnas, __global int *matrizCopia){
    size_t id = get_global_id(0);
    size_t NumWorkItems=get_global_size(0);

    int w=(columnas+1)/NumWorkItems,
        i=id*columnas*w,  //inicio de la fila
        contador=0,
        highPos=i+columnas, //ultimo elemento de la fila hasta la que tengo que ordenar
        posMediana=columnas/2,  //posicion de la mediana
        totalSize=columnas*(columnas+1); //tamano total de la matriz

    //printf(" Medianas : \n");
    for(i; i < (id+1)*columnas*w; i+=columnas){

        sort(input, i, highPos);
        printf("%d ", input[(highPos-1)-posMediana]);   //print de la mediana de cada fila conforme la calculo

        //calculo de multiplos
        for(int p=0; p < totalSize; p++){   //recorro la matriz y para cada elemnto de la matriz, compruebo con mi mediana que he calculado en el WI
            if(input[(highPos-1)-posMediana] == 0){
                    output[i/columnas] = 0;
            }else {
                if(matrizCopia[p]%input[(highPos-1)-posMediana] == 0){   //comprobacion si es multiplo
                    output[i/columnas]+=1; //si es multiplo, sumo a su posicion
                }
            }
        }
    highPos+=columnas;
    }

}//end

__kernel void med_ocl_mpi(__global int *input, __global int *output, int columnas, __global int *matrizCopia, int filas){
    size_t id = get_global_id(0);
    size_t NumWorkItems=get_global_size(0);

    int w=(filas)/NumWorkItems,
        i=id*columnas*w,  //inicio de la fila
        contador=0,
        highPos=i+columnas, //ultimo elemento de la fila hasta la que tengo que ordenar
        posMediana=columnas/2,  //posicion de la mediana
        totalSize=columnas*(columnas+1); //tamano total de la matriz

    for(i; i < (id+1)*columnas*w; i+=columnas){

        sort(input, i, highPos);
        printf("%d ", input[(highPos-1)-posMediana]);   //print de la mediana de cada fila conforme la calculo

        //calculo de multiplos
        for(int p=0; p < totalSize; p++){   //recorro la matriz y para cada elemnto de la matriz, compruebo con mi mediana que he calculado en el WI
            if(input[(highPos-1)-posMediana] == 0){
                output[ (i/columnas)%filas]=0;
            }else {
                if(matrizCopia[p]%input[(highPos-1)-posMediana] == 0){   //comprobacion si es multiplo
                    output[ (i/columnas)%filas]+=1; //si es multiplo, sumo a su posicion
                }
            }
        }
        highPos+=columnas;
    }

}//end
