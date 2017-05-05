/**
   @file converter.h
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
   - 2010/04/27 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CONVERTER_H
#define KLK_CONVERTER_H

#include "ifactory.h"
#include "iproxy.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief Messages converter

           The class converts Ice internal data (SMessage) from/to
           Kalinka's klk::IMessage interface

           @ingroup grAdapterModule
        */
        class Converter
        {
        public:
            /**
               Constructor

               @param[in] factory - the message factory
            */
            Converter(IFactory* factory);

            /**
               Destructor
            */
            ~Converter(){}

            /**
               Ice -> klk::IMessage converter

               @param[in] msg - the message to be converted

               @return pointer to the klk::IMessage
            */
            const IMessagePtr ice2msg(const ipc::SMessage& msg) const;

            /**
               klk::IMessage -> Ice converter

               @param[in] msg - the message to be converted

               @return Ice internal container with the message data
            */
            const ipc::SMessage msg2ice(const IMessagePtr& msg) const;
        private:
            IFactory * const m_factory; ///< the factory
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Converter& operator=(const Converter& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Converter(const Converter& value);
        };
    }
}

#endif //KLK_CONVERTER_H
