BDF = ./LilTrackerIcons.bdf
NAME = liltracker_icons
FONT = ./$(NAME).h

all: $(FONT)

$(FONT): $(BDF)
	./bdfconv -v -f 1 -b 2 $(BDF) -o $(FONT) -n $(NAME)

.PHONY: clean
clean:
	rm -f $(FONT)
