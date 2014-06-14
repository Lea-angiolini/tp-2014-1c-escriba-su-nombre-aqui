#!/bin/bash

ubicacion=$(pwd)
cd commons
make clear

cd $ubicacion
cd UMV
make clear

cd $ubicacion
cd Kernel
make clear

cd $ubicacion
cd CPU
make clear

cd $ubicacion
cd Programa
make clear

clear

exit 0