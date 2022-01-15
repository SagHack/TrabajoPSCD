#!/bin/bash

## $# --> Parametros de invocación
## -ne --> not equal
if [[ "$#" -ne 7 ]]
then
	
	echo "Error en los paremtros de invocación"
	exit
fi

## Hacer make
make -f Makefile_MW

## Si no existe ( ! -e ) el ejecutalbe
if [ ! -e workers ]
then
 
	echo "No existe el ejecutable de los workers"
	exit
fi
if [ ! -e master ]
then
 
	echo "No existe el ejecutable del master"
	exit
fi


./master ${1} ${2} ${3} ${4}
./workers ${5} ${6} ${7}


exit 0
