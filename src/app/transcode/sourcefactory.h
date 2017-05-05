/**
   @file sourcefactory.h
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
   - 2009/09/05 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SOURCEFACTORY_H
#define KLK_SOURCEFACTORY_H

#include "transinfo.h"
#include "db.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief The factory for getting sources

           The factory object is used for getting sources

           @ingroup grTrans
        */
        class SourceFactory
        {
        public:
            /**
               Constructor
            */
            SourceFactory();

            /**
               Destructor
            */
            virtual ~SourceFactory();

            /**
               Gets the source by its data

               The method looks at the already created sources firs
               if nothing found it will create a new object

               @param[in] uuid - the source's uuid
               @param[in] name - the source's name
               @param[in] media_type - the media's type
               (for example klk::media::MPEGTS)
               @param[in] type - the source's type
               (for example klk::trans::FileInfo)

               @return the source pointer
            */
            const SourceInfoPtr getSource(const std::string& uuid,
                                          const std::string& name,
                                          const std::string& media_type,
                                          const std::string& type);
        private:
            /**
               SourceInfo container
            */
            typedef std::list<SourceInfoPtr> SourceInfoContainer;

            Mutex m_lock; ///< mutex
            SourceInfoContainer m_container; ///< container
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SourceFactory(const SourceFactory& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SourceFactory& operator=(const SourceFactory& value);
        };
    }
}

#endif //KLK_SOURCEFACTORY_H
