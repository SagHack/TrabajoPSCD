#!/bin/bash

## $# --> Parametros de invocación
## -ne --> not equal
if [[ "$#" -ne 5 ]]
then
	
	echo "Error en los parametros de invocación"
	exit
fi

## Hacer make
make -f Makefile_gestor

## Si no existe ( ! -e ) el ejecutalbe
if [ ! -e gestorDeColas ]
then
 
	echo "No existe el ejecutable del gestor"
	exit
fi

./gestorDeColas ${1} ${2} ${3} ${4} ${5}

exit 0
