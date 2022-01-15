#!/bin/bash

## $# --> Parametros de invocación
## -ne --> not equal
if [[ "$#" -ne 6 ]]
then
	
	echo "Error en los paremtros de invocación"
	exit
fi

## Hacer make
make -f Makefile_MW

## Si no existe ( ! -e ) el ejecutalbe
if [ ! -e workers]
then
 
	echo "No existe el ejecutable de los workers"
	exit
fi
if [ ! -e wmaster]
then
 
	echo "No existe el ejecutable del master"
	exit
fi


./master ${1} ${2} ${3}
./workers ${4} ${5} ${6}


exit 0
