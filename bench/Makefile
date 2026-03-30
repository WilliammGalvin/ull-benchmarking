.PHONY: release debug bench-o0 clean run run-debug run-o0 run-pinned rebuild

BUILD_DIR    := build
RELEASE_DIR  := $(BUILD_DIR)/release
DEBUG_DIR    := $(BUILD_DIR)/debug
BENCH_O0_DIR := $(BUILD_DIR)/bench-o0
GENERATOR    := Ninja
TARGET       := ull-benchmarking

release:
	cmake -B $(RELEASE_DIR) -DCMAKE_BUILD_TYPE=Release -G $(GENERATOR)
	cmake --build $(RELEASE_DIR)

debug:
	cmake -B $(DEBUG_DIR) -DCMAKE_BUILD_TYPE=Debug -G $(GENERATOR)
	cmake --build $(DEBUG_DIR)

bench-o0:
	cmake -B $(BENCH_O0_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O0" -G $(GENERATOR)
	cmake --build $(BENCH_O0_DIR)

run: release
	./$(RELEASE_DIR)/$(TARGET)

run-debug: debug
	./$(DEBUG_DIR)/$(TARGET)

run-o0: bench-o0
	./$(BENCH_O0_DIR)/$(TARGET)

run-pinned: release
	sudo cpupower frequency-set -g performance
	taskset -c 0 ./$(RELEASE_DIR)/$(TARGET); \
	sudo cpupower frequency-set -g powersave

rebuild: clean release

clean:
	rm -rf $(BUILD_DIR)
