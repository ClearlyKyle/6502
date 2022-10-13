#ifndef __MACROS_H__
#define __MACROS_H__

// https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
// https://stackoverflow.com/questions/1623111/programmatic-way-to-get-variable-name-in-c

#ifdef DEBUG
#define DEBUG_PRINT 1
#else
#define DEBUG_PRINT 0
#endif

#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define WHITE "\e[1m"
#define COLOR_X "\e[m"

#define print_db(fmt, ...)                       \
    do                                           \
    {                                            \
        if (DEBUG_PRINT)                         \
            fprintf(stderr, fmt, ##__VA_ARGS__); \
    } while (0);

// #define print_db(fmt)
//            do { if (DEBUG_PRINT) fprintf(stderr, fmt); } while (0)

#define print_int(x) print_db("%s = %d\n", #x, x);
#define print_str(x) print_db("%s = %s\n", #x, x);
#define print_hex(x) print_db("%s = 0x%x\n", #x, x);

#define print_tr(fmt, ...)                                \
    do                                                    \
    {                                                     \
        if (DEBUG_PRINT)                                  \
            fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                    __LINE__, __func__, __VA_ARGS__);     \
    } while (0);

#define log_info(M, ...) fprintf(stderr, WHITE "[INFO]" COLOR_X " (%s:%d:%s) " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif // __MACROS_H__