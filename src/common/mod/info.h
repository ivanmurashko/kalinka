/**
   @file info.h
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
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MOD_INFO_H
#define KLK_MOD_INFO_H

#include <string>
#include <list>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/assert.hpp>

#include "imessage.h"
#include "thread.h"
#include "stringlist.h"

namespace klk
{
    namespace mod
    {

        /** @defgroup grModuleInfo Module with info
            @{

            The data storage module class defenitions

            @ingroup grModule
        */

        /**
           Smart pointer for the module info
        */
        typedef boost::shared_ptr<class Info> InfoPtr;

        /**
           @brief Module specific info

           Module specific info
        */
        class Info
        {
        public:
            /**
               Constructor

               @param[in] uuid - the info uuid from DB
               @param[in] name - the info name from DB
            */
            Info(const std::string& uuid, const std::string& name);

            /**
               Destructor
            */
            virtual ~Info();

            /**
               Retrives the file uuid

               @return the uuid
            */
            const std::string getUUID() const throw();

            /**
               Retrives the file name

               @return the name
            */
            const std::string getName() const throw();

            /**
               Sets a name

               @param[in] name - the name to be set
            */
            void setName(const std::string& name);

            /**
               Tests if the info is used by somebody

               @return
               - true
               - false
            */
            virtual bool isInUse() throw();

            /**
               Sets in use flag

               @param[in] value - the value to be set
            */
            virtual void setInUse(bool value);

            /**
               Updates the module info

               @param[in] value - the data

               @exception klk::Exception
            */
            virtual void updateInfo(const InfoPtr& value);

            /**
               Dynamic pointer cast to a specific module info
            */
            template <class T> static const boost::shared_ptr<T>
                dynamicPointerCast(const InfoPtr& info)
            {
                const boost::shared_ptr<T> result =
                    boost::dynamic_pointer_cast<T, Info>(info);
                BOOST_ASSERT(result);
                return result;
            }

            /**
               Fills the output message with values from the info instance

               @param[in] out - the message to be filled

               @exception klk::Exception

               @note FIXME!!! make the method pure virtual
            */
            virtual void fillOutMessage(const IMessagePtr& out) const;

            /**
               Checks that info corresponds data stored at the info message

               @param[in] in - the input message with check data

               @exception klk::Exception if there was an error during the check
            */
            virtual void check(const IMessagePtr& in) const;
        protected:
            mutable Mutex m_lock; ///< locker
        private:
            const std::string m_uuid; ///< record uuid
            std::string m_name; ///< name
            bool m_inuse; ///< in use flag
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Info(const Info& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Info& operator=(const Info& value);
        };

        /// List with module info
        typedef std::list<InfoPtr> InfoList;

        /** @} */
    }
}

#endif //KLK_MOD_INFO_H
