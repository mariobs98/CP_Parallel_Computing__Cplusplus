//
// Created by mariobs on 27/4/21.
//

#ifndef MEDIANASMULTIPLOS_1_ORDENACION_H
#define MEDIANASMULTIPLOS_1_ORDENACION_H

//insertionsort
void sort(int arr[], int l, int n);

//burbuja
void burbujaSort(int m[], int i, int highPos);

//quicksort
void intercambio(int* a, int* b);
int particion (int arr[], int low, int high);
void quickSort(int arr[], int low, int high);


#endif //MEDIANASMULTIPLOS_1_ORDENACION_H
