# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC
LDFLAGS = -ldl

# Исходные файлы
SRCS = main.cpp aes_menu.cpp mickey_menu.cpp magic_menu.cpp file.cpp AES-128.cpp MICKEY.cpp MAGIC.cpp
OBJS = $(SRCS:.cpp=.o)

# Имена библиотек
LIBAES = libaes.so
LIBMICKEY = libmickey.so
LIBMAGIC = libmagic.so
TARGET = work_rgr

# Правила сборки
all: $(LIBAES) $(LIBMICKEY) $(LIBMAGIC) $(TARGET)

$(LIBAES): AES-128.o file.o
	$(CXX) -shared -o $@ $^

$(LIBMICKEY): MICKEY.o file.o
	$(CXX) -shared -o $@ $^

$(LIBMAGIC): MAGIC.o file.o
	$(CXX) -shared -o $@ $^

$(TARGET): main.o aes_menu.o mickey_menu.o magic_menu.o file.o
	$(CXX) -o $@ $^ $(LDFLAGS)

# Общие правила для объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS)

.PHONY: all clean
