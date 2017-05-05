/**
   @file file.h
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
   - 2009/03/23 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_FILE_H
#define KLK_FILE_H

#include "modulewithinfo.h"
#include "fileinfo.h"

namespace klk
{
    namespace file
    {
        /** @defgroup grFile File module

            The module operate with files
            @ingroup grModule
            @{
        */
        /**
           @brief File module

           File module
        */
        class File : public klk::ModuleWithInfo<FileInfo>
        {
            friend class TestFile;
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            explicit File(IFactory *factory);

            /**
               Destructor
            */
            virtual ~File();
        private:
            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Starts a file usage

               @param[in] in - the input message
               @param[out] out - the output message

               @exception  klk::Exception
            */
            virtual void doStart(const IMessagePtr& in,
                                 const IMessagePtr& out);

            /**
               Retrives an info about a file

               @param[in] in - the input message
               @param[out] out - the output message

               @exception  klk::Exception
            */
            void doInfo(const IMessagePtr& in, const IMessagePtr& out);

            /**
               Retrives an set with data from @ref grDB "database"

               @return set
            */
            virtual const InfoSet getInfoFromDB();

            /**
               Register all processors

               @exception klk::Exception
            */
            virtual void registerProcessors();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            File(const File& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            File& operator=(const File& value);
        };
        /** @} */
    }
}

#endif //KLK_FILE_H
