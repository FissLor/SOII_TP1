# Trabajo Practico 1 SOII-FCEFyN-UNC

Implementacion Cliente-Servidor para descarga de imagenes booteables directo a un pen-drive

## Getting Started

Clonar el proyecto

`$ git clone https://github.com/FissLor/SOII_TP1.git`

En el directorio del proyecto

```
$ cd SOII_TP1
$ cmake .
$ make all
```

Antes que nada se debera inicializar la base de datos del servidor, para ello ejecutar

```
$ ./Server/bin/database_init
```

Tambien como parte del proceso de inicializacion del servidor se deben cargar manualmente las imagenes que se deseen 
hacer disponibles para descarga en `./Server/iso`

Luego para ejecutar el servidor

```
$ ./Server/bin/Server
```

Para ejecutar el cliente se debe tener permisos de super user para poder grabar el contenido de las
 imagenes en el pendrive seleccionado

```
$ sudo ./Client/bin/Client
```
### Prerequisitos

Para una correcta visualizacion de la documentacion se recomienda instalar graphviz 

```
$ apt install graphviz
$ pacman -S graphviz
```

## Built With

* [CMake](https://cmake.org/) - Build System

## Autor

* **Lorenzo Fissore**  - [FissLor](https://github.com/FissLor)

## Licencia

Este proyecto esta licenciado bajo  GNU General Public License- ver [LICENSE](LICENSE) para mas detalles.



