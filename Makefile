.PHONY: release debug clean run run-debug run-pinned rebuild

BUILD_DIR    := build
RELEASE_DIR  := $(BUILD_DIR)/release
DEBUG_DIR    := $(BUILD_DIR)/debug
GENERATOR    := Ninja
TARGET       := ull-benchmarking

release:
	cmake -B $(RELEASE_DIR) -DCMAKE_BUILD_TYPE=Release -G $(GENERATOR)
	cmake --build $(RELEASE_DIR)

debug:
	cmake -B $(DEBUG_DIR) -DCMAKE_BUILD_TYPE=Debug -G $(GENERATOR)
	cmake --build $(DEBUG_DIR)

run: release
	./$(RELEASE_DIR)/$(TARGET)

run-debug: debug
	./$(DEBUG_DIR)/$(TARGET)

run-pinned: release
	sudo cpupower frequency-set -g performance
	taskset -c 0 ./$(RELEASE_DIR)/$(TARGET)
	sudo cpupower frequency-set -g powersave

rebuild: clean release

clean:
	rm -rf $(BUILD_DIR)
