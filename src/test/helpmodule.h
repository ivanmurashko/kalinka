/**
   @file helpmodule.h
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
   - 2009/12/06 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_HELPMODULE_H
#define KLK_HELPMODULE_H

#include "module.h"
#include "maintestdefines.h"

namespace klk
{
    namespace test
    {

        /** @defgroup grTestHelpModule Help module for utests
            @brief Helper module defenitions

            There are objects related to help module for unit testing

            @ingroup grTest

            @{
        */
        /**
           @brief Test helpmodule id

           test helpmodule id
        */
        const std::string HELP_MOD_ID = test::HELPPREFIX + "helper";


        /**
           @brief Message id to initate the processing

           There is an id for async message that initiates a unit test
        */
        const std::string HELP_MSG_ID = "test_message_helper";

        /**
           @brief The help module for utests

           The module is used as a test module for different  unit tests
           when we need to catch up  and process a sync message

           @igroup grTest
        */
        class HelpModule : public Module
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory object
            */
            HelpModule(IFactory* factory);

            /**
               Destructor
            */
            virtual ~HelpModule(){}

            /**
               Gets the result of the test

               @return
               - klk::OK - the test was OK
               - klk::ERROR - the test failed
            */
            klk::Result getResult() const throw() {return m_rc;}
        protected:
            klk::Result m_rc; ///< the result storage

            /**
               Do the test

               @exception klk::Exception in the case of an error
            */
            virtual void doTest() = 0;
        private:
            /**
               Internal main function for test

               @param[in] msg - the message recieved
            */
            void doTestInternal(const IMessagePtr& msg) throw();

            /**
               @copydoc IModule::registerProcessors
            */
            virtual void registerProcessors();

            /**
               @copydoc IModule::getName
            */
            virtual const std::string getName() const throw()
            {
                return "helper module for utests";
            }

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            HelpModule(const HelpModule& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            HelpModule& operator=(const HelpModule& value);
        };

        /** @} */

    }
}

#endif //KLK_HELPMODULE_H
