/**
   @file moduleprocessor.h
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
   - 2009/05/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MODULEPROCESSOR_H
#define KLK_MODULEPROCESSOR_H

#include <list>

#include "moduleinfo.h"
#include "ifactory.h"

namespace klk
{
    namespace cli
    {
        /**
           @brief The class processes modules specific commands

           The class processes modules specific commands

           @ingroup grClient
        */
        class ModuleProcessor
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            ModuleProcessor(IFactory* factory);

            /// Destructor
            virtual ~ModuleProcessor(){}

            /**
               Processes a module's specific command

               @param[in] fd - the file descriptor for output messages
               @param[in] argc - TBD
               @param[in] argv - TBD
               @param[in] command_name - TBD
               @param[in] msgid - the command's message id
               @param[in] modnameused - TBD

               @exception klk::Exception
            */
            void handleModule(int fd, int argc, char *argv[],
                              const char* command_name,
                              const char* msgid, int modnameused);

            /**
               Processes a module's specific command
               (completion staff)

               @param[in] line - the line
               @param[in] word - the word
               @param[in] pos - the position index
               @param[in] msgid - the message id

               @return the vector with possible completions for the
               following position

               @exception @ref klk::Exception
            */
            const ParameterVector getCompletion(const std::string& line,
                                                const std::string& word,
                                                int pos, const char* msgid);

            /**
               Updates the modules info

               @exception klk::Exception
            */
            void update();
        private:
            /**
               The list for garbage
            */
            typedef std::list<ModuleInfoPtr> ModuleInfoList;

            IFactory* const m_factory; ///< factory object
            ModuleInfoList m_list; ///< module info list

            /**
               Retrives command by its message id

               @param[in] msgid - the message id

               @return the command

               @exception @ref klk::Exception if the command can not be found
            */
            const cli::ICommandPtr getCommandByMsgID(const std::string& msgid);

            /**
               Retrives the destination module id (main application module)
               the CLI command to be sent

               @param[in] msgid - the message id to be used to retrive the module
               info

               @return the module id

               @see klk::adapter::MessagesProtocol
            */
            const std::string getReceiverAppID(const std::string& msgid) const;
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ModuleProcessor& operator=(const ModuleProcessor& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ModuleProcessor(const ModuleProcessor& value);
        };
    }
}

#endif //KLK_MODULEPROCESSOR_H
