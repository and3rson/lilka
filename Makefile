# .PHONY: reformat
# reformat:
# 	find \
# 		./firmware-demo/src/ \
# 		./lib/lilka \
# 		-not \( -name .ccls-cache -prune \) \
# 		-not \( -name .pio -prune \) \
# 		-iname *.h \
# 		-o -iname *.cpp \
# 		| xargs clang-format -i

all:
	false

.PHONY: todo
todo:
	@# Find all files, but exclude .pio and .ccls-cache directories
	@# Preserve colors in output
	@# Print file and line on separate lines
	@find \
		. \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name mjs -prune \) \
		-not \( -name doomgeneric -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		-o -iname *.c \
		-o -iname *.hpp \
		-o -iname *.h \
		-o -iname *.rst \
		| xargs grep --color=always -n -H -E "TODO|FIXME|XXX" \
