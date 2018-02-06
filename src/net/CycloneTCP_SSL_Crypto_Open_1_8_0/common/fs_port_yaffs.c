/**
 * @file fs_port_fatfs.c
 * @brief File system abstraction layer (FatFs)
 *
 * @section License
 *
 * Copyright (C) 2010-2017 Oryx Embedded SARL. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.0
 **/

//Dependencies
#include <string.h>
#include "fs_port.h"
#include "error.h"
#include "debug.h"

//YAFFS specific headers
#include "yaffsfs.h"

typedef struct 
{
    yaffs_DIR *d;
    char path[260];
}dir_tab_t;
//File system objects
static int fileTable[FS_MAX_FILES] = { -1 };
static dir_tab_t dirTable[FS_MAX_DIRS] = { 0 };

//Mutex that protects critical sections
static OsMutex fsMutex;


/**
 * @brief File system initialization
 * @return Error code
 **/

error_t fsInit(void)
{
    //Create a mutex to protect critical sections
    if (!osCreateMutex(&fsMutex))
    {
       //Failed to create mutex
        return ERROR_OUT_OF_RESOURCES;
    }
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Check whether a file exists
 * @param[in] path NULL-terminated string specifying the filename
 * @return The function returns TRUE if the file exists. Otherwise FALSE is returned
 **/

bool_t fsFileExists(const char_t *path)
{
   int res;
    struct yaffs_stat st;


   //Make sure the pathname is valid
   if(path == NULL)
      return FALSE;


   //Check whether the file exists
   res = yaffs_stat(path, &st);

   //Any error to report?
   if(res < 0)
      return FALSE;

   //Valid file?
   if(S_ISDIR(st.st_mode))
      return FALSE;
   else
      return TRUE;
}


/**
 * @brief Retrieve the size of the specified file
 * @param[in] path NULL-terminated string specifying the filename
 * @param[out] size Size of the file in bytes
 * @return Error code
 **/

error_t fsGetFileSize(const char_t *path, uint32_t *size)
{
   int res;
   struct yaffs_stat st;

   //Check parameters
   if(path == NULL || size == NULL)
      return ERROR_INVALID_PARAMETER;

   //Retrieve information about the specified file
   res = yaffs_stat(path, &st);

   //Any error to report?
   if(res < 0)
      return ERROR_FAILURE;
   //Valid file?
   if(S_ISDIR(st.st_mode))
      return ERROR_FAILURE;

   //Return the size of the file
   *size = st.st_size;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Rename the specified file
 * @param[in] oldPath NULL-terminated string specifying the pathname of the file to be renamed
 * @param[in] newPath NULL-terminated string specifying the new filename
 * @return Error code
 **/

error_t fsRenameFile(const char_t *oldPath, const char_t *newPath)
{
   int res;

   //Check parameters
   if(oldPath == NULL || newPath == NULL)
      return ERROR_INVALID_PARAMETER;

   //Rename the specified file
   res = yaffs_rename(oldPath, newPath);

   //Any error to report?
   if(res < 0)
      return ERROR_FAILURE;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Delete a file
 * @param[in] path NULL-terminated string specifying the filename
 * @return Error code
 **/

error_t fsDeleteFile(const char_t *path)
{
   int res;

   //Make sure the pathname is valid
   if(path == NULL)
      return ERROR_INVALID_PARAMETER;

   //Delete the specified file
   res = yaffs_unlink(path);

   //Any error to report?
   if(res < 0)
      return ERROR_FAILURE;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Open the specified file for reading or writing
 * @param[in] path NULL-terminated string specifying the filename
 * @param[in] mode Type of access permitted (FS_FILE_MODE_READ,
 *   FS_FILE_MODE_WRITE or FS_FILE_MODE_CREATE)
 * @return File handle
 **/

FsFile *fsOpenFile(const char_t *path, uint_t mode)
{
   uint_t i;
   uint_t flags;

   //File pointer
   FsFile *file = NULL;

   //Make sure the pathname is valid
   if(path == NULL)
      return NULL;

   //Enter critical section
   osAcquireMutex(&fsMutex);

   //Loop through the file objects
   for(i = 0; i < FS_MAX_FILES; i++)
   {
      //Unused file object found?
      if(fileTable[i] < 0)
      {
         //Default access mode
         flags = 0;

         //Check access mode
          if (mode & FS_FILE_MODE_READ)
              flags |= O_RDONLY;
          if (mode & FS_FILE_MODE_WRITE)
              flags |= O_WRONLY;
          if (mode & FS_FILE_MODE_CREATE)
              flags |= O_CREAT;
          if (mode & FS_FILE_MODE_TRUNC)
              flags |= O_TRUNC;

         //Open the specified file
          fileTable[i] = yaffs_open(path, flags, S_IWRITE | S_IREAD);

         //Check status code
          if (fileTable[i] >= 0)
            file = &fileTable[i];

         //Stop immediately
         break;
      }
   }

   //Leave critical section
   osReleaseMutex(&fsMutex);
   //Return a handle to the file
   return file;
}


/**
 * @brief Move to specified position in file
 * @param[in] file Handle that identifies the file
 * @param[in] offset Number of bytes to move from origin
 * @param[in] origin Position used as reference for the offset (FS_SEEK_SET,
 *   FS_SEEK_CUR or FS_SEEK_END)
 * @return Error code
 **/

error_t fsSeekFile(FsFile *file, int_t offset, uint_t origin)
{
   //Make sure the file pointer is valid
   if(file == NULL)
      return ERROR_INVALID_PARAMETER;

   //Offset is relative to the current file pointer position?
   if(origin == FS_SEEK_CUR)
      offset = yaffs_lseek(*(int *)file, offset, SEEK_CUR);
   //Offset is relative to the end of the file?
   else if(origin == FS_SEEK_END)
      offset = yaffs_lseek(*(int *)file, offset, SEEK_END);

   //Any error to report?
    if (offset < 0)
      return ERROR_FAILURE;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Write data to the specified file
 * @param[in] file Handle that identifies the file to be written
 * @param[in] data Pointer to a buffer containing the data to be written
 * @param[in] length Number of data bytes to write
 * @return Error code
 **/

error_t fsWriteFile(FsFile *file, void *data, size_t length)
{
   int n;

   //Make sure the file pointer is valid
   if(file == NULL)
      return ERROR_INVALID_PARAMETER;

   //Write data
   n = yaffs_write(*(int *)file, data, length);

   //Sanity check
   if(n != length)
      return ERROR_FAILURE;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Read data from the specified file
 * @param[in] file Handle that identifies the file to be read
 * @param[in] data Pointer to the buffer where to copy the data
 * @param[in] size Size of the buffer, in bytes
 * @param[out] length Number of data bytes that have been read
 * @return Error code
 **/

error_t fsReadFile(FsFile *file, void *data, size_t size, size_t *length)
{
   int n;

   //No data has been read yet
   *length = 0;

   //Make sure the file pointer is valid
   if(file == NULL)
      return ERROR_INVALID_PARAMETER;

   //Read data
   n = yaffs_read(*(int *)file, data, size);

   //End of file?
   if(!n)
      return ERROR_END_OF_FILE;

   //Total number of data that have been read
   *length = n;
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Close a file
 * @param[in] file Handle that identifies the file to be closed
 **/

void fsCloseFile(FsFile *file)
{
    int i;
   //Make sure the file pointer is valid
   if(file != NULL)
   {
      //Enter critical section
      osAcquireMutex(&fsMutex);

      //Close the specified file
      yaffs_close(*(int *)file);
       
      //Mark the corresponding entry as free
//       for (i = 0; i < FS_MAX_FILES; i++)
//       {
//           if (fileTable[i] == *(int*)file)
//           {
//               fileTable[i] = -1;
//           }
//       }
       *(int*)file = -1;

      //Leave critical section
      osReleaseMutex(&fsMutex);
   }
}


/**
 * @brief Check whether a directory exists
 * @param[in] path NULL-terminated string specifying the directory path
 * @return The function returns TRUE if the directory exists. Otherwise FALSE is returned
 **/

bool_t fsDirExists(const char_t *path)
{
   int res;
   struct yaffs_stat st;

   //Make sure the pathname is valid
   if(path == NULL)
      return FALSE;

   //Root directory?
   if(!strcmp(path, "/"))
      return TRUE;

   //Check whether the file exists
   res = yaffs_stat(path, &st);

   //Any error to report?
   if(res < 0)
      return FALSE;

   //Valid directory?
   if(S_ISDIR(st.st_mode))
      return TRUE;
   else
      return FALSE;
}


/**
 * @brief Create a directory
 * @param[in] path NULL-terminated string specifying the directory path
 * @return Error code
 **/

error_t fsCreateDir(const char_t *path)
{
   int res;

   //Make sure the pathname is valid
   if(path == NULL)
      return ERROR_INVALID_PARAMETER;

   //Create a new directory
    res = yaffs_mkdir(path, S_IREAD | S_IWRITE);

   //Any error to report?
   if(res != 0)
      return ERROR_FAILURE;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Remove a directory
 * @param[in] path NULL-terminated string specifying the directory path
 * @return Error code
 **/

error_t fsRemoveDir(const char_t *path)
{
   int res;

   //Make sure the pathname is valid
   if(path == NULL)
      return ERROR_INVALID_PARAMETER;

   //Remove the specified directory
   res = yaffs_rmdir(path);

   //Any error to report?
   if(res != 0)
      return ERROR_FAILURE;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Open a directory stream
 * @param[in] path NULL-terminated string specifying the directory path
 * @return Directory handle
 **/

FsDir *fsOpenDir(const char_t *path)
{
   uint_t i;

   //Directory pointer
   FsDir *dir = NULL;

   //Make sure the pathname is valid
   if(path == NULL)
      return NULL;

   //Enter critical section
   osAcquireMutex(&fsMutex);

   //Loop through the directory objects
   for(i = 0; i < FS_MAX_DIRS; i++)
   {
      //Unused directory object found?
      if(dirTable[i].d == NULL)
      {
         //Open the specified directory
          dirTable[i].d = yaffs_opendir(path);

         //Check status code
          if (dirTable[i].d != NULL)
          {
             strcpy(dirTable[i].path, path);
             dir = dirTable[i].d;
          }

         //Stop immediately
         break;
      }
   }

   //Leave critical section
   osReleaseMutex(&fsMutex);
   //Return a handle to the directory
   return dir;
}


/**
 * @brief Read an entry from the specified directory stream
 * @param[in] dir Handle that identifies the directory
 * @param[out] dirEntry Pointer to a directory entry
 * @return Error code
 **/

error_t fsReadDir(FsDir *dir, FsDirEntry *dirEntry)
{
   int res;
    int i;
   struct yaffs_dirent *de;
   struct yaffs_stat st;
   char_t *fn;
   size_t n;
    char_t path[260];
    struct tm *ptm;

   //Make sure the directory pointer is valid
   if(dir == NULL)
      return ERROR_INVALID_PARAMETER;

   //Read the specified directory
    de = yaffs_readdir((yaffs_DIR *)dir);
    for (i = 0; i < FS_MAX_DIRS; i++)
    {
        if (dirTable[i].d == dir)
        {
            sprintf(path, "%s/%s", dirTable[i].path, de->d_name);
            break;
        }
    }
    
    yaffs_stat(path, &st);
    
   //End of the directory stream?
    if (de == NULL)
      return ERROR_END_OF_STREAM;

   //Point to the long filename
   fn = de->d_name;
    
   //File attributes
    if (S_ISDIR(st.st_mode))
    {
        dirEntry->attributes = FS_FILE_ATTR_DIRECTORY;
    }
    if (S_ISREG(st.st_mode))
    {
        dirEntry->attributes = FS_FILE_ATTR_SYSTEM;
    }
        
   //File size
   dirEntry->size = st.st_size;
   //Last modified date
    ptm = localtime(&st.yst_mtime);
   dirEntry->modified.year = 1900 + ptm->tm_year;
   dirEntry->modified.month = ptm->tm_mon + 1;
   dirEntry->modified.day = ptm->tm_mday;
   dirEntry->modified.dayOfWeek = 0;
   //Last modified time
   dirEntry->modified.hours = ptm->tm_hour;
   dirEntry->modified.minutes = ptm->tm_min;
   dirEntry->modified.seconds = ptm->tm_sec;
   dirEntry->modified.milliseconds = 0;

   //Make sure the date is valid
   dirEntry->modified.month = MAX(dirEntry->modified.month, 1);
   dirEntry->modified.month = MIN(dirEntry->modified.month, 12);
   dirEntry->modified.day = MAX(dirEntry->modified.day, 1);
   dirEntry->modified.day = MIN(dirEntry->modified.day, 31);

   //Retrieve the length of the file name
   n = strlen(fn);
   //Limit the number of characters to be copied
   n = MIN(n, FS_MAX_NAME_LEN);

   //Copy file name
   strncpy(dirEntry->name, fn, n);
   //Properly terminate the string with a NULL character
   dirEntry->name[n] = '\0';

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Close a directory stream
 * @param[in] dir Handle that identifies the directory to be closed
 **/

void fsCloseDir(FsDir *dir)
{
    int res;
    int i;
   //Make sure the directory pointer is valid
   if(dir != NULL)
   {
      //Enter critical section
      osAcquireMutex(&fsMutex);

      res = yaffs_closedir((yaffs_DIR *)dir);
      

      //Mark the corresponding entry as free
       for (i = 0; i < FS_MAX_DIRS; i++)
       {
           if (dirTable[i].d == dir)
               dirTable[i].d = NULL;
       }
       
      //Leave critical section
      osReleaseMutex(&fsMutex);
   }
}
