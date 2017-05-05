/**
   @file moduledb.h
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

#ifndef KLK_MODULEWITHDB_H
#define KLK_MODULEWITHDB_H

#include "module.h"

namespace klk
{
    /**
       @brief Module with DB connection support

       Module with DB connection support

       @ingroup grModule
    */
    class ModuleWithDB : public Module
    {
    public:
        /**
           Constructor

           @param[in] factory the factory for getting all necessary objects
           @param[in] id the module id
        */
        ModuleWithDB(IFactory *factory, const std::string& id);

        /**
           Destructor
        */
        virtual ~ModuleWithDB();
    protected:
        /// @copydoc klk::Module::init
        virtual void init();

        /**
           Register all processors

           It register DB related staff

           @exception klk::Exception
        */
        virtual void registerProcessors();

        /**
           Register a message that is used for DB update.

           There is a sync message that starts DB change update procedure
           and sends the response only when the update is finished

           @param[in] msgid - the message to be register
        */
        void registerDBUpdateMessage(const std::string&  msgid);

        /**
           Each db related module should process
           klk::msg::id::CHANGEDB message

           The method will be called for the procesing

           @param[in] msg - the input message

           @exception @ref klk::Exception
        */
        virtual void processDB(const IMessagePtr& msg) = 0;

        /**
           Do some actions before main loop

           @exception @ref klk::Exception
        */
        virtual void preMainLoop();
    private:
        /**
           The processor for DB update message
           @see registerDBUpdateMessage

           @param[in] in - the input message
           @param[out] out - the output message
        */
        void processDBUpdate(const IMessagePtr& in, const IMessagePtr& out);

        /**
           Reads first data portion from DB

           @param[in] in - the input message
        */
        void initFromDB(const IMessagePtr& in);
    };
};

#endif //KLK_MODULEWITHDB_H
