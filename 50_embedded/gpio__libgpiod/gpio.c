/*
   using the new descriptor based libgpiod-dev  (debian)

reference: Bartosz Golaszewski
	https://ostconf.com/system/attachments/files/000/001/532/original/Linux_Piter_2018_-_New_GPIO_interface_for_linux_userspace.pdf
*/

#include <stdlib.h>
#include <stdio.h>
#include <gpiod.h>

struct gpiod_chip *chip;
struct gpiod_line *line;
int rv, value;

int main()
{
	chip = gpiod_chip_open("/dev/gpiochip0");
	if (!chip)
		return -1;
	line = gpiod_chip_get_line(chip, 3);
	if (!line) {
		gpiod_chip_close(chip);
		return -1
	}
	rv = gpiod_line_request_input(line, “foobar”);
	if (rv) {
		gpiod_chip_close(chip);
		return -1;
	}
	value = gpiod_line_get_value(line);
	gpiod_chip_close(chip);

	exit(EXIT_SUCCESS);
}
