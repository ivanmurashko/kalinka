/**
   @file utils.cpp
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <uuid/uuid.h>
#include <time.h>
#include <sys/wait.h>

#include <fstream>
#include <numeric>    // for accumulate
#include <iomanip>    // for setprecision


#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>


#include "utils.h"
#include "log.h"
#include "exception.h"
#include "paths.h"

using namespace klk;
using namespace klk::base;


// wrapper for close sys call
// @see close(2)
// @param[in] i_iFD file descriptior to be closed
Result klk_close(int i_iFD)
{
    if (close(i_iFD) < 0)
    {
        klk_log(KLKLOG_ERROR, "Error %d in close(): %s",
                errno, strerror(errno));
        return ERROR;
    }
    return OK;
}

//
// Utils class
//

// Reads a folder and provides a list with filename from the
// folder
// @param[in] folder - the folder with files
// @exception @ref Result
const StringList Utils::getFiles(const std::string& folder, bool recursive)
{
    DIR* dir = NULL;
    BOOST_ASSERT(Utils::fileExist(folder) == true);
    StringList res;
    dir = opendir(folder.c_str());
    if (dir != NULL)
    {
        struct dirent* dir_info = NULL;
        // Process all files in the subdirectory
        while ((dir_info = readdir(dir)))
        {
            if ((strcmp(dir_info->d_name, ".") != 0) &&
                (strcmp(dir_info->d_name, "..") != 0))
            {
                std::string file_name = folder;
                file_name += "/";
                file_name += dir_info->d_name;

                struct stat st;

                if (lstat(file_name.c_str(), &st) < 0)
                {
                    klk_log(KLKLOG_ERROR, "Error %d in lstat(): %s",
                            errno, strerror(errno));
                    KLKASSERT(0);
                    continue;
                }

                // process only reg file
                if (S_ISREG(st.st_mode))
                {
                    res.push_back(file_name);
                }
                else if (S_ISDIR(st.st_mode) && recursive)
                {
                    StringList files = getFiles(file_name, recursive);
                    std::copy(files.begin(), files.end(),
                              std::back_inserter(res));
                }
            }
        }

        closedir(dir);
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in opendir() for %s: %s", errno,
                             folder.c_str(), strerror(errno));
    }

    return res;
}

// Generates uuid
const std::string Utils::generateUUID()
{
    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[36 + 1];
    uuid_unparse(uuid, uuid_str);
    return uuid_str;
}

// Replaces all substr at a string
const std::string Utils::replaceAll(
    const std::string& initial,
    const std::string& replaceWhat,
    const std::string& replaceWithWhat)
{
    std::string result(initial);
    std::size_t pos = 0;
    while(1)
    {
        pos = result.find(replaceWhat, pos);
        if (pos == std::string::npos)
            break;
        result.replace(pos,replaceWhat.size(),replaceWithWhat);
        pos++;
    }
    return result;
}

// Gets current time in format specified as input parameter
// @param[in] format - the time format see man strftime
const std::string Utils::getCurrentTime(const std::string& format)
{
    struct tm lt;
    time_t now = time(NULL);
    if (localtime_r(&now, &lt) == NULL)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in localtime_r(): %s",
                             errno, strerror(errno));
    }
    char buff[512];
    strftime(buff, sizeof(buff) - 1, format.c_str(), &lt);
    return std::string(buff);
}

// Converts string to lower case
// @return the lowercased string
const std::string Utils::toLower(const std::string& initial)
{
    std::string result(initial);
    boost::to_lower(result);
    return result;
}

// Converts seconds into DDD HH:MM:SS
const std::string Utils::time2Str(time_t time)
{
    std::string result = "";

    try
    {
        const time_t SECOND = 1;
        const time_t MINUTE = SECOND * 60;
        const time_t HOUR = MINUTE*60;
        const time_t DAY = HOUR*24;
        time_t day = time / DAY;
        if (day > 0)
        {
            time -= day * DAY;
            result += boost::lexical_cast<std::string>(day);
            result += " d. ";
        }
        int hour = time / HOUR;
        if (hour > 0)
        {
            time -= hour * HOUR;
        }
        int minute = time / MINUTE;
        if (minute > 0)
        {
            time -= minute * MINUTE;
        }
        int second = time;
        char buff[128];
        snprintf(buff, sizeof(buff) - 1, "%.2d:%.2d:%.2d",
                 hour, minute, second);
        result += buff;
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }
    return result;
}

// Retrives all info from a config file
// and removes all comments from it
const StringList Utils::getConfig(const std::string& config)
{
    StringList result;
    try
    {
        std::ifstream file;

        file.open(config.c_str(), std::ifstream::in);

        if (file.fail())
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in open(): %s. File name: '%s'",
                                 errno, strerror(errno), config.c_str());
        }

        while (file.good())
        {
            char buff[1024];
            file.getline(buff, sizeof(buff));
            std::string line = buff;

            // remove comments
            std::string::size_type pos = line.find('#');
            if (pos != std::string::npos)
                line.erase(pos, std::string::npos);
            // remove garbage
            boost::trim_if(line,  boost::is_any_of(" \r\n\t"));
            if (line.empty())
            {
                continue; // ignore it
            }

            result.push_back(line);
        }

        file.close();
    }
    catch(Exception& err)
    {
        throw err;
    }
    catch (std::out_of_range&)
    {
        throw Exception(__FILE__, __LINE__, err::OUTRANGE);
    }
    catch(...)
    {
        throw Exception(__FILE__, __LINE__, err::UNSPECIFIED);
    }

    return result;
}

// Splits data by the delimiters
const std::vector<std::string> Utils::split(const std::string& data,
                                            const std::string& del)
    throw()
{
    std::vector<std::string> result;
    boost::split(result, data, boost::is_any_of(del));
    std::vector<std::string>::iterator new_end =
        std::remove_if(result.begin(), result.end(),
                       boost::bind(&std::string::empty, _1));
    result.erase(new_end, result.end());

    return result;
}

// checks an existance of a file that is specified at the argument
bool Utils::fileExist(const std::string& path) throw()
{
    if (access(path.c_str(), F_OK) == 0)
        return true;
    return false;
}

// Do alignment
const std::string Utils::align(const std::string& initial, size_t size)
     throw()
{
    std::string result(initial);
    for (size_t i = charLength(initial); i < size; i++)
        result += " ";
    return result;
}


// Reads the data from a file to binary data
const BinaryData
Utils::readDataFromFile(const std::string& fname, size_t size)
{
    BinaryData data(size);
    BOOST_ASSERT(fileExist(fname));
    int fd = open(fname.c_str(),  O_RDONLY);
    if (fd < 0)
    {
        throw Exception(__FILE__, __LINE__, "Error %d in open(); %s",
                             errno, strerror(errno));
    }

    try
    {
        size_t read_count = read(fd, data.toVoid(), size);
        if (read_count < 0)
        {
            throw Exception(__FILE__, __LINE__, "Error %d in read(); %s",
                                 errno, strerror(errno));
        }
        if (read_count != size)
        {
            data.resize(read_count);
        }
    }
    catch(...)
    {
        close(fd);
        throw;
    }
    close(fd);

    return data;
}

// Reads the whole data from a file
const BinaryData
Utils::readWholeDataFromFile(const std::string& fname)
{
    // retrive the size of the file
    BOOST_ASSERT(fileExist(fname));
    int fd = open(fname.c_str(),  O_RDONLY);
    if (fd < 0)
    {
        throw Exception(__FILE__, __LINE__, "Error %d in open(); %s",
                             errno, strerror(errno));
    }

    off_t size = 0;
    try
    {
        size = lseek(fd, 0L, SEEK_END);
        if (size < 0)
        {
            throw Exception(__FILE__, __LINE__, "Error %d in lseek(); %s",
                                 errno, strerror(errno));
        }
    }
    catch(...)
    {
        klk_close(fd);
        throw;
    }

    close(fd);
    return readDataFromFile(fname, size);
}

// Saves the data to a file
void Utils::saveData2File(const std::string& fname,
                          const BinaryData& data)
{
    int fd = open(fname.c_str(),  O_WRONLY|O_APPEND|O_CREAT);
    if (fd < 0)
    {
        throw Exception(__FILE__, __LINE__, "Error %d in open(): %s",
                             errno, strerror(errno));
    }

    try
    {
        size_t wcount = write(fd, data.toVoid(), data.size());
        if (wcount != data.size())
        {
            throw Exception(__FILE__, __LINE__, "Error %d in write(): %s",
                                 errno, strerror(errno));
        }
    }
    catch(...)
    {
        close(fd);
        throw;
    }
    close(fd);

    // change permsiisons to rw for owner
    if (chmod (fname.c_str(), S_IRUSR|S_IWUSR) < 0)
    {
        throw Exception(__FILE__, __LINE__, "Error %d in chmod(): %s",
                             errno, strerror(errno));
    }
}


// Runs a script
void Utils::runScript(ScriptType type,
                      const std::string& path,
                      const std::string& command)
{
    std::string real_path = path;
    if (*path.begin() != '/')
    {
        real_path = dir::INST + "/" + path;
    }

    if (Utils::fileExist(real_path) == false)
    {
        throw Exception(__FILE__, __LINE__,
                             "Path '" + real_path + "' does not exist");
    }

    const char* cmd = NULL;
    if (command.empty() == false)
    {
        cmd = command.c_str();
    }


    std::string prg, prg_name;
    if (type == SHELL)
    {
        prg = "/bin/sh";
        prg_name = "sh";
    }
    else if (type == PERL)
    {
        prg = dir::PERL;
        prg_name = "perl";
    }
    else
    {
        throw Exception(__FILE__, __LINE__,
                             "Unsupported script: " +
                             real_path);
    }
    BOOST_ASSERT(prg.empty() == false);
    BOOST_ASSERT(prg_name.empty() == false);

    if (Utils::fileExist(prg) == false)
    {
        throw Exception(__FILE__, __LINE__,
                             "Path '" + std::string(prg) +
                             "' does not exist");
    }

    pid_t pid = 0;
    int status = 0;

    switch (pid = fork())
    {
    case -1:
        throw Exception(__FILE__, __LINE__, "Error %d in fork(): %s\n",
                             errno, strerror(errno));
    case 0:
        execl(prg.c_str(), prg_name.c_str(), real_path.c_str(),
              cmd, NULL);
        exit(errno);
    }

    // wait for child end
    if ((waitpid(pid, &status, 0) == pid) < 0)
    {
        throw Exception(__FILE__, __LINE__, "Error %d in waitpid(): %s",
                        errno, strerror(errno));
    }

    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) == 0)
            return;
    }
    throw Exception(__FILE__, __LINE__, "Script startup failed(): %s %s %s",
                    prg.c_str(), real_path.c_str(), cmd);
}

// Removes a file/folder specified at the argument
void Utils::unlink(const std::string& path)
{
    if (access(path.c_str(), F_OK) == 0)
    {
        // check that the parent is a folder
        struct stat st;

        if (lstat(path.c_str(), &st) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in lstat(): %s",
                                 errno, strerror(errno));
        }

        // check that there is a folder
        if (S_ISDIR(st.st_mode))
        {
            rmDir(path);
        }
        else if (::unlink(path.c_str()) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in unlink(): %s. Argument: '%s'",
                                 errno, strerror(errno),
                                 path.c_str());
        }
    }
}

// Checks is the path a folder or not
bool Utils::isDir(const std::string& path)
{
    boost::filesystem::path check_path(path);
    if (boost::filesystem::exists(check_path))
    {
        return boost::filesystem::is_directory(path);
    }
    return false;
}


// Creates a dir
void Utils::mkdir(const std::string& path)
{
    if (fileExist(path))
    {
        if (!isDir(path))
        {
            throw Exception(__FILE__, __LINE__,
                                 "Path '%s' is not a folder.",
                                 path.c_str());
        }
        return; // nothing to do
    }

    BOOST_ASSERT(path.empty() == false);

    // make parent
    mkParentDir(path);

    if (::mkdir(path.c_str(), 0775) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in mkdir() for %s: %s",
                             errno, path.c_str(), strerror(errno));
    }
}

// Removes a dir
void Utils::rmDir(const std::string& path)
{
    BOOST_ASSERT(isDir(path) == true);

    DIR* dir= opendir(path.c_str());

    if (dir == NULL)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in opendir() for %s: %s",
                             errno, path.c_str(), strerror(errno));
    }

    try
    {
        struct dirent *dir_info = NULL;
        // Process all entries in the subdirectory
        while ((dir_info = readdir(dir)) != NULL)
        {
            if ((strcmp(dir_info->d_name, ".") != 0) &&
                (strcmp(dir_info->d_name, "..") != 0))
            {
                std::string file_name = path;
                file_name += "/";
                file_name += dir_info->d_name;
                Utils::unlink(file_name);
            }
        }
    }
    catch(...)
    {
        closedir(dir);
        throw;
    }
    closedir(dir);

    if (rmdir(path.c_str()) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in rmdir() for %s: %s",
                             errno, path.c_str(), strerror(errno));
    }
}

// Creates a parent folder for the object specified
// as argument (file or folder)
// For example path is /tmp/folder/file.txt
// The method will create /tmp/folder folder
void Utils::mkParentDir(const std::string& path)
{
    std::string::size_type pos = path.rfind('/');
    BOOST_ASSERT(pos != std::string::npos);
    std::string parent(path, 0, pos);
    if (Utils::fileExist(parent) == false)
    {
        Utils::mkdir(parent);
    }
    else
    {
        if (!isDir(parent))
        {
            throw Exception(__FILE__, __LINE__,
                                 "Parent path '%s' is not a folder. "
                                 "The mkdir() for %s is failed",
                                 parent.c_str(),
                                 path.c_str());
        }
    }
}

/**
   Accumulator class

   @ingroup grNetModule
*/
struct StringListAccumulator
{
    const std::string operator() (const std::string& initial,
                                  const std::string& element,
                                  const std::string& sep)
    {
        if (initial.empty())
        {
            return element;
        }
        else
        {
            return initial + sep + element;
        }
    }
};


