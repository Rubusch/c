# Lothar's DevKit Functions

What you can see here is a wild collection of wrapper functions to POSIX and Linux libraries. I use it basically for quicker setup of demos and prototyping. When building a demo in most cases memory and resource management can be left over to the OS, respectively to calls like ``exit(EXIT_FAILURE)`` or ``exit(EXIT_SUCCESS`` which will perform a overall cleanup. Thus, executing a unix function, I might check its return value for errors, log something down and abort the demo right away, which is what the wrappers do in most cases. There is no sophisticated resource cleanup, or event handling as it would be the case in an application. This is not needed for most of the small demos. Hence the functions are not intended to build up more complex application code!  

All GPL  

TODO: tests and a bit more library-fication  
