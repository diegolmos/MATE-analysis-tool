# MATE Analysis Tool

MATE Analysis Tool es una pequeña herramienta en C++ basada en ROOT que facilita el procesamiento y análisis de datos del Muon Andes Telescope (MATE).

## Características principales

- **Validación de fechas** (m101/m102/m103): Detecta archivos faltantes, líneas corruptas y otros posibles errores para cada archivo con datos del detector, genera un informe y separa los datos inválidos en un directorio definido por el usuario.
- **Conversión a ROOT**: Luego de validar los archivos, el programa da la opción de seleccionar una lista o intervalo de fechas para procesar los datos limpios y crear un `TTree` para realizar el análisis con ROOT.
El programa además da la opción de transformar los datos limpios en archivos CSV los cuales contienen la información de los 6 planos del detector, de esta manera se facilita un posterior análisis de los datos con herramientas externas.
- **Concatenación de árboles**:  El programa permite unir una lista o intervalo de archivos ROOT usando la clase `TChain` con opción de exportar el archivo .root resultante.
- **Filtrado interactivo**: permite aplicar expresiones ROOT para cortes en los datos de todo tipo, ya sea en memoria o creando un nuevo árbol filtrado.
- **Generación de histogramas 1D/2D**: produce reportes en PDF con estilos predefinidos.
  [hist2d_planes.pdf](https://github.com/user-attachments/files/20751261/hist2d_planes.pdf)
  [hist1d.pdf](https://github.com/user-attachments/files/20751257/hist1d.pdf)

- **Ajuste exponencial del rate**: procesa timestamps de los tres planos, calcula la distribución de tiempos entre eventos y ajusta un modelo exponencial, mostrando gráficas y parámetros.
  [rate_fit.pdf](https://github.com/user-attachments/files/20751264/rate_fit.pdf)


## Requisitos

- Compilador con soporte **C++17**
- **ROOT 6** instalado (disponible como `root-config`)

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
   git clone https://github.com/diegolmos/MATE-Analysis-Tool.git
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

- **data/**: coloque los archivos de texto con datos del detector, nombrados como `YYYY_MM_DD_06h00_mate-m101.txt`, `YYYY_MM_DD_06h00_mate-m102.txt` y `YYYY_MM_DD_06h00_mate-m103.txt`.
- **badData/**: aquí se moverán las triadas m101/m102/m103 inválidas (faltantes o con errores de formato y escritura ).

Para ejecutar la herramienta, se pedirá especificar también el nombre archivo de reporte, este archivo de texto contiene la información de los datos inválidos, indicando la línea, nombre del archivo y el tipo de problema que presenta.


En este flujo:

1. **Validación**: se comprueba cada fecha en `data/` y se registra en `reporte.txt` cualquier triada incompleta o línea corrupta; las triadas erróneas se mueven a `badData/`.
2. **Procesamiento**: las triadas válidas generan un archivo `YYYYMMDD_combined_output.txt` y un árbol ROOT `YYYYMMDD_output.root` en el directorio actual.
3. **Menú interactivo**: ofrece opciones para:
   - Concatenar árboles ROOT y guardar el resultado.
   - Aplicar filtros dinámicos.
   - Generar histogramas 1D/2D en PDF.
   - Ajustar la distribución de tiempos entre eventos con modelo exponencial.

## Módulos internos

- **PathConfig**: configura rutas de datos y reporte.
- **DataValidator**: inspecciona y reporta triadas de texto.
- **DateSelector**: maneja la selección de fechas.
- **DataProcessor**: convierte triadas en combinados y árboles ROOT.
- **TreeLoader**: concatena y explora árboles ROOT.
- **DataFilter**: construye filtros con expresiones ROOT.
- **HistogramMaker**: crea histogramas 1D/2D en PDF.
- **RateFitter**: implementa la limpieza de timestamps y el ajuste exponencial del rate.

## Flujo de trabajo resumido

1. Validación y separación de datos malos.
2. Procesamiento de triadas válidas.
3. Carga/concatenación de ROOT.
4. Filtrado interactivo.
5. Generación de histogramas.
6. Ajuste exponencial del rate.

---


