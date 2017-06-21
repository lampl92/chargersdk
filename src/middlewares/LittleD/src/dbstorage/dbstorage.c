/******************************************************************************/
/**
@file		dbstorage.c
@author		Graeme Douglas
@brief		The databases storage layer.
@details
@see		For more information, refer to @ref dbstorage.h
@copyright	Copyright 2013 Graeme Douglas
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
/******************************************************************************/

#include "dbstorage.h"
#include "../db_ctconf.h"

db_int db_fileexists(char *filename)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	int check = cfs_open(filename, CFS_READ);
	if (-1 == check)
		return 0;

	cfs_close(check);
	return 1;
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	return SD_File_Exists(filename);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FIL check;
    FRESULT res;
    res = f_open(&check, filename, FA_OPEN_EXISTING);
    printf_safe("openexist res = %d\n", res);
    if(FR_OK == res)
    {
        f_close(&check);
        return 1;
    }
    return 0;

#else
	FILE* check = fopen(filename, "rb");
	if (NULL == check)
		return 0;

	fclose(check);
	return 1;
#endif
}

db_fileref_t db_openreadfile(char *filename)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return cfs_open(filename, CFS_READ);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	db_fileref_t toret;
	if (1 != SD_File_Open(&toret, filename, SD_FILE_MODE_READ))
		return DB_STORAGE_NOFILE;
	return toret;
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FIL *toret;
    FRESULT res;
    toret = (FIL *)malloc(sizeof(FIL));
    res = f_open(toret, filename, FA_READ);
    if(FR_OK != res)
    {
        free(toret);
        return DB_STORAGE_NOFILE;
    }
    return toret;
#else
	return fopen(filename, "rb");
#endif
}

db_fileref_t db_openwritefile(char *filename)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return cfs_open(filename, CFS_READ|CFS_WRITE);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	SD_File_Remove(filename);
	db_fileref_t toret;
	if (1 != SD_File_Open(&toret, filename, SD_FILE_MODE_WRITE))
		return DB_STORAGE_NOFILE;
	return toret;
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FIL *toret;
    FRESULT res;
    toret = (FIL *)malloc(sizeof(FIL));
    res = f_open(toret, filename, FA_CREATE_NEW | FA_WRITE);
    if(FR_OK != res && FR_EXIST != res)
    {
        printf_safe("openwrite res = %d\n", res);
        free(toret);
        return DB_STORAGE_NOFILE;
    }
    return toret;
#else
	return fopen(filename, "wb");
#endif
}

db_fileref_t db_openappendfile(char *filename)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return cfs_open(filename, CFS_READ|CFS_WRITE|CFS_APPEND);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	db_fileref_t toret;
	if (1 != SD_File_Open(&toret, filename, SD_FILE_MODE_WRITE))
		return DB_STORAGE_NOFILE;
	return toret;
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FIL *toret;
    FRESULT res;
    toret = (FIL *)malloc(sizeof(FIL));
    res = f_open(toret, filename, FA_OPEN_APPEND);
    if(FR_OK != res)
    {
        free(toret);
        return DB_STORAGE_NOFILE;
    }
    return toret;
#else
	return fopen(filename, "ab");
#endif
}

size_t db_fileread(db_fileref_t f, unsigned char *dest, size_t numbytes)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return (size_t)cfs_read(f, dest, numbytes);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	return SD_File_Read(f, dest, numbytes);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FRESULT res;
    UINT br;
    res = f_read(f, dest, numbytes, &br);
    if(FR_OK != res)
    {
        return 0;
    }
    return (size_t)br;
#else
	return numbytes*fread(dest, numbytes, 1, f);
#endif
}

size_t db_filewrite(db_fileref_t f, void *towrite, size_t numbytes)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return (size_t)cfs_write(f, towrite, numbytes);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	return SD_File_Write(f, towrite, numbytes);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FRESULT res;
    UINT btw;
    res = f_write(f, towrite, numbytes, &btw);
    if(FR_OK != res)
    {
        return 0;
    }
    return (size_t)btw;
#else
	return numbytes*fwrite(towrite, numbytes, 1, f);
#endif
}

db_int db_filerewind(db_fileref_t f)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return cfs_seek(f, 0, CFS_SEEK_SET);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	return SD_File_Seek(f, 0);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FRESULT res;
    res = f_lseek(f, 0);
    if(FR_OK != res)
    {
        return 0;
    }
    return 1;
#else
	rewind(f);
	return 1;
#endif
}

db_int db_fileseek(db_fileref_t f, size_t size)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return cfs_seek(f, size, CFS_SEEK_CUR);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	size_t pos = SD_File_Position(f);
	return SD_File_Seek(f, pos+size);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FRESULT res;
    res = f_lseek(f, size);
    if(FR_OK != res)
    {
        return 0;
    }
    return 1;
#else
	return fseek(f, size, SEEK_CUR);
#endif
}

db_int db_fileclose(db_fileref_t f)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	cfs_close(f);
	return 1;
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	SD_File_Close(f);
    return 1;
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FRESULT res;
    res = f_close(f);
    if(FR_OK != res)
    {
        return 0;
    }
    free(f);
    return 1;
#else
	return fclose(f);
#endif
}

db_int db_fileremove(char *filename)
{
#if DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_CONTIKI
	return (0==cfs_remove(filename));
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_ARDUINO
	return SD_File_Remove(filename);
#elif DB_CTCONF_SETTING_TARGET == DB_CTCONF_OPTION_TARGET_FatFS
    FRESULT res;
    res = f_unlink(filename);
    if(FR_OK != res)
    {
        return 0;
    }
    return 1;
#else
	return (0==remove(filename));
#endif
}
