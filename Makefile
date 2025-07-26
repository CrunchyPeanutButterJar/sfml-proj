CC:=g++

INCLUDE_DIR:=include
BUILD_DIR:=build
SOURCE_DIR:=src

SOURCE_FILES:=$(wildcard $(SOURCE_DIR)/*.cpp)
OBJ_FILES:=$(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCE_FILES))

LDFLAGS:= -L/usr/local/lib
LDLIBS:= -lsfml-graphics -lsfml-window -lsfml-system -lreflectcpp

CFLAGS:= -I$(INCLUDE_DIR) -I/usr/local/include/rfl -I/usr/local/include/rfl/rfl -MMD -MP -std=c++20 -Wall

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $< $(CFLAGS) -o $@

runRelease: CFLAGS+=-O3
runRelease: sfml-app
	./sfml-app

runDebug: CFLAGS+=-g -DDEBUG_BUILD
runDebug: sfml-app
	./sfml-app

clean:
	rm -rf $(BUILD_DIR) sfml-app bindings.json

sfml-app: $(OBJ_FILES)
	$(CC) -o sfml-app $^ $(LDFLAGS) $(LDLIBS)

.PHONY: run clean

-include $(OBJ_FILES:.o=.d)