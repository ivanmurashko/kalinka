/**
   @file fileinfo.h
   @brief This file is part of Kalinka mediaserver.
   @author Ivan Murashko <ivan.murashko@gmail.com>

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
   - 2009/03/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2010/02/20 klk::file::FolderInfo class was added by ipp
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_FILEINFO_H
#define KLK_FILEINFO_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "ifactory.h"
#include "thread.h"
#include "mod/info.h"
#include "defines.h"

namespace klk
{
    namespace file
    {
        /**
           @brief File info container

           File info container

           @ingroup grFile
        */
        class FileInfo : public mod::Info
        {
        public:
            /**
               Constructor

               @param[in] uuid - the file uuid
               @param[in] name - the name
               @param[in] path - the path
               @param[in] factory - the factory instance
            */
            FileInfo(const std::string& uuid,
                     const std::string& name,
                     const std::string& path,
                     IFactory* factory);

            /**
               Destructor
            */
            virtual ~FileInfo(){}

            /**
               Retrives the file path

               @return the path
            */
            const std::string getPath() const throw()
            {
                Locker lock(&m_lock);
                return m_path;
            }

            /**
               Retrives the file info type

               @return the type

               @note FIXME!!! used in utest only
            */
            virtual const std::string getType() const throw()
            {
                return  type::REGULAR;
            }

            /**
               Updates the module info

               @param[in] value - the data

               @exception klk::Exception
            */
            virtual void updateInfo(const mod::InfoPtr& value);
        protected:
            /**
               Retrives factory instance

               @return factory
            */
            IFactory * getFactory() const
            {
                return m_factory;
            }

            /**
               @copydoc klk::mod::Info::fillOutMessage
            */
            virtual void fillOutMessage(const IMessagePtr& out) const;

            /**
               @copydoc klk::mod::Info::check
            */
            virtual void check(const IMessagePtr& in) const;
        private:
            std::string m_path; ///< file's path
            IFactory * const m_factory; ///< factory instance

            /**
               Checks file existance

               @return
               - true
               - false
            */
            bool exist() const;
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            FileInfo(const FileInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            FileInfo& operator=(const FileInfo& value);
        };

        /**
           Smart pointer for the file info
        */
        typedef boost::shared_ptr<FileInfo> FileInfoPtr;

        /**
           List with info
        */
        typedef std::list<FileInfoPtr> FileInfoList;

        /**
           @brief Folder info container

           Keeps info about a folder
        */
        class FolderInfo : public FileInfo
        {
        public:
            /**
               Constructor

               @param[in] uuid - the file uuid
               @param[in] name - the name
               @param[in] path - the path
               @param[in] factory - the factory instance
            */
            FolderInfo(const std::string& uuid,
                       const std::string& name,
                       const std::string& path,
                       IFactory* factory);

            /**
               Destructor
            */
            virtual ~FolderInfo(){}
        private:
            /**<
               There is a uuid for file that was last processed at the folder
            */
            mutable std::string m_file_uuid;

            /**
               @copydoc klk::mod::Info::fillOutMessage
            */
            virtual void fillOutMessage(const IMessagePtr& out) const;

            /**
               @copydoc klk::mod::Info::check
            */
            virtual void check(const IMessagePtr& in) const;

            /**
               @copydoc klk::file::FileInfo::getType()
            */
            virtual const std::string getType() const throw()
            {
                return  type::FOLDER;
            }

            /**
               Sets file uuid

               @param[in] uuid - the uuid to be set

               @exception klk::Exception

               @note FIXME!!! remove const modifier from it
            */
            void setFileUUID(const std::string& uuid) const;

            /**
               Retrives uuid of file that was last processed

               @return the uuid
            */
            const std::string getFileUUID() const
            {
                Locker lock(&m_lock);
                return m_file_uuid;
            }
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            FolderInfo(const FolderInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            FolderInfo& operator=(const FolderInfo& value);
        };
    }
}

#endif //KLK_FILEINFO_H
