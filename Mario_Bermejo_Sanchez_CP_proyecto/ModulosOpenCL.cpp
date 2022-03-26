//
// Created by mariobs on 26/4/21.
//

#include "ModulosOpenCL.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>
#include <CL/cl.h>


cl_int ObtenerPlataformas(cl_platform_id *&platforms, cl_uint &num_platforms){
    cl_int error;
    int i;
    char Info[500];
    error = clGetPlatformIDs(0, NULL, &num_platforms);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    platforms = new cl_platform_id[num_platforms];
    error = clGetPlatformIDs(num_platforms, platforms, &num_platforms);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }


    for(i=0; i<num_platforms; i++){
        error = clGetPlatformInfo (platforms[i], CL_PLATFORM_NAME, 500, Info, NULL);
        if (error!=CL_SUCCESS){
            CodigoError(error);
            return error;
        }
        printf("%s\n", Info);
    }
    return CL_SUCCESS;
}

cl_int ObtenerDispositivos(cl_platform_id platform, cl_device_type device_type, cl_device_id *&devices, cl_uint &num_devices){

    cl_int error;
    int i;
    char buffer[500];

    error = clGetDeviceIDs(platform,device_type, 0,NULL, &num_devices);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    devices = new cl_device_id[num_devices];

    error = clGetDeviceIDs (platform,device_type, num_devices,devices, NULL);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    for(i=0;i<num_devices;i++){
        error= clGetDeviceInfo (devices[i], CL_DEVICE_NAME, 500, buffer,NULL);
        if (error!=CL_SUCCESS){
            CodigoError(error);
            return error;
        }
        printf("\t%s\n",buffer);
    }

    return CL_SUCCESS;
}

cl_int CrearContexto(cl_platform_id platform, cl_device_id *devices,cl_uint num_devices, cl_context &contexto){
    cl_int error;
    cl_context_properties prop[3]={CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0};

    contexto=clCreateContext (prop,num_devices, devices, NULL, NULL, &error);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    return CL_SUCCESS;
}

cl_int CrearPrograma(cl_program &program, cl_context context,cl_uint num_devices, const cl_device_id *device_list, const char *options, const char *fichero){
    cl_int error;
    char CompLog[20000], *fuente;
    leerFuentes(fuente, fichero);

    program=clCreateProgramWithSource(context, 1, (const char **)&fuente, NULL, &error);
    if (error!=CL_SUCCESS){
        CodigoError(error);
        return error;
    }
    error=clBuildProgram(program, num_devices,device_list, options, NULL,NULL);
    if (error!=CL_SUCCESS){
        if(error = CL_BUILD_PROGRAM_FAILURE){
            error=clGetProgramBuildInfo(program, device_list[0], CL_PROGRAM_BUILD_LOG, 20000, CompLog, NULL);
            if (error!=CL_SUCCESS){
                CodigoError(error);
                return error;
            }
            printf("Errores de compilacion: \n%s\n", CompLog);
        }else {
            CodigoError(error);
            return error;
        }
    }

    return CL_SUCCESS;
}

char leerFuentes(char *&src, const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf("Error al abrir el fichero '%s'\n", fileName);
        return 0;
    }

    if (fseek(file, 0, SEEK_END))  {
        printf("Error de posicionamiento en el '%s'\n", fileName);
        fclose(file);
        return 0;
    }

    long size = ftell(file);
    if (size == 0)  {
        printf("Error al calcular el tamaño del fichero '%s'\n", fileName);
        fclose(file);
        return 0;
    }

    rewind(file);
    src = (char *)malloc(sizeof(char) * size + 1);
    size_t res = fread(src, 1, sizeof(char) * size, file);
    if (res != sizeof(char) * size)   {
        printf("Error de lectura del fichero '%s'\n", fileName);
        fclose(file);
        free(src);
        return 0;
    }
    src[size] = '\0';
    fclose(file);
    return 1;
}