// Converts a list to a string with items separated
// by the specified separator
const std::string
Utils::convert2String(const StringList& list, const std::string& sep)
{
    const std::string result = std::accumulate(
        list.begin(),
        list.end(), std::string(""),
        boost::bind<std::string>(
            StringListAccumulator(), _1, _2, sep));

    return result;
}


// Converts percent to string
const std::string Utils::percent2Str(const double val)
{
    if (val > 1)
    {
        return "> 100%";
    }

    if (val < 0)
    {
        return "< 0%";
    }

    std::stringstream result;
    result << std::setprecision(4) << val * 100 << "%";
    return result.str();
}

// Converts memory size to string
const std::string Utils::memsize2Str(const long val)
{
    std::stringstream result;
    if (val > 1024 * 1024 * 1024)
    {
        result << std::setprecision(4) << val/(1024*1024*1024) << " Gb";
    }
    else if (val > 1024 * 1024)
    {
        result << std::setprecision(4) << val/(1024*1024) << " Mb";
    }
    else if (val > 1024)
    {
        result << std::setprecision(4) << val/1024 << " kb";
    }
    else
    {
        result << std::setprecision(4) << val << " b";
    }

    return result.str();
}

// Retrives file extension
const std::string Utils::getFileExt(const std::string& fname)
{
    size_t i = fname.rfind('.', fname.length( ));
    if (i != std::string::npos)
    {
        return(fname.substr(i + 1, fname.length( ) - i));
    }

    return fname;
}

