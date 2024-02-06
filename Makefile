.PHONY: reformat
reformat:
	find \
		./firmware-demo/src/ \
		./lib/lilka \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		| xargs clang-format -i
