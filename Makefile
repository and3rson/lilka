.PHONY: reformat
reformat:
	find \
		./firmware-demo/src/ \
		./lib/lilka \
		-not \( -name .ccls-cache -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		| xargs clang-format -i
