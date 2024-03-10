help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-16s\033[0m %s\n", $$1, $$2}'

.PHONY: reformat
reformat: ## Reformat all source files
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
		| xargs clang-format -i

.PHONY: todo
todo: ## Find all TODO, FIXME, XXX comments
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
check: clang-format cppcheck ## Run all checks

.ONESHELL: check-docker
check-docker: ## Run all checks in docker
	docker build -t lilka-check -f - . <<EOF
		FROM ubuntu:24.04
		RUN apt-get update -y && \
		apt-get install -y clang-format cppcheck findutils grep make
	EOF
	docker run --rm -it -v $(PWD):/lilka -w /lilka lilka-check make check

.PHONY: clang-format
clang-format: ## Run clang-format check
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
cppcheck: ## Run cppcheck check
	cppcheck . -i.ccls-cache -ipio -imjs -idoomgeneric -ibak --enable=performance,style \
		--suppress=knownPointerToBool \
		--suppress=noCopyConstructor \
		--suppress=noOperatorEq \
		--inline-suppr \
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
