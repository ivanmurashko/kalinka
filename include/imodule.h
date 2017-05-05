/**
   @file imodule.h
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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_IMODULE_H
#define KLK_IMODULE_H

#include <sys/types.h>
#include <time.h>

#include <list>

#include <boost/shared_ptr.hpp>

#include "stringlist.h"

#include "imessage.h"
#include "icli.h"

namespace klk
{
    namespace mod
    {

        /** @defgroup grModule Mediaserver modules

            The mediaserver design consist of several modules.
            Each module does a specific work.

            @{
        */

        /**
           Module type
        */
        typedef enum
        {
            MOD = 1,
            APP = 2
        } Type;

        /**
           Unknown module id
        */
        const std::string UNKNOWN = "484ea667-9658-452d-9e3e-85f478083226";
    };

    /**
       @brief Module interface

       Module interface
    */
    class IModule
    {
    public:
        /**
           Destructor
        */
        virtual ~IModule(){}

        /**
           Adds a message for processing

           @param[in] msg the pointer to the message that has to be added

           @exception @ref klk::Exception
        */
        virtual void addMessage(const IMessagePtr& msg) = 0;

        /**
           Gets the module's id

           @return the id or NULL if there was an error
        */
        virtual const std::string getID() const throw() = 0;

        /**
           Sends a synchronous message (need for a response)
           for processing

           @see klk::IModuleFactory::sendMessage()

           @param[in] in - the pointer to the message that has to be sent
           @param[out] out - the output message

           @exception @ref klk::Exception
        */
        virtual void sendSyncMessage(const IMessagePtr& in,
                                     IMessagePtr& out) = 0;

        /**
           Gets a human readable module name

           @return the name
        */
        virtual const std::string getName() const throw() = 0;

        /**
           Gets info for CLI

           @return the list with known CLI commands

           @exception @ref Result
        */
        virtual const cli::ICommandList getCLIInfo() const throw()= 0;

        /**
           Register all processors

           @exception klk::Exception
        */
        virtual void registerProcessors() = 0;

        /**
           Retrive the type of the module
           is there standard module or an application

           @return @ref mod::Type
        */
        virtual mod::Type getType() const throw() = 0;

        /**
           Retrive the module's version

           @return the version
        */
        virtual const std::string getVersion() const throw() = 0;

        /**
           Gets uptime in seconds

           @return the uptime
        */
        virtual const time_t getUptime() const = 0;

        /**
           Retrive CPU usage info in the following interval
           (0, 1)

           @return the CPU usage
        */
        virtual const double getCPUUsage() const = 0;

        /**
           @brief Waits the module start

           Waits until the module will finish its startup procedure

           @param[in] timeout - the time to wait in seconds

           @exception klk::Exception if there was an error
           or the timeout exceed
         */
        virtual void waitStart(time_t timeout) = 0;

        /**
           @brief Checks module initialization

           Is the module pass throught all checkpoints during it startup (startup was finished)
           or not and is in progress

           @return
           - true all initialization checkpoints were pass
           - false - there is at lest one checkpoint has not been passed yet

           @see klk::IModule::waitStart
        */
        virtual bool isStarted() const = 0;
    };

    /**
       Module smart pointer
    */
    typedef boost::shared_ptr<IModule> IModulePtr;

    /**
       Modules list
    */
    typedef std::list<IModulePtr> ModuleList;

    /** @} */

};



#endif //KLK_IMODULE_H
