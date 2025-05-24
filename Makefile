IMAGE2CODE = ./sdk/tools/image2code/image2code.py
CPPCHECK ?= cppcheck
CLANG_FORMAT ?= $(shell command -v clang-format-17 2>/dev/null || echo clang-format)

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-16s\033[0m %s\n", $$1, $$2}'

.PHONY: todo
guidelines:
	find \
		. \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name doomgeneric -prune \) \
		-not \( -name bak -prune \) \
		-not \( -name mJS -prune \) \
		-not \( -name SimpleFTPServer -prune \) \
		-not \( -name LodePNG -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		-o -iname *.c \
		-o -iname *.hpp \
		-o -iname *.h \
		-o -iname *.rst \
		| xargs grep --color=always -n -H -E "GUIDELINE" \

todo: ## Find all TODO, FIXME, XXX comments
	find \
		. \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name doomgeneric -prune \) \
		-not \( -name bak -prune \) \
		-not \( -name mJS -prune \) \
		-not \( -name SimpleFTPServer -prune \) \
		-not \( -name LodePNG -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		-o -iname *.c \
		-o -iname *.hpp \
		-o -iname *.h \
		-o -iname *.rst \
		| xargs grep --color=always -n -H -E "TODO|FIXME|XXX" \

.PHONY: icons
icons:
	# Find all PNG images in firmware and sdk folders and convert them to .h
	# Exclude splash screens
	find \
		firmware \
		sdk \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name doomgeneric -prune \) \
		-not \( -name bak -prune \) \
		-not \( -name mJS -prune \) \
		-not \( -name SimpleFTPServer -prune \) \
		-not \( -name LodePNG -prune \) \
		-not \( -name *splash* -prune \) \
		-not \( -name *weather* -prune \) \
		-iname '*.png' \
		-exec $(IMAGE2CODE) {} \;

.PHONY: check
check: clang-format cppcheck ## Run all checks

.ONESHELL: check-docker
check-docker: ## Run all checks in docker
	docker build -t lilka-check -f - . <<EOF
		FROM ubuntu:24.04
		RUN apt-get update -y && \
		apt-get install -y clang-format-17 cppcheck findutils grep make
	EOF
	docker run --rm -it -v $(PWD):/lilka -w /lilka lilka-check make check

.PHONY: clang-format
clang-format: ## Run clang-format check
	find \
		. \
		-not \( -name .ccls-cache -prune \) \
		-not \( -name .pio -prune \) \
		-not \( -name doomgeneric -prune \) \
		-not \( -name bak -prune \) \
		-not \( -name mJS -prune \) \
		-not \( -name SimpleFTPServer -prune \) \
		-not \( -name LodePNG -prune \) \
		-iname *.h \
		-o -iname *.cpp \
		-o -iname *.c \
		-o -iname *.hpp \
		-o -iname *.h \
		| xargs $(CLANG_FORMAT) --dry-run --Werror

.PHONY: cppcheck
cppcheck: ## Run cppcheck check
	$(CPPCHECK) . -i.ccls-cache -ipio -idoomgeneric -ibak -imJS -iSimpleFTPServer -iLodePNG \
		--enable=performance,style \
		--suppress=knownPointerToBool \
		--suppress=noCopyConstructor \
		--suppress=noOperatorEq \
		--inline-suppr \
		--error-exitcode=1

.PHONY: fix
fix: ## Fix code style
