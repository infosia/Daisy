
#ifndef DAISY_EXPORT_H
#define DAISY_EXPORT_H

#ifdef DAISY_STATIC_DEFINE
#  define DAISY_EXPORT
#  define DAISY_NO_EXPORT
#else
#  ifndef DAISY_EXPORT
#    ifdef Daisy_EXPORTS
        /* We are building this library */
#      define DAISY_EXPORT 
#    else
        /* We are using this library */
#      define DAISY_EXPORT 
#    endif
#  endif

#  ifndef DAISY_NO_EXPORT
#    define DAISY_NO_EXPORT 
#  endif
#endif

#ifndef DAISY_DEPRECATED
#  define DAISY_DEPRECATED 
#endif

#ifndef DAISY_DEPRECATED_EXPORT
#  define DAISY_DEPRECATED_EXPORT DAISY_EXPORT DAISY_DEPRECATED
#endif

#ifndef DAISY_DEPRECATED_NO_EXPORT
#  define DAISY_DEPRECATED_NO_EXPORT DAISY_NO_EXPORT DAISY_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define DAISY_NO_DEPRECATED
#endif

#endif
