/**
   @file flvbranchfactory.h
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
   - 2009/11/02 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_FLVBRANCHFACTORY_H
#define KLK_FLVBRANCHFACTORY_H

#include "basebranchfactory.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief FLV barnch creation factory

           FLV barnch creation factory

           As well as gstreamer have only one flv muxer ffmux_flv that
           supports only mp3 as audio codec for flv and only one
           type of video encoder: ffenc_flv we are limited in the flv support.

           There is an example of our encode gst-launch command
           @verbatim
           gst-launch filesrc location=/Video/Borat.avi ! queue ! decodebin name=d \
           ffmux_flv name=mux ! tcpclientsink host=127.0.01 port=20000 d.  ! ffmpegcolorspace ! "video/x-raw-yuv",format=\(fourcc\)I420 ! deinterlace ! videoscale ! "video/x-raw-yuv",width=320,height=240 ! ffenc_flv bitrate=600000 ! identity sync=TRUE ! mux. \
          d. ! queue ! audioconvert ! audioresample ! audio/x-raw-int,rate=44100 ! lame3 bitrate=64000 ! mp3parse ! mux.
           @endverbatim

           @see klk::media::FLV

           @ingroup grTrans
        */
        class FLVBranchFactory : public BaseBranchFactory,
            public IFactoryMaker
        {
        public:
            /**
               Constructor

               @param[in] factory - the main factory object
               @param[in] pipeline - the pipeline for callbacks
            */
            FLVBranchFactory(IFactory* factory, IPipeline* pipeline);

            /**
               Destructor
            */
            virtual ~FLVBranchFactory();
        private:
            GstElement* m_flvmux; ///< flv muxer to create pads

            /// @copydoc klk::trans::BaseBranchFactory::createVideoEncoder
            virtual GstElement * createVideoEncoder(const std::string& quality) const;

            /// @copydoc klk::trans::BaseBranchFactory::createAudioBin
            virtual GstElement*  createAudioBin();

            /// @copydoc klk::trans::BaseBranchFactory::getMuxElement
            virtual GstElement* getMuxElement();

            /// @copydoc klk::trans::BaseBranchFactory::getAudioMuxPad
            virtual GstPad * getAudioMuxPad();

            /// @copydoc klk::trans::BaseBranchFactory::getVideoMuxPad
            virtual GstPad * getVideoMuxPad();

            /// @copydoc klk::trans::IFactoryMaker::makeFactory
            virtual const IBranchFactoryPtr makeFactory() const
            {
                return IBranchFactoryPtr(new FLVBranchFactory(getFactory(), getPipeline()));
            }
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            FLVBranchFactory(const FLVBranchFactory& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            FLVBranchFactory& operator=(const FLVBranchFactory& value);
        };
    }
}

#endif //KLK_FLVBRANCHFACTORY_H
