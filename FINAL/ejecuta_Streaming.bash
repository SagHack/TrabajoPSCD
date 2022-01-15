#!/bin/bash

## $# --> Parametros de invocación
## -ne --> not equal
if [[ "$#" -ne 1 ]]
then
	
	echo "Error en los paremtros de invocación"
	exit
fi

## Hacer make
make -f Makefile_Streaming

## Si no existe ( ! -e ) el ejecutalbe
if [ ! -e ServicioStreaming ]
then
 
	echo "No existe el ejecutable del Streaming"
	exit
fi


./ServicioStreaming ${1}


exit 0
