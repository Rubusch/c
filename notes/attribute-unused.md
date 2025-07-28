# attribute unused declaration (gcc)

Allows to declare e.g. a debugging function unused to turn off warnings about its
usage.

## EXAMPLE

in adxl345.h during development, place a general available debug function
```
__attribute__((unused))
static void adxl345_reg_debug(const struct device *dev)
{
	...
}
```

Building the source won't show warnings about adxl345_reg_debug() not being
used, if noone calls it.
