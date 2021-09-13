# tp-2021-2c-Olimpianos

La config tiene archivos necesarios para VSCode. Aunque se puede importar cada modulo a eclipse y laburar de ahi si es mas comodo para alguien.

# VSCode

- Installar la extension [C/C++ Intellisense](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools "C/C++ Intellisense") en **VSCode**
- Instalar ultima version de [VSCode de 32bit](https://code.visualstudio.com/updates/v1_35) 

Existen estas tareas para cada modulo: 

 - Run modulo
 - Valgrind modulo
 - Helgrind modulo
 - Test modulo

Crear un par de keybinds para las tareas mas usadas. Tambien existe la tarea **Build all** que compila todos los modulos.

Si no estas comodo con las tareas siempre podes usar la consola para compilar y correr el modulo.`make help` en los dirs para mas info.

# Consultas 

Consultar si el LD_LIBRARY_PATH de los procesos externos es fijo, o si nosotros lo ponemos segun nuestra implementacion. Y si es fija la forma en que se incluye a la libreria en los procesos externos.