// Retrives file extension
const std::string Utils::getFileName(const std::string& fpath)
{
    size_t i = fpath.rfind('/', fpath.length( ));
    if (i != std::string::npos)
    {
        return(fpath.substr(i + 1, fpath.length( ) - i));
    }

    return fpath;
}

// give the character length for an UF8 string
// gotten from
// http://rosettacode.org/wiki/String_Character_Length#Character_Length_8
// http://porg.es/blog/counting-characters-in-utf-8-strings-is-faster
size_t Utils::charLength(const std::string& text)
{
    std::size_t i = 0;

    const char* s = text.c_str();

    //Go fast if string is only ASCII.
    //Loop while not at end of string,
    // and not reading anything with highest bit set.
    //If highest bit is set, number is negative.
    while (s[i] > 0)
        i++;

    if (s[i] <= -65) // all follower bytes have values below -65
    {
        throw Exception(__FILE__, __LINE__,
                             "Incorrect utf string: " + text);
    }

    //Note, however, that the following code does *not*
    // check for invalid characters.
    //The above is just included to bail out on the tests :)

    std::size_t count = i;
    while (s[i])
    {
        //if ASCII just go to next character
        if (s[i] > 0)
        {
            i += 1;
        }
        else
        {
            //select amongst multi-byte starters
            switch (0xF0 & s[i])
            {
            case 0xE0: i += 3; break;
            case 0xF0: i += 4; break;
            default:   i += 2; break;
            }
        }

        ++count;

        if (i > text.size())
        {
            throw Exception(__FILE__, __LINE__,
                            "Incorrect utf string: " + text);
        }
    }
    return count;
}
