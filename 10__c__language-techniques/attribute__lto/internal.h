#ifndef INTERNAL_H
#define INTERNAL_H

// NB: the symbol used for the version can be chosen freely as
// e.g. 'DEMO', or 'main.c', or...

//*
#if __GNUC__ >= 10
#define IMPL_SYMVER(name, version)                                             \
	__attribute__((__symver__("public_" #name "@@DEMO_" version)))

#else
// due to not available feature, use __asm__
#define IMPL_SYMVER(name, version)                                             \
	__asm__(".symver new_" #name ",public_" #name "@@DEMO_" version);

#endif

#endif
