AS31 ?= as31

build/full.bin: identity48_applet.asm
	mkdir -p build
	$(AS31) -Fbin -O$@ $<

build/identity48_applet.bin: build/full.bin tools/slice.py
	python3 tools/slice.py $< $@ 0xB000 0xB19E

verify: build/identity48_applet.bin
	cmp build/identity48_applet.bin identity48_applet.bin
	python3 check.py
	sha256sum -c SHA256SUMS

clean:
	rm -rf build

.PHONY: verify clean
