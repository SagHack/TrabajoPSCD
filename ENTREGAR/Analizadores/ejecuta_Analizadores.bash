#!/bin/bash

## $# --> Parametros de invocación
## -ne --> not equal
if [[ "$#" -ne 4 ]]
then
	
	echo "Error en los paremtros de invocación"
	exit
fi

## Hacer make
make -f Makefile_Analizadores

## Si no existe ( ! -e ) el ejecutalbe
if [ ! -e Analizadores ]
then
 
	echo "No existe el ejecutable de los Analizadores"
	exit
fi

./Analizadores ${1} ${2} ${3} ${4}

exit 0
