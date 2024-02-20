all:
	sphinx-build -M html . _build

autobuild:
	sphinx-autobuild . _build/html

gen:
	kicad-cli sch export pdf ../hardware/v2/main.kicad_sch -t Dark -o _gen/v2_schematic.pdf
	kicad-cli pcb export pdf ../hardware/v2/main.kicad_pcb -l "F.Cu,B.Cu,F.Mask,B.Mask,F.SilkS,B.SilkS,Edge.Cuts" -t Dark --ibt -o _gen/v2_pcb.pdf
