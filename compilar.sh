#!/bin/bash

ubicacion=$(pwd)

if [ $1 = clean ]; then

	cd commons
	rm -r Debug
	cd ../UMV
	rm -r Debug
	cd ../Kernel
	rm -r Debug
	cd ../CPU
	rm -r Debug
	cd ../Programa
	rm -r Debug
	
else

	cd commons
	mkdir Debug
	cd Debug
	mkdir parser
	mkdir collections
	cd ..
	make all

	cd $ubicacion
	cd UMV
	mkdir Debug
	make all

	cd $ubicacion
	cd Kernel
	mkdir Debug
	make all

	cd $ubicacion
	cd CPU
	mkdir Debug
	make all

	cd $ubicacion
	cd Programa
	mkdir Debug
	make all
fi
	
clear
exit 

