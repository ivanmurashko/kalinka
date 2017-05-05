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
   - 2008/03/25 created by ipp (Ivan Murashko)
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
    namespace file
    {
        /** @addtogroup grFileTest
            @{
        */
        /**
           @brief Test helpmodule id

           test helpmodule id
        */
        const std::string TEST_MOD_ID = test::HELPPREFIX + "file";


        /**
           @brief Test helpmodule id

           test helpmodule id
        */
        const std::string TEST_MSG_ID = "test_message_file";

        /**
           The class establish info about a file.Test
           The main info is the file path. There is also info about file creation
           and stop usage time
        */
        class TestInfo
        {
        public:
            /**
               Constructor
               @param[in] path - the file path
               @param[in] start - the start time
               @param[in] stop - the stop time
            */
        TestInfo(const std::string& path, time_t start, time_t stop) :
            m_path(path), m_start(start), m_stop(stop) { }

            /**
               Destructor
            */
            ~TestInfo(){}

            /**
               @return path
            */
            const std::string getPath() const
            {
                return m_path;
            }

            /**
               @return start time
            */
            time_t getStart() const
            {
                return m_start;
            }

            /**
               @return stop time
            */
            time_t getStop() const
            {
                return m_stop;
            }

            /**
               Compare operator with string

               @param[in] value - the value to be compared
            */
            bool operator==(const std::string& value) const
            {
                return (value == m_path);
            }
        private:
            std::string m_path; ///< path
            time_t m_start; ///< start time
            time_t m_stop; ///< stop time
        };

        /**
           @brief Test helpmodule for messaging core tests

           Test helpmodule for messaging core tests
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
               Gets the result
            */
            klk::Result getResult() const throw() {return m_rc;}
        private:
            klk::Result m_rc; ///< result

            /**
               List with file test info
            */
            typedef std::list<TestInfo> TestInfoList;

            /**
               Gets a human readable helpmodule name

               @return the name
            */
            virtual const std::string getName() const throw()
            {return "filetest";}

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Do the test
            */
            void doTest();

            /**
               Do the cleanup test
            */
            void doTestCleanup();

            /**
               Do the folder test
            */
            void doTestFolder();

            /**
               Process an async message

               @param[in] msg the message to be processed

               @exception @ref klk::Exception
            */
            virtual void processASync(const IMessagePtr& msg);

            /// @copydoc klk::IModule::registerProcessors
            virtual void registerProcessors();

            /**
               Retrives a list with file info (paths, creation times etc.) for the test folder

               @param[in] uuid - the parent file entry uuid

               The uuid of test folder is set into uuid
               There is a parent klk_file entry. The result keeps all
               children paths and other file info for the entry as the parent.

               @return the list with file info
            */
            const TestInfoList getChildren(const std::string& uuid);
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

        /** @} */
    }
}

#endif //KLK_TESTHELPMODULE_H
