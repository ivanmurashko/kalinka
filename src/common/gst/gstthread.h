/**
   @file gstthread.h
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
   - 2009/09/27 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_GSTTHREAD_H
#define KLK_GSTTHREAD_H

#include <gst/gst.h>

#include <boost/shared_ptr.hpp>

#include "ithread.h"
#include "iprocessor.h"
#include "thread.h"

namespace klk
{
    namespace gst
    {
        /** @addtogroup grGST

            GST thread related classes

            @{
        */

        /**
           @brief Smart pointer for GstElement

           There is a smart pointer for GstElement instance
        */
        class Element
        {
        public:
            /**
               Constructor

               @param[in] element - the element to hold
            */
            explicit Element(GstElement* element);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Element(const Element& value);

            /**
               Destructor
            */
            ~Element();

            /**
               Retrives the element itself

               @return the element
            */
            GstElement* getElement(){return m_element;}
        private:
            GstElement* m_element; ///< element to hold
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Element& operator=(const Element& value);
        };

        /**
           @brief GST thread implementation

           GST thread implementation
        */
        class Thread : public IThread
        {
        public:
            /**
               Constructor

               @param[in] timeout - the timeout. 0 means without timeout
            */
            explicit Thread(const u_long timeout);

            /**
               Constructor

               @param[in] processor - the processor
               @param[in] uuid - the uuid to be assigned
            */
            Thread(const IProcessorPtr& processor, const std::string& uuid);

            /**
               Destructor
            */
            virtual ~Thread();

            /**
               Call bus callback

               @param[in] msg - the message was gotten

               @exception klk::Exception in the case of a critical error

               @return
               - TRUE - we should continue to use the bus watch
               (added with gst_bus_add_watch ())
               - FALSE - we should stop use the bus watch
               (added with gst_bus_add_watch ())

               @note If we want to be notified again the next time there is a message
               on the bus, we should return TRUE (FALSE means we want to stop watching
               for messages on the bus and our callback should not be called again)
            */
            virtual gboolean callBus(GstMessage* msg);

            /**
               Stops the thread
            */
            virtual void stop() throw();

            /**
               Retrives UUID

               @return the thread's uuid
            */
            const std::string getUUID() const {return m_uuid;}
        protected:
            IProcessorPtr m_processor; ///< processor

            /**
               Retrives pipeline
               @note thread safe method

               @return the pipeline instance in the klk::gst::Element wrapper

               @exception klk::Exception in the case of the pipeline is not initialized
            */
            Element getPipeline();

            /**
               @copydoc klk::IThread::init
            */
            virtual void init();

            /**
               @copydoc klk::IThread::start
            */
            virtual void start();

            /**
               Do cleanup
            */
            virtual void clean() throw();

            /**
               Puts an event for processing

               @param[in] type - the event's type
            */
            void addEvent(EventType type);

            /**
               Starts playing the pipeline

               @exception klk::Exception - there was an error
            */
            void playPipeline();

            /**
               Starts pauses the pipeline

               @exception klk::Exception - there was an error
            */
            void pausePipeline();

            /**
               Process stage event
            */
            virtual void processStageChange(){}
        private:
            GMainLoop* m_loop; ///< main loop
            GstElement* m_pipeline; ///< pipeline
            const u_long m_timeout; ///< timeout
            const std::string m_uuid; ///< thread's UUID
            guint m_bus_id; ///< source id for GST bus

            mutable Mutex m_bus_lock; ///< locker for m_bus_id
            mutable Mutex m_pipeline_lock; ///< locker for m_pipeline
            mutable Mutex m_loop_lock; ///< locker for m_loop


            /**
               Check if the thread stopped

               @return
               - true - the thread has been stopped
               - false - the thread is still active

               @exception klk::Exception - there was an error during check
            */
            virtual bool isStopped();

            /**
               Retrives main loop
               @note thread safe method

               @return the main loop instance
            */
            GMainLoop* getLoop();

            /**
               Stops the loop
            */
            void stopLoop();

            /**
               @brief Quit callback

               There is a g_main_loop callback that is called to quit
               from the loop

               @param[in] data - the callback data (this pointer)

               @return
               - FALSE (the callback should be called only one time)
            */
            static gboolean quitCallback(gpointer data);

            /**
               Sets the specified state for  pipeline (klk::gst::Thread::m_pipeline)

               @param[in] state - the state to be set

               @exception klk::Exception if there was an error
            */
            void setState(GstState state);

            /**
               Stops processing events at GST bus

               @note thread safe function
            */
            void freeBus();

            /**
               Free pipeline resource
            */
            void freePipeline();

            /**
               Free loop resource
            */
            void freeLoop();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Thread(const Thread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Thread& operator=(const Thread& value);
        };

        /** @} */
    }
}


#endif //KLK_GSTTHREAD_H
