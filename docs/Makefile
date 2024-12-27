KICAD_CLI = kicad-cli
BUILDER = dirhtml

.PHONY: all
all: clean pdf doxygen
	sphinx-build -M $(BUILDER) . _build

.PHONY: pdf
pdf:
	$(KICAD_CLI) sch export pdf ../hardware/v2/main.kicad_sch -t Dark -o _gen/v2_schematic.pdf
	$(KICAD_CLI) pcb export pdf ../hardware/v2/main.kicad_pcb -l "F.Cu,B.Cu,F.Mask,B.Mask,F.SilkS,B.SilkS,Edge.Cuts" -t Dark --ibt -o _gen/v2_pcb.pdf

.PHONY: doxygen
doxygen:
	cd _doxygen && doxygen Doxyfile

.PHONY: autobuild
autobuild: clean doxygen
	@# Live-reload the documentation
	sphinx-autobuild -b $(BUILDER) . _build/html --ignore '**/_doxygen/**'

.PHONY: clean
clean:
	rm -rf _build _doxygen/xml
