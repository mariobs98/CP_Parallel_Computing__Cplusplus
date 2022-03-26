//
// Created by mariobs on 27/4/21.
//

#include "Ordenacion_sec.h"

//----------------------------------------------INSERTIONSORT-------------------------------------------------------------------
void sort(int arr[], int l, int n)
{
    int i = l+1, key, j;

    for ( i; i < n; i++) {
        key = arr[i];
        j = i - 1;

        // Move elements of arr[0..i-1],
        // that are greater than key,
        // to one position ahead of
        // their current position
        while (j >= l && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


//--------------------------------------------------BURBUJA-------------------------------------------------------------------------
void burbujaSort(int m[], int i, int highPos) {
    int j, aux;

    for (i; i<highPos-1; i++)
    {
        for (j=i+1; j<highPos; j++)
        {
            if(m[i]>m[j])
            {
                aux = m[i];
                m[i] = m[j];
                m[j] = aux;
            }
        }
    }
}


//--------------------------------------------------QUICKSORT------------------------------------------------------------------------
void intercambio(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

// partition the array using last element as pivot
int particion (int arr[], int low, int high)
{
    int pivot = arr[high];    // pivot
    int i = (low - 1);

    for (int j = low; j <= high- 1; j++)
    {
        //if current element is smaller than pivot, increment the low element
        //swap elements at i and j
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            intercambio(&arr[i], &arr[j]);
        }
    }
    intercambio(&arr[i + 1], &arr[high]);
    return (i + 1);
}

//quicksort
void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        //particion del vector
        int pivot = particion(arr, low, high);

        //ordena los subvectores de forma independiente
        quickSort(arr, low, pivot - 1);
        quickSort(arr, pivot + 1, high);
    }
}