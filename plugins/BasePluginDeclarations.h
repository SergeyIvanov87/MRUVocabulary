#ifndef BASE_PLUGIN_DECLARATIONS_H
#define BASE_PLUGIN_DECLARATIONS_H

#ifdef __cplusplus
    #define EXTERN_BEGIN    extern "C" {
    #define EXTERN_END      }
    #define EXTERN          extern "C"
#else
    #define EXTERN_BEGIN
    #define EXTERN_END
    #define EXTERN
#endif

#define STR(name)               #name

#endif //BASE_PLUGIN_DECLARATIONS_H
