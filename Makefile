# Компилятор и флаги
CC = gcc
CFLAGS = -g -lm -D _DEBUG -ggdb3 -O0 -Wall -Wextra -D_GNU_SOURCE \
  -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual \
  -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal \
  -Wformat-nonliteral -Wformat-security -Wformat-signedness \
  -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith \
  -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 \
  -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default \
  -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
  -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-protector -fcheck-new \
  -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
  -Wstack-usage=8192 -pie -fPIE -Werror=vla

CFLAGS += $(shell pkg-config --cflags cglm)
CFLAGS += $(shell pkg-config --cflags glfw3)
CFLAGS += $(shell pkj-config --cflags notcurses)

# Директории
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SAVES_DIR = saves

# Исходные файлы
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/program

# Флаги для генерации зависимостей
DEPFLAGS = -MMD -MP

LDFLAGS = -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
LDFLAGS += $(shell pkg-config --libs cglm)
LDFLAGS += $(shell pkg-config --libs glfw3)
LDFLAGS += $(shell pkg-config --libs notcurses) $(CUSTOM_WARNINGS)

# Цель по умолчанию
all: $(TARGET)
	@echo "success"

# Линковка
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	mkdir -p $(SAVES_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Компиляция
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Создание директорий
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Запуск программы
run: $(TARGET)
	@echo "success"
	./$(TARGET)


# Очистка
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(SAVES_DIR)
	@echo "Cleaned build artifacts"

-include $(OBJECTS:.o=.d)

.PHONY: all run clean
