ENV ?= v2

all:
	pio -f -c vim run -e $(ENV)

upload-usb:
	pio -f -c vim run --target upload -e $(ENV)

clean:
	pio -f -c vim run --target clean -e $(ENV)

program:
	pio -f -c vim run --target program -e $(ENV)

update:
	pio -f -c vim update

compile_commands:
	pio run -t compiledb -e $(ENV)

uploadfs:
	pio run -t uploadfs -e $(ENV)
