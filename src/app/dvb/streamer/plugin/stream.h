/**
   @file stream.h
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
   - 2009/01/01 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_STREAM_H
#define KLK_STREAM_H

#include <list>

#include "thread.h"
#include "ithreadfactory.h"

#include "getstream2/getstream.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            namespace getstream2
            {
                /**
                   @brief The DVB thread that is used for streaming purposes

                   The DVB thread that is used for streaming purposes
                */
                class Stream : public IStream
                {
                public:
                    /**
                       Constructor

                       @param[in] dev - the dev with initial tune info

                    */
                    explicit Stream(const IDevPtr& dev);

                    /**
                       Destructor
                    */
                    virtual ~Stream();

                    /**
                       Updates DVB stat info
                    */
                    void updateInfo();

                    /**
                       Inits info timer
                    */
                    void initInfoTimer();

                    /**
                       Inits pipeline

                       @exception klk::Exception
                    */
                    void init() ;

                    /**
                       Starts processing
                    */
                    void start();

                    /**
                       Stops processing
                    */
                    void stop() throw();
                private:
                    const IDevPtr m_dev; ///< dev container with tune info
                    mutable klk::Mutex m_lock; ///< adapter lock mutex

                    struct adapter_s m_adapter; ///< adapter structure
                    struct event m_checkdevevent; ///< checkdev event

                    IStationList m_streams; ///< current streams
                    IStationList m_add_streams; ///< streams to be added
                    IStationList m_del_streams; ///< streams to be del

                    /**
                       Do cleanup
                    */
                    void clean() throw();

                    /**
                       @copydoc klk::dvb::stream::IStream::addStation
                    */
                    virtual void addStation(const IStationPtr& station);

                    /**
                       @copydoc klk::dvb::stream::IStream::delStation
                    */
                    virtual void delStation(const IStationPtr& station);

                    /**
                       @copydoc klk::dvb::stream::IStream::isStream
                    */
                    virtual const bool isStream() const;

                    /**
                       Frees resources allocated for stream info

                       @param[in] stream - the stream to be freed
                    */
                    static void freeStream(struct stream_s* stream);

                    /**
                       Inits adapter structure

                       @exception klk::Exception if there was any problem
                       with the init
                    */
                    void initAdapter();

                    /**
                       Checks DVB adapter state has it lock and signal

                       @exception klk::Exception if there was any problem
                       with the check
                    */
                    void checkAdapter();

                    /**
                       Tune deinit
                    */
                    void releaseInit();

                    /**
                       Inits a station

                       @param[in] station - the add station info
                    */
                    void initStation(const IStationPtr& station);

                    /**
                       Deinits a station

                       @param[in] station - the station to be freed
                    */
                    void
                        deinitStation(const IStationPtr& station);

                    /**
                       Finds getstream2 stream_s staruct by program number

                       @param[in] pnr - the program number

                       @return the pointer to the struct or NULL
                       if it was not found
                    */
                    struct stream_s* getStreamStruct(const u_int pnr);

                    /**
                       Updates rates for the specified channel

                       @param[in] station - the station to be updated
                    */
                    void updateRate(IStationPtr& station) throw();

                    /**
                       Retrives rate in bytes per second from stat info and
                       prepare (clear) the stat info for next collection

                       @param[in] stat - the stat info

                       @note the stat will be cleared (modified)

                       @return the rate

                       @exception klk::Exception
                    */
                    const int getRate(struct klkstat_s& stat) const;
                private:
                    /**
                       Copy constructor
                       @param[in] value - the copy param
                    */
                    Stream(const Stream& value);

                    /**
                       Assigment operator
                       @param[in] value - the copy param
                    */
                    Stream& operator=(const Stream& value);
                };

                /**
                   Smart poineter
                */
                typedef boost::shared_ptr<Stream> StreamPtr;
            }
        }
    }
}


#endif //KLK_STREAM_H
