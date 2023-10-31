# Output
OUT_FILE = Lab4

# Source Files
SRC = Lab4.cu

# Compile and link
all:
	nvcc $(SRC) -o $(OUT_FILE)

# Clean
clean:
	rm -f $(OUT_FILE)

zip:
	zip -r $(OUT_FILE).zip $(SRC) Makefile
