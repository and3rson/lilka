# Fetch icons from https://openweathermap.org/weather-conditions#Icon-list
ICON_IDS := 01 02 03 04 09 10 11 13 50
ICON_NAMES := clear few-clouds scattered-clouds broken-clouds shower-rain rain thunderstorm snow mist
ICON_TYPES := d n
PNG_ICONS = $(foreach name,$(ICON_NAMES),$(foreach type,$(ICON_TYPES),$(name)-$(type).png))
H_ICONS = $(patsubst %.png,%.h,$(PNG_ICONS))
OUT = weather_icons.h
SIZE = 4x
IMG2CODE = ../../../../../../sdk/tools/image2code/image2code.py

# Function to get ID from name
define get_id_from_name
$(shell python -c "print('$(ICON_IDS)'.split(' ')['$(ICON_NAMES)'.split(' ').index('$(1)')])")
endef

.PHONY: all
all: $(OUT)

$(OUT): $(H_ICONS)
	echo "Generating header file"
	echo "#pragma once" > $@
	echo "" >> $@
	for file in $(H_ICONS); do \
		echo "#include \"$$file\"" >> $@; \
	done

# Mark all PNG files as precious to prevent deletion
.PRECIOUS: $(PNG_ICONS)

%.h: %.png
	echo "Converting icon to code"
	$(IMG2CODE) $< --rle

%.png:
	echo "Downloading icon $@"
	$(eval basename := $(basename $(@)))
	$(eval name := $(subst -$(lastword $(subst -, ,$(basename))),,$(basename)))
	$(eval type := $(lastword $(subst -, ,$(basename))))
	$(eval code := $(call get_id_from_name,$(name)))
	echo "$(basename) $(name) $(type) $(code)"
	wget -q https://openweathermap.org/img/wn/$(code)$(type)@${SIZE}.png -O $@
	convert $@ -background black -alpha remove -alpha off PNG32:$@

.PHONY: clean
clean:
	rm -f $(PNG_ICONS) $(H_ICONS) $(OUT)
