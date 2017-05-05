/**
   @file appmodule.h
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
   - 2009/04/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_APPMODULE_H
#define KLK_APPMODULE_H

#include "moduledb.h"
#include "thread.h"

namespace klk
{

    namespace app
    {
        /** @defgroup grApp Mediaserver applications

            @ref grModules "Mediaserver modules" can not be
            considered as a finished solution. To provide a
            complite functionality we should combine several modules.
            The combination is called as application

            @note FIXME!!! in future application should be start
            as a separate process
            to provide more robust solution. See also ticket #191

            @{
        */

        /**
           @brief The application module

           The module implements the base application functionlity

           @ingroup grCommon
        */
        class Module : public ModuleWithDB
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory object
               @param[in] modid - the module id
               @param[in] setmsgid - message id for set CLI command
               @param[in] showmsgid - message id for show CLI command
            */
            Module(IFactory* factory,
                   const std::string& modid,
                   const std::string& setmsgid,
                   const std::string& showmsgid);

            /// Destructor
            virtual ~Module(){}

            /**
               Retrives application uuid

               @return the uuid
            */
            const std::string getAppUUID();
        protected:
            /// @copydoc klk::IModule::registerProcessors
            virtual void registerProcessors();
        private:
            Mutex m_appuuid_mutex; ///< mutex for application uuid retrivial
            std::string m_appuuid; ///< application uuid
            const std::string m_setmsgid; ///< set cli message id
            const std::string m_showmsgid; ///< show cli message id

            /// @copydoc klk::IModule::getType
            virtual mod::Type getType() const throw() {return mod::APP;}
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Module(const Module& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Module& operator=(const Module& value);
        };

        /** @} */
    }
}

#endif //KLK_APPMODULE_H
