cmake_minimum_required(VERSION 3.24)
project(ProjetA22 C)

set(CMAKE_C_STANDARD 23)

add_executable(ProjetA22 main.c Moduleur.h Moduleur.c Colors.h Vehicule.h Peage_utils.h Vehicule.c Guichet.h Guichet.c Peage_utils.c)
