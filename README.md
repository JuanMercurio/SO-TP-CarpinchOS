<h1 align="center" > CarpinchOS </h1>
<p align="center"><img src="https://user-images.githubusercontent.com/48862380/178836671-6aa5e5ee-1fa8-44a5-9800-3b3b50b1213f.png"> </p>

<p align="center"> Trabajo Practico - Sistemas Operativos - UTN FRBA 2021 2C  <p>

## Objetivos del Trabajo Práctico

El objetivo del trabajo práctico consiste en desarrollar una solución que permita la simulación de un sistema distribuido, donde los grupos tendrán que planificar procesos externos no implementados por los grupos, que ejecuten peticiones al sistema de recursos. El sistema deberá, mediante esta interacción, habilitar recursos de memoria (bajo un esquema de paginación pura), recursos de entrada-salida y semáforos.

### Mediante la realización de este trabajo se espera que el alumno:

- Adquiera conceptos prácticos del uso de las distintas herramientas de programación e interfaces (APIs) que brindan los sistemas operativos.
- Entienda aspectos del diseño de un sistema operativo.
- Afirme diversos conceptos teóricos de la materia mediante la implementación práctica de algunos de ellos.
- Se familiarice con técnicas de programación de sistemas, como el empleo de makefiles, archivos de configuración y archivos de log.
- Conozca con grado de detalle la operatoria de Linux mediante la utilización de un lenguaje de programación de relativamente bajo nivel como C.

El enunciado completo del trabajo practico se puede encontrar [aca](https://docs.google.com/document/d/1BDpr5lfzOAqmOOgcAVg6rUqvMPUfCpMSz1u1J_Vjtac/edit# "aca")

## Ejecucion

### Requisitos Previos

Se recomienda compilar y ejecutar en las [maquinas virtuales](https://www.utnso.com.ar/recursos/maquinas-virtuales/) de la catedra 

>  **Memoria** y **Swap** estan optimizados para que puedan correr/compilar fuera de las maquinas virtuales de la catedra. **Kernel** puede tirar errores fuera de este entorno

Se debe clonar el repositorio en todas las maquinas que vayan a ejecutar un modulo del sistema.  

Luego de clonar el repo ejecutamos _deploy.sh_

	chmod +x deploy.sh
	./deploy.sh

### Orden De Ejecucion
1. **Memoria**
2. **Swap**

**Kernel** puede ser ejecutado en cualquier momento.

### Ejecucion de Carpinchos

Una vez que el sistema este funcionando, podemos ejecutar los "procesos" que utilizan la **matelib**, alias los carpinchos.

Ejemplos de estos "procesos" en  [pruebas](https://github.com/JuanMercurio/utn-so-tp/tree/main/pruebas "pruebas")
