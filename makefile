# Diretórios
SRC_DIR = ./src
BUILD_DIR = ./build

# Arquivo executável final
EXEC = $(BUILD_DIR)/Main

# Arquivos fontes
SRC_FILES = $(wildcard $(SRC_DIR)/**/*.cpp) ./src/Main.cpp

# Arquivos objeto correspondentes
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Arquivos de dependência
DEP_FILES = $(OBJ_FILES:.o=.d)

# Flags de compilação
CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS = -lglfw -lvulkan -lGL

run: $(EXEC)
	$(EXEC)

# Regra padrão para compilar o executável
$(EXEC): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) $(LDFLAGS) -o $(EXEC)

# Regra para compilar os arquivos .cpp para .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DIR)/%.d
	@mkdir -p $(dir $@)  # Cria o diretório para o arquivo objeto
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para gerar as dependências (.d)
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)  # Cria o diretório para o arquivo de dependência
	$(CXX) -MM $(CXXFLAGS) $< > $@

# Limpeza dos arquivos de compilação
clean:
	rm -rf $(BUILD_DIR)

# Forçar uma recompilação (usado com "make clean")
rebuild: clean $(EXEC)

# Incluir dependências geradas automaticamente
-include $(DEP_FILES)

# Impedir que make tente compilar arquivos que não são alvos
.PHONY: clean rebuild
