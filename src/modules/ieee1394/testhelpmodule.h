/**
   @file testhelpmodule.h
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
   - 2008/03/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTHELPMODULE_H
#define KLK_TESTHELPMODULE_H


#include "module.h"
#include "maintestdefines.h"

namespace klk
{
    namespace fw
    {
        /**
           @brief Test helpmodule id

           test helpmodule id

           @ingroup grTest
        */
        const std::string TEST_MOD_ID = test::HELPPREFIX + "ieee1394";


        /**
           @brief Test helpmodule for messaging core tests

           Test helpmodule for messaging core tests

           @ingroup grTest
        */
        class TestHelpModule : public klk::Module
        {
        public:
            /**
               Constructor
            */
            TestHelpModule(IFactory *factory);

            /**
               Destructor
            */
            virtual ~TestHelpModule(){}

            /**
               Clears the state
            */
            void clear();

            /**
               How many times 1st cam was appeared
            */
            u_int getAppearCount1(){return m_appear1;}

            /**
               How many times 1st cam was disappeared
            */
            u_int getDisappearCount1(){return m_disappear1;}

            /**
               How many times 2d cam was appeared
            */
            u_int getAppearCount2(){return m_appear2;}

            /**
               How many times 2d cam was disappeared
            */
            u_int getDisappearCount2(){return m_disappear2;}
        private:
            u_int m_appear1; ///< how many times 1st cam appeared
            u_int m_disappear1; ///< how many times 1st cam disappeared
            u_int m_appear2; ///< how many times 2d cam appeared
            u_int m_disappear2; ///< how many times 2d cam disappeared

            /**
               Gets a human readable helpmodule name

               @return the name
            */
            virtual const std::string getName() const throw()
            {return "ieee1394test";}

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Process an async message

               @param[in] msg the message to be processed

               @exception @ref klk::Exception
            */
            virtual void processASync(const IMessagePtr& msg);

            /**
               @copydoc klk::IModule::registerProcessors
            */
            virtual void registerProcessors();
        private:
            /**
               Copy constructor
            */
            TestHelpModule(const TestHelpModule&);

            /**
               Assignment opearator
            */
            TestHelpModule& operator=(const TestHelpModule&);

        };
    }
}

#endif //KLK_TESTHELPMODULE_H
