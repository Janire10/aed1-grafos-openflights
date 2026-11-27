CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET := vuelos
SRC := main.cpp

.PHONY: all datos ejecutar graficas todo clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

datos:
	python3 preparar_datos.py

ejecutar: $(TARGET)
	./$(TARGET)

graficas:
	python3 generar_graficas.py

todo: datos $(TARGET) ejecutar graficas

clean:
	rm -f $(TARGET) vuelos.exe resultados_*.csv grafica*.png