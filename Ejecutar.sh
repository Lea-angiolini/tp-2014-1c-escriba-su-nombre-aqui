#!/bin/bash


ubicacion=$(pwd)
clear
echo "Usar solo en gnome (exec gnome-terminal)"
echo "Ingrese cantidad de CPU - 1 - 2 - 3 - 4 -"
read n

case $n in
2)
cd $ubicacion
cd UMV/Debug
gnome-terminal --title=UMV -e "./UMV ../resources/config.cfg"

cd $ubicacion
cd Kernel/Debug
gnome-terminal --title=Kernel -e "./Kernel ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU1 -e "./CPU ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU2 -e "./CPU ../resources/config.cfg"

;;
3)
cd $ubicacion
cd UMV/Debug
gnome-terminal --title=UMV -e "./UMV ../resources/config.cfg"

cd $ubicacion
cd Kernel/Debug
gnome-terminal --title=Kernel -e "./Kernel ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU1 -e "./CPU ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU2 -e "./CPU ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU3 -e "./CPU ../resources/config.cfg"
;;
4)
cd $ubicacion
cd UMV/Debug
gnome-terminal --title=UMV -e "./UMV ../resources/config.cfg"

cd $ubicacion
cd Kernel/Debug
gnome-terminal --title=Kernel -e "./Kernel ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU1 -e "./CPU ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU2 -e "./CPU ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU3 -e "./CPU ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU4 -e "./CPU ../resources/config.cfg"
;;
*)
cd $ubicacion
cd UMV/Debug
gnome-terminal --title=UMV -e "./UMV ../resources/config.cfg"

cd $ubicacion
cd Kernel/Debug
gnome-terminal --title=Kernel -e "./Kernel ../resources/config.cfg"

cd $ubicacion
cd CPU/Debug
gnome-terminal --title=CPU1 -e "./CPU ../resources/config.cfg"

#gnome-terminal --maximize --tab --title=UMV -e "bash -c './umv.sh / && sleep 0'" --tab --title=Kernel -e "bash -c './Kernel.sh / && sleep 0'"  --tab --title=CPU -e "bash -c './CPU.sh / && sleep 0'"
;;
esac

s=1

while [ $s != 0 ]
 	do
		echo "Ingrese script a ejecutar (no hace falta .ansisop)"
		echo " "
		cd $ubicacion
		cd Programa/resources
		ls
		read s
		cd $ubicacion
		cd Programa/Debug
		export ANSISOP_CONFIG=../resources/config.cfg

		gnome-terminal --title=$s -e "./Programa ../resources/$s.ansisop"

		clear

	done

clear
exit 0


