/**
   @file dvbthreadinfo.h
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
   - 2009/06/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_DVBTHREADINFO_H
#define KLK_DVBTHREADINFO_H

#include "ithreadfactory.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /**
               @brief Basic process thread implementation

               Basic process thread implementation
            */
            class ThreadInfo : public IThreadInfo
            {
            public:
                /**
                   Constructor

                   @param[in] thread - the thread
                   @param[in] dev - the dev
                */
                ThreadInfo(const IThreadPtr& thread, const IDevPtr& dev);

                /**
                   Constructor

                   @param[in] thread - the thread
                   @param[in] dev - the dev
                   @param[in] stream - the stream
                */
                ThreadInfo(const IThreadPtr& thread, const IDevPtr& dev,
                           const IStreamPtr& stream);

                /**
                   Destructor
                */
                virtual ~ThreadInfo();
            private:
                IThreadPtr m_thread; ///< thread it self
                IDevPtr m_dev; ///< the dev
                IStreamPtr m_stream; ///< stream info

                /**
                   Checks is the thread correspond to the specified
                   device or not

                   @param[in] dev - the dev to be checked

                   @return
                   - true
                   - false
                */
                virtual const bool matchDev(const IDevPtr& dev) const throw();

                /**
                   Retrives IThread pointer

                   @return the thread pointer
                */
                virtual const IThreadPtr getThread() const throw();

                /**
                   Retrives IStream pointer

                   @return the stream pointer

                   @exception klk::Exception
                */
                virtual const IStreamPtr getStream() const;
            };
        }
    }
}

#endif //KLK_DVBTHREADINFO_H
