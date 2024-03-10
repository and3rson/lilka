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
	@find \
		. \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name mjs -prune \) \
		-not \( -name doomgeneric -prune \) \
		-not \( -name bak -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		-o -iname *.c \
		-o -iname *.hpp \
		-o -iname *.h \
		-o -iname *.rst \
		| xargs grep --color=always -n -H -E "TODO|FIXME|XXX" \

.PHONY: check
check: clang-format cppcheck

.PHONY: clang-format
clang-format:
	# Find all files, but exclude .pio and .ccls-cache directories
	# Preserve colors in output
	find \
		. \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name mjs -prune \) \
		-not \( -name doomgeneric -prune \) \
		-not \( -name bak -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		-o -iname *.c \
		-o -iname *.hpp \
		-o -iname *.h \
		| xargs clang-format --dry-run --Werror

.PHONY: cppcheck
cppcheck:
	cppcheck . -i.ccls-cache -ipio -imjs -idoomgeneric -ibak --enable=performance,style \
		--suppress=cstyleCast \
		--suppress=constVariablePointer \
		--suppress=constParameterPointer \
		--suppress=constVariableReference \
		--suppress=noExplicitConstructor \
		--suppress=knownPointerToBool \
		--suppress=noCopyConstructor \
		--suppress=noOperatorEq \
		--error-exitcode=1

.PHONY: fix
fix:
	# Find all files, but exclude .pio and .ccls-cache directories
	# Preserve colors in output
	find \
		. \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name mjs -prune \) \
		-not \( -name doomgeneric -prune \) \
		-not \( -name bak -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		-o -iname *.c \
		-o -iname *.hpp \
		-o -iname *.h \
		| xargs clang-format -i
