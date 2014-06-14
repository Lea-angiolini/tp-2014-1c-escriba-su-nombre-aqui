#!/bin/bash

ubicacion=$(pwd)
cd commons
make all

cd $ubicacion
cd UMV
make all

cd $ubicacion
cd Kernel
make all

cd $ubicacion
cd CPU
make all

cd $ubicacion
cd Programa
make all

clear

exit 0