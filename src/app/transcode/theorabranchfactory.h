/**
   @file theorabranchfactory.h
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
   - 2010/03/05 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_THEORABRANCHFACTORY_H
#define KLK_THEORABRANCHFACTORY_H

#include "basebranchfactory.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief Theora barnch creation factory

           Theora barnch creation factory

           There is an example of our encode gst-launch command
           @verbatim
           gst-launch filesrc location=~/fun/video/public/Borat.avi ! queue ! decodebin name=d oggmux name=mux ! tcpclientsink host=192.168.0.100 port=20000 d.  ! ffmpegcolorspace ! "video/x-raw-yuv",format=\(fourcc\)I420 ! videoscale ! "video/x-raw-yuv",width=320,height=240 ! theoraenc ! identity sync=TRUE ! mux. d. ! queue ! audioconvert ! vorbisenc ! queue ! mux.
          @endverbatim

           @see klk::media::THEORA

           @ingroup grTrans
        */
        class TheoraBranchFactory : public BaseBranchFactory,
            public IFactoryMaker
        {
        public:
            /**
               Constructor

               @param[in] factory - the main factory object
               @param[in] pipeline - the pipeline for callbacks
            */
            TheoraBranchFactory(IFactory* factory, IPipeline* pipeline);

            /**
               Destructor
            */
            virtual ~TheoraBranchFactory();
        private:
            GstElement* m_theoramux; ///< flv muxer to create pads

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
                return IBranchFactoryPtr(new TheoraBranchFactory(getFactory(), getPipeline()));
            }
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TheoraBranchFactory(const TheoraBranchFactory& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TheoraBranchFactory& operator=(const TheoraBranchFactory& value);
        };
    }
}

#endif //KLK_THEORABRANCHFACTORY_H
