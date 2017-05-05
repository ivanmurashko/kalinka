/**
   @file fwinfo.h
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
   - 2010/06/13 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_FWINFO_H
#define KLK_FWINFO_H

#include "transinfo.h"

namespace klk
{
    namespace trans
    {
        class Transcode;

        /**
           @brief IEEE1394 source info

           There is a source info about IEEE1394 device (aka firewire)

           The class is used as klk::trans::SourceInfoPtr instance at
           klk::trans::SourceFactory::getSource() method

           @ingroup grTrans
        */
        class FWSrcInfo : public SourceInfo
        {
        public:
            /// @copydoc klk::trans::SourceInfo
            FWSrcInfo(const std::string& uuid,  const std::string& name,
                      const std::string& media_type);

            /// Destructor
            virtual ~FWSrcInfo(){}
        private:
            SafeValue<Transcode*> m_module; ///< module instance to unlock the resource

            /**
               Inits the IEEE1394 element assosiated with the source

               @param[in] module - the klk::trans::Transcode module instance

               @exception klk::Exception
            */
            virtual void initInternal(Transcode* module);

            /**
               Deinits the IEEE1394 source element assosiated with the source

               @exception klk::Exception
            */
            virtual void deinit() throw();
        private:
            /// Fake copy constructor
            FWSrcInfo(const FWSrcInfo&);

            /// Fake assigment operator
            FWSrcInfo& operator=(const FWSrcInfo&);
        };
    }
}

#endif //KLK_FWINFO_H
