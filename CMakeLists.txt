cmake_minimum_required(VERSION 4.2)
project(analyseur_circuit_electronique C)

set(CMAKE_C_STANDARD 11)

add_library(analyseur_circuit_electronique STATIC library.c
        circuit.h)
