/**
   @file basebranchfactory.h
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

#ifndef KLK_BASEBRANCHFACTORY_H
#define KLK_BASEBRANCHFACTORY_H

#include "ifactory.h"
#include "ipipeline.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief The base class for all klk::trans::IBranchFactory

           The base class for all klk::trans::IBranchFactory

           @ingroup grTrans
        */
        class SimpleBaseBranchFactory : public IBranchFactory
        {
        public:
            /**
               Constructor
            */
            SimpleBaseBranchFactory(){}

            /**
               Destructor
            */
            virtual ~SimpleBaseBranchFactory(){}
        protected:
            /**
               Makes a queue gst element

               @exception klk::Exception if there was an error

               @return the queue element
            */
            static GstElement* makeQueue();
        private:
            /**
               Copy constructor
            */
            SimpleBaseBranchFactory(const SimpleBaseBranchFactory&);

            /**
               Assigment operator
            */
            SimpleBaseBranchFactory& operator=(const SimpleBaseBranchFactory&);
        };

        /**
           @brief The object is used for transcoding branches creation

           It analized the task and created GStreamer branch as GST_BIN
           element for it. The sink is accesable via sink ghost pad.

           @ingroup grTrans
        */
        class BaseBranchFactory : public SimpleBaseBranchFactory
        {
        public:
            /**
               Constructor

               @param[in] factory - the base factory
               @param[in] pipeline - the pipeline for callbacks
            */
            BaseBranchFactory(IFactory* const factory, IPipeline* const pipeline);

            /**
               Destructor
            */
            virtual ~BaseBranchFactory();
        protected:
            /**
               Creates video encoder element

               @param[in] quality - the video encoder quality

               @return the pointer to encoder plugin

               @note as well as the element was added to pipeline
               the memory allocated will be freed automatically
            */
            virtual GstElement* createVideoEncoder(const std::string& quality) const = 0;

            /**
               Creates audio bin container (GstElement) to encode audio stream

               @return the created element

               @note as well as the element was added to pipeline
               the memory allocated will be freed automatically
            */
            virtual GstElement*  createAudioBin() = 0;

            /**
               Get muxer element

               @return the smart pointer to the element

               @note as well as the element was added to pipeline
               the memory allocated will be freed automatically
            */
            virtual GstElement* getMuxElement() = 0;

            /**
               Retrives request pad for audio at the muxer

               @return the pad

               @note the result should be freed with gst_object_unref
             */
            virtual GstPad * getAudioMuxPad() = 0;

            /**
               Retrives request pad for video at the muxer

               @return the pad

               @note the result should be freed with gst_object_unref
             */
            virtual GstPad * getVideoMuxPad() = 0;

            /**
               @return pointer to factory
            */
            IFactory* const getFactory() const
            {
                return m_factory;
            }

            /**
               @return pointer to pipeline
            */
            IPipeline* const getPipeline() const
            {
                return m_pipeline;
            }

            /// Gst elements container
            typedef std::vector<GstElement*> GstElementVector;

            /**
               Get video scale elements based on video quality settings

               @return empty container if default size is used

               @exception klk::Exception if there was an error
            */
            const GstElementVector getVScale() const;
        private:
            IFactory* const m_factory; ///< main factory
            IPipeline* const m_pipeline; ///< pipeline

            GstElement* m_branch; ///< branch GST (GST_BIN) container
            GstElement* m_mux; ///< muxer bin

            quality::VideoPtr m_vquality; ///< video quality info

            /**
               Creates video sink

               @param[in] new_pad - the video pad for links

               @exception klk::Exception
            */
            void createVideoSink(GstPad *new_pad);

            /**
               Creates audio sink

               @param[in] new_pad - the audio pad for links

               @exception klk::Exception
            */
            void createAudioSink(GstPad *new_pad);

            /**
               Sets pipeline into the playing mode

               @exception klk::Exception
            */
            void playPipeline();

            /**
               Sets pipeline into the paused mode

               @exception klk::Exception
            */
            void pausePipeline();

            /**
               Creates the simple branch

               @param[in] task - the task info container

               @return the branch packed to a bin
            */
            virtual GstElement* createBranch(const TaskPtr& task);

            /**
               Creates muxer for FLV transcode
               It creates common stuff

               @param[in] sink - the sink element
            */
            void createMux(GstElement* sink);

            /**
               Creates muxer for FLV transcode
               It creates audio specific pad
            */
            void createMuxAudio();

            /**
               Creates muxer for FLV transcode
               It creates video specific pad
            */
            void createMuxVideo();

            /**
               Creates video bin container (GstElement) to encode video stream

               @return the created element

               @note as well as the element was added to pipeline
               the memory allocated will be freed automatically
            */
            GstElement*  createVideoBin() const;

            /**
               new-decoded-pad signal was recieved

               This signal gets emitted as soon as a new pad
               of the same type as one of the valid 'raw' types is added.

               @param[in] decodebin - The decodebin
               @param[in] new_pad  - The newly created pad
               @param[in] last  - TRUE if this is the last pad to be added. Deprecated.
               @param[in] user_data - user data set when the signal handler was connected.

               @note it should not throw any exception
             */
            static void onNewDecodedPad(GstElement *decodebin,
                                        GstPad     *new_pad,   gboolean    last,
                                        gpointer    user_data) throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            BaseBranchFactory(const BaseBranchFactory& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            BaseBranchFactory& operator=(const BaseBranchFactory& value);
        };
    }
}

#endif //KLK_BASEBRANCHFACTORY_H
