//
// Created by mariobs on 26/4/21.
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>
#include <CL/cl.h>


#ifndef MEDIANASMULTIPLOS_1_MODULOSOPENCL_H
#define MEDIANASMULTIPLOS_1_MODULOSOPENCL_H


cl_int ObtenerPlataformas(cl_platform_id *&platforms, cl_uint &num_platforms);

cl_int ObtenerDispositivos(cl_platform_id platform, cl_device_type device_type, cl_device_id *&devices, cl_uint &num_devices);

cl_int CrearContexto(cl_platform_id platform, cl_device_id *devices,cl_uint num_devices, cl_context &contexto);

cl_int CrearPrograma(cl_program &program, cl_context context,cl_uint num_devices, const cl_device_id *device_list, const char *options, const char *fichero);

char leerFuentes(char *&src, const char *fileName);

void CodigoError(cl_int err);


#endif //MEDIANASMULTIPLOS_1_MODULOSOPENCL_H
