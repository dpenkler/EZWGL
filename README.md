
========================================================================

This repo contains the source code of EZWGL, the EZ widget
and graphics library.

This version is 1.51 

To compile a shared library, type (you'll have to install the
library as root !)                           
```
   ./configure
   make shared  
   make install-shared  # as root
```

To compile a static library, type
```
   make static   # static libs
   make install-static
```

If it fails, you'll probabily need to edit the Makefile. 
(tested under SunOS4.1.4, SunOS 5.* and Linux 2.*.**, AIX 4.3)


Comments, bug reports, demo programs and contributions are welcome,
please direct them to

```
	mzou@math.utexas.edu
    	dpenkler@gmail.com
```	

Enjoy.

Here is a screenshot of an interactive graph with sliders to set the Y-intercepts and a dial to set the slope of the line.

![msim](https://github.com/dpenkler/EZWGL/tree/master/doc/html/screenshots/msim.png "EZWGL screenshot 1")


========================================================================

 




