/**************************************************************/
#ifndef _FEATURE_H
#define _FEATURE_H
/**************************************************************/

/* enable (minimal) debugging */
/* #define EZ_DEBUG */


/* do malloc/free debuging */
/* #define EZ_MEM_DEBUG */


/* A few high frequency gl routines have macro substitutes. */
#define USE_GL_MACROS


/* use X's shm extension if available */
#define  USE_XSHM


/* turn on jpeg image support if available */
#define  JPEG_SUPPORT 


/* turn on png image support if available */
#define  PNG_SUPPORT 


/* turn on tiff image support if available */
#define  TIFF_SUPPORT 


/**************************************************************/
#endif

