//*****************************************************************************
/**
@file		debug.h
@author		Andrii Kuplevakhskyi
@brief		Debug utlities (printf() etc.). The utilities can be
                enabled or disabled

@details
@copyright	Copyright 2016 Andrii Kuplevakhskyi
@license	Licensed under the Apache License, Version 2.0 (the "License");
		you may not use this file except in compliance with the License.
		You may obtain a copy of the License at
			http://www.apache.org/licenses/LICENSE-2.0
@par
		Unless required by applicable law or agreed to in writing,
		software distributed under the License is distributed on an
		"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
		either express or implied. See the License for the specific
		language governing permissions and limitations under the
		License.
*/
//*****************************************************************************

#ifndef DB_DEBUG_H
#define DB_DEBUG_H

#include <stdio.h>
#include "db_ctconf.h"
#include "bsp.h"

#ifdef DB_ENABLE_DEBUG
 #define DEBUG_ENABLED (1)
#else // DB_ENABLE_DEBUG
 #define DEBUG_ENABLED (0)
#endif //DB_ENABLE_DEBUG


#define DB_PRINTF(...) \
    do \
    { \
        printf_safe(__VA_ARGS__); \
    } while (1 == 0)

#define DB_PRINTF_DEBUG(...) \
    do \
    { \
        if (0 != DEBUG_ENABLED) \
        { \
            printf_safe(__VA_ARGS__); \
        } \
    } while (1 == 0)

#endif // DEBUG_H

