/**
   @file ieee1394.h
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
   - 2009/03/23 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_IEEE1394_H
#define KLK_IEEE1394_H

#include <boost/function/function0.hpp>

#include "modulewithinfo.h"
#include "dvinfo.h"
#include "modulescheduler.h"

namespace klk
{
    namespace fw
    {
        /** @defgroup grIEEE1394 IEEE1394 module

            The module operate with IEEE1394 device aka FireWire

            It periodically scans for new IEEE1394 hardware connected (mini DV cameras)
            and updates the DB: adds new cameras. Update period is set via
            klk::fw::CHECKINTERVAL interval

            It can also update listeners (another modules that want to know about IEEE1394
            devices state changes) via klk::msg::id::IEEE1394DEV message

            @ingroup grModule
            @{
        */

        /// The IEEE1394 specific info set
        typedef mod::InfoContainer<DVInfo>::InfoSet InfoSet;

        /**
           This functor is used for retriving
           DV camera information
        */
        typedef boost::function0<const InfoSet> UpdateFunction;

        /**
           @brief IEEE1394 module

           IEEE1394 module main instance
        */
        class FireWire : public ModuleWithInfo<DVInfo>
        {
            friend class TestFireWire;
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            FireWire(IFactory *factory);

            /**
               Destructor
            */
            virtual ~FireWire();
        private:
            /// the data info list
            typedef std::list< DVInfoPtr > InfoList;

            UpdateFunction m_update; ///< updater functor

            /**
               Register all processors

               @exception klk::Exception
            */
            virtual void registerProcessors();

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               Do the start

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            virtual void doStart(const IMessagePtr& in,
                                 const IMessagePtr& out);

            /**
               Retrives an list with data from @ref grDB "database"

               @return list
            */
            virtual const InfoSet getInfoFromDB();

            /// @copydoc klk::IModule::getType
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Checks cameras
            */
            void checkCameras();

            /**
               Adds an info about new camera to the DB

               @param[in] info - the info to be added
            */
            void addInfo2DB(const DVInfoPtr& info);

            /**
               Retrives info from db without any modifications

               @return the list with info
            */
            const InfoList getInfoFromDBWithoutMod();

            /**
               Retrives info about current available cameras

               @exception klk::Exception
            */
            static const InfoSet getDVInfoSet();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            FireWire(const FireWire& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            FireWire& operator=(const FireWire& value);
        };
        /** @} */
    }
}

#endif //KLK_IEEE1394_H
