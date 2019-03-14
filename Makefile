all:
	gcc pxi-encode.c -O3 -s -o pxi-encode

.PHONY: clean
clean:
	rm pxi-encode
