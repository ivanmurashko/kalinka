/**
   @file scanchannel.h
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
   - 2009/07/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SCANCHANNEL_H
#define KLK_SCANCHANNEL_H

#include <string>
#include <list>

#include "iresources.h"
#include "thread.h"
#include "db.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               Vector with PIDs

               @ingroup grDVBScan
            */
            typedef std::list<uint> PidsList;

            /**
               @brief Keeps info about scanned channel

               Keeps info about scanned channel
            */
            class ScanChannel
            {
            public:
                /**
                   Constructor

                   @param[in] channel_no - the channel number
                   @param[in] dev - the tunned device
                */
                explicit ScanChannel(uint channel_no, const IDevPtr& dev);

                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                ScanChannel(const ScanChannel& value);

                /**
                   Destructor
                */
                virtual ~ScanChannel(){}

                /**
                   Retrives channel number
                */
                const uint getChannelNumber() const throw() {return m_channel_no;}

                /**
                   Sets channel name

                   @param[in] name - the channel name to be set
                */
                void setChannelName(const std::string& name);

                /**
                   Sets provider name

                   @param[in] name - the provider name to be set
                */
                void setProviderName(const std::string& name);

                /**
                   Sets scrimbling

                   @param[in] value - the value to be set
                */
                void setScrambling(bool value){m_scrambling = value;}

                /**
                   Adds an audio/video pid

                   @param[in] pid - the pid to be added
                   @param[in] type - the pid's type

                   Possible values for the type argument are
                   - klk::dvb::PID_VIDEO
                   - klk::dvb::PID_AUDIO
                */
                void addPid(uint pid, const std::string& type);

                /**
                   Saves the channel info to DB

                   @param[in] db - the pointer to db that has to be used

                   @exception  klk::Exception
                */
                void save2DB(db::DB& db);
            private:
                uint m_channel_no; ///< channel number
                std::string m_name; ///< channel name
                std::string m_provider; ///< provider
                PidsList m_video_pid; ///< video pids container
                PidsList m_audio_pid; ///< audio pids container
                bool m_scrambling; ///< scrambling
                IDevPtr m_dev; ///< device info

                /**
                   Saves a pid

                   @param[in] db - the pointer to db that has to be used
                   @param[in] channel - the channel uuid
                   @param[in] type - the pid's type
                   @param[in] value - the pids value

                   @exception klk::Exception
                */
                void savePid2DB(db::DB& db,
                                const std::string& channel,
                                const std::string& type,
                                uint value);

                /**
                   Clears pids for specified channel

                   @param[in] db - the pointer to db that has to be used
                   @param[in] uuid - the channel uuid

                   @exception klk::Exception
                */
                void delPids(db::DB& db, const std::string& uuid);
            private:
                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                ScanChannel& operator=(const ScanChannel& value);
            };

            /**
               List with scan channel data

               @ingroup grDVBScan
            */
            typedef std::list<ScanChannel> ScanChannelList;
        }
    }
}


#endif //KLK_SCANCHANNEL_H
