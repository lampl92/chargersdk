/**
* @file minunit.h
* @brief see more in readme.md or http://www.jera.com/techinfo/jtns/jtn002.html
* @author rgw
* @version v1.0
* @date 2016-10-27
*/

#ifndef MINUNIT_H
#define MINUNIT_H

/* ---------------------------------------------------------------------------*/
/**
* @brief 
*
* @param message
* @param test
*
* @return 
*/
/* ---------------------------------------------------------------------------*/
#define mu_assert(message, test) do {   \
            if (!(test))                \
            return message;             \
        } while (0)

/* ---------------------------------------------------------------------------*/
/**
* @brief 
*
* @param test
*
* @return 
*/
/* ---------------------------------------------------------------------------*/
#define mu_run_test(test) do {  \
        char *message = test(); \
        tests_run++;            \
        if (message)            \
            return message;     \
        } while (0)


extern int tests_run;

#endif