void CodigoError(cl_int err) {
    switch (err) {
        case CL_SUCCESS: printf("Ejecución correcta\n"); break;
        case CL_BUILD_PROGRAM_FAILURE: printf("CL_BUILD_PROGRAM_FAILURE\n"); break;
        case CL_COMPILER_NOT_AVAILABLE: printf("CL_COMPILER_NOT_AVAILABLE\n"); break;
        case CL_DEVICE_NOT_AVAILABLE: printf("CL_DEVICE_NOT_AVAILABLE \n"); break;
        case CL_DEVICE_NOT_FOUND: printf("CL_DEVICE_NOT_FOUND\n"); break;
        case CL_INVALID_ARG_INDEX : printf("CL_INVALID_ARG_INDEX \n"); break;
        case CL_INVALID_ARG_SIZE : printf("CL_INVALID_ARG_SIZE \n"); break;
        case CL_INVALID_ARG_VALUE: printf("CL_INVALID_ARG_VALUE\n"); break;
        case CL_INVALID_BUFFER_SIZE : printf("CL_INVALID_BUFFER_SIZE \n"); break;
        case CL_INVALID_BUILD_OPTIONS: printf("CL_INVALID_BUILD_OPTIONS\n"); break;
        case CL_INVALID_COMMAND_QUEUE : printf("CL_INVALID_COMMAND_QUEUE \n"); break;
        case CL_INVALID_CONTEXT: printf("CL_INVALID_CONTEXT\n"); break;
        case CL_INVALID_DEVICE_TYPE: printf("CL_INVALID_DEVICE_TYPE\n"); break;
        case CL_INVALID_EVENT: printf("CL_INVALID_EVENT\n"); break;
        case CL_INVALID_EVENT_WAIT_LIST : printf("CL_INVALID_EVENT_WAIT_LIST \n"); break;
        case CL_INVALID_GLOBAL_WORK_SIZE : printf("CL_INVALID_GLOBAL_WORK_SIZE \n"); break;
        case CL_INVALID_HOST_PTR : printf("CL_INVALID_HOST_PTR \n"); break;
        case CL_INVALID_KERNEL: printf("CL_INVALID_KERNEL \n"); break;
        case CL_INVALID_KERNEL_ARGS : printf("CL_INVALID_KERNEL_ARGS \n"); break;
        case CL_INVALID_KERNEL_NAME: printf("CL_INVALID_KERNEL_NAME\n"); break;
        case CL_INVALID_MEM_OBJECT : printf("CL_INVALID_MEM_OBJECT \n"); break;
        case CL_INVALID_OPERATION : printf("\n"); break;
        case CL_INVALID_PLATFORM: printf("CL_INVALID_PLATFORM\n"); break;
        case CL_INVALID_PROGRAM : printf("CL_INVALID_PROGRAM \n"); break;
        case CL_INVALID_PROGRAM_EXECUTABLE : printf("CL_INVALID_PROGRAM_EXECUTABLE\n"); break;
        case CL_INVALID_QUEUE_PROPERTIES: printf("CL_INVALID_QUEUE_PROPERTIES\n"); break;
        case CL_INVALID_VALUE: printf("CL_INVALID_VALUE\n"); break;
        case CL_INVALID_WORK_DIMENSION : printf("CL_INVALID_WORK_DIMENSION \n"); break;
        case CL_INVALID_WORK_GROUP_SIZE : printf("CL_INVALID_WORK_GROUP_SIZE \n"); break;
        case CL_INVALID_WORK_ITEM_SIZE : printf("CL_INVALID_WORK_ITEM_SIZE \n"); break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE : printf("CL_MEM_OBJECT_ALLOCATION_FAILURE \n"); break;
        case CL_OUT_OF_HOST_MEMORY: printf("CL_OUT_OF_HOST_MEMORY\n"); break;
        case CL_OUT_OF_RESOURCES: printf("CL_OUT_OF_RESOURCES\n"); break;
        case CL_PROFILING_INFO_NOT_AVAILABLE : printf("CL_PROFILING_INFO_NOT_AVAILABLE \n"); break;
        default: printf("Código de error no contemplado\n"); break;
    }
}

