/**
   @file dvinfo.h
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
   - 2009/03/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_DVINFO_H
#define KLK_DVINFO_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "thread.h"
#include "mod/info.h"

namespace klk
{
    namespace fw
    {
        /**
           @brief DV info container

           DV info container

           @ingroup grIEEE1394
        */
        class DVInfo : public mod::Info
        {
        public:
            /**
               Constructor

               @param[in] uuid - the file uuid
               @param[in] name - the name
               @param[in] description - the description
            */
            DVInfo(const std::string& uuid,
                   const std::string& name,
                   const std::string& description);

            /**
               Destructor
            */
            virtual ~DVInfo();

            /**
               Retrives the description

               @return the description
            */
            const std::string getDescription() const throw();

            /**
               Sets port

               @param[in] port - the value to be set

               @exception klk::Exception
            */
            void setPort(const int port);

            /**
               Gets port in the string form

               @return the string with port
            */
            const std::string getPort() const throw();

            /**
               Updates the module info

               @param[in] value - the data

               @exception klk::Exception
            */
            virtual void updateInfo(const mod::InfoPtr& value);
        private:
            std::string m_description; ///< description
            std::string m_port; ///< port value

            /**
               @copydoc klk::mod::Info::fillOutMessage
            */
            virtual void fillOutMessage(const IMessagePtr& out) const;
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            DVInfo(const DVInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            DVInfo& operator=(const DVInfo& value);
        };

        /**
           Smart pointer for the file info
        */
        typedef boost::shared_ptr<DVInfo> DVInfoPtr;

        /**
           List with info
        */
        typedef std::list<DVInfoPtr> DVInfoList;
    }
}


#endif //KLK_DVINFO_H
