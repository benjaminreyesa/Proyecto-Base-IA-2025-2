# Makefile — Proyecto-Base-IA-2025-2
# Construye el binario 'painter' desde app.cpp, greedy_solver.cpp y stroke.cpp

# ---- Configuración ----
CXX       ?= g++
CXXFLAGS  ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -MMD -MP
LDFLAGS   ?=
SRC        = app.cpp greedy_solver.cpp stroke.cpp
OBJ        = $(SRC:%.cpp=build/%.o)
DEP        = $(OBJ:.o=.d)
BIN        = painter

# ---- Targets principales ----
.PHONY: all clean debug release run dirs

all: $(BIN)

# Build optimizado con NDEBUG
release: CXXFLAGS := -std=c++17 -O3 -DNDEBUG -Wall -Wextra -Wpedantic -MMD -MP
release: clean $(BIN)

# Build de depuración
debug: CXXFLAGS := -std=c++17 -O0 -g -Wall -Wextra -Wpedantic -MMD -MP
debug: clean $(BIN)

# ---- Enlazado ----
$(BIN): dirs $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# ---- Compilación a objetos (con dependencias .d) ----
build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Carpeta para objetos
dirs:
	@mkdir -p build

# ---- Limpieza ----
clean:
	rm -rf build $(BIN) $(BIN).exe

# Incluir dependencias generadas por -MMD
-include $(DEP)
