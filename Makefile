CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET := vuelos
SRC := src/main.cpp

.PHONY: all datos ejecutar graficas todo clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

datos:
	python3 scripts/preparar_datos.py

ejecutar: $(TARGET)
	mkdir -p resultados
	./$(TARGET)

graficas:
	python3 scripts/generar_graficas.py

todo: datos $(TARGET) ejecutar graficas

clean:
	rm -f $(TARGET) vuelos.exe resultados/*.csv resultados/*.png