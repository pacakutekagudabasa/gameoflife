# Game of Life Enhanced - Makefile
# गेम ऑफ लाइफ एन्हांस्ड - बिल्ड सिस्टम
# 
# यह Makefile enhanced Game of Life को compile और manage करने के लिए
# बनाई गई है। इसमें सभी dependencies, build targets, और utility commands
# शामिल हैं।

# Compiler और compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2  # Warning flags और optimization
LIBS = -lSDL2 -lm                    # SDL2 और math libraries

# Source files और object files
SRCS = main.c board.c state.c rules.c  # सभी source files
OBJS = $(SRCS:.c=.o)                   # Corresponding object files
TARGET = gameoflife                     # Final executable का नाम

# Default target - सबसे पहले यह run होता है
all: $(TARGET)

# Main executable build करने के लिए target
# सभी object files को link करके final executable बनाता है
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# Object files build करने के लिए generic rule
# हर .c file को corresponding .o file में compile करता है
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build files को clean करने के लिए target
# सभी generated files (object files और executable) को delete करता है
clean:
	rm -f $(OBJS) $(TARGET)

# SDL2 dependencies install करने के लिए target (Ubuntu/Debian)
# Development libraries install करता है जो compilation के लिए जरूरी हैं
install-deps:
	sudo apt-get update
	sudo apt-get install libsdl2-dev

# Game को build करके run करने के लिए target
# पहले build करता है फिर execute करता है
run: $(TARGET)
	./$(TARGET)

# Sample pattern file के साथ game run करने के लिए target
# पहले sample file create करता है फिर उसके साथ game run करता है
run-sample: $(TARGET)
	./$(TARGET) sample.txt

# Sample pattern file create करने के लिए target
# एक glider pattern के साथ example file बनाता है
sample:
	@echo "Creating sample.txt with a glider pattern..."
	@echo "Sample glider pattern बनाया जा रहा है..."
	@echo "0000000000000000" > sample.txt
	@echo "0000000000000000" >> sample.txt
	@echo "0000000000000000" >> sample.txt
	@echo "0000000000000000" >> sample.txt
	@echo "0000100000000000" >> sample.txt
	@echo "0000010000000000" >> sample.txt
	@echo "0001110000000000" >> sample.txt
	@echo "0000000000000000" >> sample.txt
	@echo "0000000000000000" >> sample.txt
	@echo "0000000000000000" >> sample.txt
	@echo "Sample glider pattern created in sample.txt"
	@echo "Sample glider pattern sample.txt में बनाया गया"

# Help information display करने के लिए target
# सभी available targets और controls की जानकारी provide करता है
help:
	@echo "Game of Life Enhanced - Build System"
	@echo "गेम ऑफ लाइफ एन्हांस्ड - बिल्ड सिस्टम"
	@echo ""
	@echo "Targets / टारगेट्स:"
	@echo "  all          - Build the game (default) / गेम build करें"
	@echo "  clean        - Remove build files / build files हटाएं"
	@echo "  install-deps - Install SDL2 development libraries / SDL2 dev libraries install करें"
	@echo "  run          - Build and run the game / गेम build करके run करें"
	@echo "  run-sample   - Build and run with sample pattern / sample pattern के साथ run करें"
	@echo "  sample       - Create a sample pattern file / sample pattern file बनाएं"
	@echo "  help         - Show this help / यह help दिखाएं"
	@echo ""
	@echo "Controls in game / गेम में controls:"
	@echo "  SPACE       - Pause/Resume / पॉज़/रिज्यूम"
	@echo "  R           - Reload from file / फाइल से reload करें"
	@echo "  C           - Clear board / बोर्ड clear करें"
	@echo "  G           - Generate random / रैंडम generate करें"
	@echo "  T           - Switch rule set / rule set बदलें"
	@echo "  H           - Help in game / गेम में help"
	@echo "  ESC/Q       - Quit / बाहर निकलें"
	@echo "  Mouse Click - Toggle cell (when paused) / cell toggle करें (pause में)"
	@echo "  Mouse Drag  - Paint alive cells (when paused) / जीवित cells paint करें"
	@echo "  Ctrl+Drag   - Paint dead cells (when paused) / मृत cells paint करें"

# Phony targets - ये actual files नहीं हैं बल्कि commands हैं
# Make को बताता है कि ये targets file names नहीं हैं
.PHONY: all clean install-deps run run-sample sample help
