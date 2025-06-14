# MATE Analysis Tool

MATE Analysis Tool es una herramienta en C++ basada en ROOT que facilita el procesamiento y análisis de los datos del Muon Andes Telescope (MATE). Abarca desde la validación de archivos de texto crudos hasta la generación de histogramas y el ajuste exponencial del rate de eventos de muones.

## Características principales

* **Validación de triadas** (m101/m102/m103): detecta archivos faltantes o líneas corruptas, genera un informe y separa los datos inválidos.
* **Conversión a ROOT**: transforma los datos limpios en archivos de texto combinados y en árboles `TTree` para un análisis más rápido.
* **Concatenación de árboles**: une múltiples archivos ROOT en un solo `TChain` con opción de exportar el resultado.
* **Filtrado interactivo**: permite aplicar expresiones ROOT en memoria o creando un nuevo árbol filtrado.
* **Generación de histogramas 1D/2D**: produce reportes en PDF con estilos predefinidos.
* **Ajuste exponencial del rate**: procesa timestamps de los tres planos, calcula la distribución de tiempos entre eventos y ajusta un modelo exponencial, mostrando gráficas y parámetros.

## Requisitos

* Compilador con soporte **C++17**
* **ROOT 6** instalado (disponible como `root-config`)

## Estructura del proyecto

```
MATE-Analysis-Tool/
├── include/       # headers (.hpp)
├── src/           # implementaciones (.cpp)
└── README.md      # este archivo
```

## Compilación

1. Clona el repositorio:

   ```sh
   git clone https://github.com/tuusuario/MATE-Analysis-Tool.git
   cd MATE-Analysis-Tool
   ```
2. Ejecuta:

   ```sh
   g++ -std=c++17 -Iinclude \
     src/PathConfig.cpp src/DataValidator.cpp src/DataProcessor.cpp \
     src/DateSelector.cpp src/TreeLoader.cpp src/DataFilter.cpp \
     src/HistogramMaker.cpp src/RateFitter.cpp src/main.cpp \
     $(root-config --cflags --libs) -o MATE-Analysis-Tool
   ```

El binario `MATE-Analysis-Tool` quedará listo para su uso.

## Instrucciones de uso

Antes de iniciar, prepare dos carpetas:

* **data/**: coloque los archivos de texto crudos, nombrados como `YYYYMMDD_mate-m101.txt`, `YYYYMMDD_mate-m102.txt` y `YYYYMMDD_mate-m103.txt`.
* **badData/**: aquí se moverán las triadas inválidas (faltantes o con errores de formato).

Para ejecutar la herramienta, especifique también el archivo de reporte:

```sh
./MATE-Analysis-Tool data/ badData/ reporte.txt
```

En este flujo:

1. **Validación**: se comprueba cada fecha en `data/` y se registra en `reporte.txt` cualquier triada incompleta o línea corrupta; las triadas erróneas se mueven a `badData/`.
2. **Procesamiento**: las triadas válidas generan un archivo `YYYYMMDD_combined_output.txt` y un árbol ROOT `YYYYMMDD_output.root` en el directorio actual.
3. **Menú interactivo**: ofrece opciones para:

   * Concatenar árboles ROOT y guardar el resultado.
   * Aplicar filtros dinámicos.
   * Generar histogramas 1D/2D en PDF.
   * Ajustar la distribución de tiempos entre eventos con modelo exponencial.

## Módulos internos

* **PathConfig**: configura rutas de datos y reporte.
* **DataValidator**: inspecciona y reporta triadas de texto.
* **DateSelector**: maneja la selección de fechas.
* **DataProcessor**: convierte triadas en combinados y árboles ROOT.
* **TreeLoader**: concatena y explora árboles ROOT.
* **DataFilter**: construye filtros con expresiones ROOT.
* **HistogramMaker**: crea histogramas 1D/2D en PDF.
* **RateFitter**: implementa la limpieza de timestamps y el ajuste exponencial del rate.

## Flujo de trabajo resumido

1. Validación y separación de datos malos.
2. Procesamiento de triadas válidas.
3. Carga/concatenación de ROOT.
4. Filtrado interactivo.
5. Generación de histogramas.
6. Ajuste exponencial del rate.


*MATE Analysis Tool* proporciona una solución integral para el análisis de datos de muografía con ROOT y C++.
