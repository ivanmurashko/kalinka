/**
   @file utils.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

   Copyright (c) 2007-2012 Kalinka Team

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   CHANGE HISTORY

   @date
   - 2007/May/23 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/


#ifndef KLK_UTILS_H
#define KLK_UTILS_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <vector>

#include "common.h"
#include "errors.h"
#include "stringlist.h"
#include "binarydata.h"

namespace klk
{
    namespace base
    {

        /** @defgroup grCommonUtils Several useful utilites

            Several utilities

            @ingroup grCommon

            @{
        */

        /**
           @brief Utils holder

           Utils holder
        */
        class Utils
        {
        public:
            /**
               Constructor
            */
            Utils(){}

            /**
               Destructor
            */
            ~Utils(){}

            /**
               Retrives all info from a config file
               and removes all comments from it

               @param[in] config - the config file

               @return the list with file items

               @exception @ref klk::Exception
            */
            static const StringList getConfig(const std::string& config);

            /**
               Reads a folder and provides a list with filename from the
               folder

               @param[in] folder - the folder with files
               @param[in] recursive - should we retrive files
               from sub folders (true) or not (false)

               @exception @ref klk::Exception
            */
            static const StringList getFiles(const std::string& folder, bool recursive = false);

            /**
               Splits data by the delimiters

               @param[in] data - the string with data
               @param[in] del - the string with delimiters

               @note empty results will be ignored
            */
            static const std::vector<std::string> split(
                const std::string& data,
                const std::string& del)
                throw();

            /**
               Generates uuid
            */
            static const std::string generateUUID();

            /**
               Replaces all substr at a string
            */
            static const std::string
                replaceAll(const std::string& initial,
                           const std::string& replaceWhat,
                           const std::string& replaceWithWhat);

            /**
               Gets current time in format specified as input parameter

               @param[in] format - the time format see man strftime

               @return the formated time
            */
            static const std::string getCurrentTime(const std::string& format);

            /**
               Converts string to lower case

               @return the lowercased string
            */
            static const std::string toLower(const std::string& initial);

            /**
               Converts seconds into DDD HH:MM:SS
            */
            static const std::string time2Str(time_t time);

            /**
               @brief checks an existance of a file that
               is specified at the argument

               Checks an existance of a file that is specified
               at the argument

               @param[in] path the path that have to be checked

               @return
               - false the path doesn't exist
               - true the path exist
            */
            static bool fileExist(const std::string& path) throw();

            /**
               Do alignment

               @param[in] initial - the initial string
               @param[in] size - the alignment size

               @return the alignmented string
            */
            static const std::string align(const std::string& initial,
                                           size_t size) throw();


            /**
               Converts a list to a string with items separated
               by the specified separator

               @param[in] list - the list to be converted
               @param[in] sep - the used separator

               @return the result
            */
            static const std::string convert2String(const StringList& list,
                                                    const std::string& sep);


            /**
               Reads the data from a file to binary data

               @param[in] fname  - the file name
               @param[in] size - the data size to be read

               @return the data

               @exception klk::Exception
            */
            static const klk::BinaryData
                readDataFromFile(const std::string& fname, size_t size);

            /**
               Reads the whole data from a file

               @param[in] fname  - the file name

               @return the data

               @exception klk::Exception
            */
            static const klk::BinaryData
                readWholeDataFromFile(const std::string& fname);


            /**
               Saves the data to a file

               @param[in] fname  - the file name
               @param[in] data - the data container

               @exception klk::Exception
            */
            static void saveData2File(const std::string& fname,
                                      const klk::BinaryData& data);

            /**
               Script type
            */
            typedef enum
            {
                SHELL = 0,
                PERL = 1
            } ScriptType;

            /**
               Runs a script

               @param[in] type - the script type
               @param[in] path to the script
               @param[in] command - the command

               @note relative path will be started from install dir
               absolute - from real

               @exception klk::Exception
            */
            static void runScript(ScriptType type,
                                  const std::string& path,
                                  const std::string& command);

            /**
               @brief Wrapper for unlink sys call

               Removes a file/folder specified at the argument

               @param[in] path path to be unlinked

               @note nothing to do if the file does not exist
               @see unlink(2)

               @exception klk::Exception
            */
            static void unlink(const std::string& path);

            /**
               Checks is the path a folder or not

               @param[in] path path to the dir

               @return
               - true - the path is a folder
               - false - the path is not a folder

               @exception klk::Exception
            */
            static bool isDir(const std::string& path);

            /**
               @brief klk_mkdir creates a dir

               Creates a dir

               @note the function is recursive -
               makes parent directories as needed

               @param[in] path path to the dir

               @exception klk::Exception
            */
            static void mkdir(const std::string& path);

            /**
               Creates a parent folder for the object specified
               as argument (file or folder)

               @param[in] path path to the original file/dir

               For example path is /tmp/folder/file.txt
               The method will create /tmp/folder folder

               @exception klk::Exception
            */
            static void mkParentDir(const std::string& path);

            /**
               Converts percent to string

               For example 0.106 -> 10.60%

               @param[in] val - the value to be converted

               @return the string
            */
            static const std::string percent2Str(const double val);

            /**
               Converts memory size to string


               For example 1362452 -> 13 kb

               @param[in] val - the value to be converted

               @return the string
            */
            static const std::string memsize2Str(const long val);

            /**
               Retrives file extension

               @param[in] fname - the file name

               /tmp/file.ext => ext

               @return the extension
            */
            static const std::string getFileExt(const std::string& fname);

            /**
               Retrives file name

               @param[in] fpath - the file path

               /tmp/file.ext => file.ext

               @return the file name
            */
            static const std::string getFileName(const std::string& fpath);

            /**
               give the character length for an UF8 string

               @param[in] text - the text string in utf8

               @return the character count
            */
            static size_t charLength(const std::string& text);
        private:
            /**
               Removes a dir

               @param[in] path - the folder path to be removed

               used at the unlink method

               @exception klk::Exception
            */
            static void rmDir(const std::string& path);
        };

        /** @} */

    }
}

/**
   wrapper for close sys call

   @see close(2)

   @param[in] i_iFD file descriptior to be closed

   @return
   - @ref klk::OK the opperation was successful
   - @ref klk::ERROR the operation was failed

   @ingroup grCommonUtils
*/
klk::Result klk_close(int i_iFD);


#endif //KLK_UTILS_H
