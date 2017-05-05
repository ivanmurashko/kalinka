/**
   @file dvb.h
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
   - 2008/09/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_DVB_H
#define KLK_DVB_H

#include "moduledb.h"
#include "thread.h"
#include "processor.h"

namespace klk
{
    namespace dvb
    {
        /** @defgroup grDVB DVB module

            Incapsulate DVB functionality. Manages DVB resources
            (DVB cards and DVB channels).

            @ingroup grModule

            @{
        */

        /**
           @brief The DVB module class implementation

           The DVB module class implementation

           @ingroup grDVB
        */
        class DVB : public klk::ModuleWithDB
        {
            friend class TestDVB;
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            DVB(IFactory *factory);

            /**
               Destructor
            */
            virtual ~DVB();

            /**
               Retrives processor
            */
            const ProcessorPtr getProcessor() const throw()
            {return m_processor;}
        private:
            const ProcessorPtr m_processor; ///< dvb processor
        private:
            /**
               Checks DVB devs state

               @exception @ref klk::Exception
            */
            void checkDVBDevs();

            /**
               Checks a DVB dev state

               @param[in] dev - the dev to be checked

               @exception @ref klk::Exception
            */
            void checkDVBDev(const IDevPtr& dev);

            /**
               Process changes at the DB

               @param[in] msg - the input message

               @exception @ref klk::Exception
            */
            virtual void processDB(const IMessagePtr& msg);

            /**
               Register all processors

               @exception @ref klk::Exception
            */
            virtual void registerProcessors();

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               Do some actions before main loop

               @exception @ref klk::Exception
            */
            virtual void preMainLoop();

            /**
               Do some actions after main loop
            */
            virtual void postMainLoop() throw();

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Processes SNMP request

               @param[in] req - the request

               @return the response
            */
            const snmp::IDataPtr processSNMP(const snmp::IDataPtr& req);
        private:
            /**
               Copy constructor
            */
            DVB(const DVB&);

            /**
               Assignment opearator
            */
            DVB& operator=(const DVB&);
        };

        /** @} */

    }
}

#endif //KLK_DVB_H

