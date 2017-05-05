/**
   @file gstthread.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstthread.h"
#include "exception.h"
#include "errors.h"

using namespace klk;
using namespace klk::gst;

//
// Element class
//

// Constructor
Element::Element(GstElement* element) : m_element(element)
{
    BOOST_ASSERT(m_element);
    gst_object_ref(m_element);
}

// Copy constructor
Element::Element(const Element& value) :
    m_element(value.m_element)
{
    // no need to check for NULL here
    KLKASSERT(m_element)
    gst_object_ref(m_element);
}


// Destructor
Element::~Element()
{
    gst_object_unref(m_element);
    m_element = NULL;
}

//
// Thread class
//

static gboolean
call_bus (GstBus     *bus,
	  GstMessage *msg,
	  gpointer    data)
{
    gst::Thread *thread = static_cast<gst::Thread*>(data);

    try
    {
        if (thread)
        {
            return thread->callBus(msg);
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,  "Exception in call_bus(). It will be terminated");
    }
    /* There was an error thus stop use it */
    klk_log(KLKLOG_ERROR,  "Error in call_bus(). It will be terminated");
    return FALSE;
}

// Constructor
Thread::Thread(const u_long timeout) :
    m_processor(),
    m_loop(NULL), m_pipeline(NULL),
    m_timeout(timeout),  m_uuid(),
    m_bus_lock(), m_pipeline_lock(), m_loop_lock()
{
}

// Constructor
Thread::Thread(const IProcessorPtr& processor, const std::string& uuid) :
    m_processor(processor),
    m_loop(NULL), m_pipeline(NULL),
    m_timeout(0),  m_uuid(uuid), m_bus_id(0),
    m_bus_lock(), m_pipeline_lock(), m_loop_lock()
{
    BOOST_ASSERT(m_processor);
    BOOST_ASSERT(m_uuid.empty() == false);
}


// Destructor
Thread::~Thread()
{
}

// @brief Quit callback
// There is a g_main_loop callback that is called to quit
// from the loop
/*static*/
gboolean Thread::quitCallback(gpointer data)
{
    gst::Thread *thread = static_cast<gst::Thread*>(data);

    if (thread)
    {
        thread->stopLoop();
    }
    else
    {
        KLKASSERT(0);
    }

    // Call only one time
    return FALSE;
}

//  Stops processing events at GST bus
void Thread::freeBus()
{
    Locker lock(&m_bus_lock);
    // stop processing events
    if (m_bus_id)
    {
        if (!g_source_remove(m_bus_id))
        {
            klk_log(KLKLOG_ERROR, "Cannot remove watch source with id: %lu", m_bus_id);
        }
        m_bus_id = 0;
    }
}


// Stops the loop
void Thread::stopLoop()
{
    klk_log(KLKLOG_DEBUG, "GST thread processing stop signal. "
            "This: 0x%lx. UUID: %s",
            this, m_uuid.c_str());

    freeBus();

    // stop the loop
    {
        Locker lock(&m_loop_lock);
        if (m_loop)
        {
            // finishing the loop
            g_main_loop_quit(m_loop);
        }
    }
}

// stops processing
void Thread::stop() throw()
{
    klk_log(KLKLOG_DEBUG, "GST thread got stop signal. "
            "This: 0x%lx. UUID: %s",
            this, m_uuid.c_str());

    GMainLoop* loop = NULL;

    // to prevent a dead lock in processStageChange
    {
            Locker lock(&m_loop_lock);
            if (m_loop && g_main_loop_is_running(m_loop))
            {
                loop = g_main_loop_ref(m_loop);
            }
    }


    // adding a event to main loop
    // we use it instead of simple g_main_loop_quite to avoid
    // hangs in g_poll
    if (loop)
    {
        // the stop event equvivalent to change state
        processStageChange();
        GMainContext *context = g_main_loop_get_context(m_loop);
        if (context)
        {
            GSource *source = g_idle_source_new();
            g_source_set_callback(source, &Thread::quitCallback, this, NULL);
            g_source_attach(source, context);
            g_source_unref(source);
        }
        else
        {
            KLKASSERT(context);
        }
        g_main_loop_unref(loop);
    }
}

// Do init before startup
void Thread::init()
{

    klk_log(KLKLOG_DEBUG, "GST thread initialization. This: 0x%lx. UUID: %s",
            this, m_uuid.c_str());

    {
        Locker lock(&m_loop_lock);
        BOOST_ASSERT(m_loop == NULL);
        m_loop = g_main_loop_new (NULL, FALSE);
        BOOST_ASSERT(m_loop);

        if (m_timeout)
        {
            // exit after specified timeout
            GMainContext *context = g_main_loop_get_context(m_loop);
            BOOST_ASSERT(context);
            GSource *source = g_timeout_source_new_seconds(m_timeout);
            g_source_set_callback(source, &Thread::quitCallback, this, NULL);
            g_source_attach(source, context);
            g_source_unref(source);
        }
    }


    // pipeline initialization
    {
        Locker lock(&m_pipeline_lock);
        BOOST_ASSERT(m_pipeline == NULL); // only one time
        m_pipeline = gst_pipeline_new (NULL);
        BOOST_ASSERT(m_pipeline);
    }

    // setup GST messages processing
    {
        Element pipeline(getPipeline());
        GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE(pipeline.getElement()));

        Locker lock(&m_bus_lock);
        m_bus_id = gst_bus_add_watch (bus, call_bus, this);
        KLKASSERT(m_bus_id != 0);
        gst_object_unref(bus);
    }
}

// Sets the state for specified pipeline
void Thread::setState(GstState state)
{
    Element pipeline(getPipeline());
    if (gst_element_set_state (GST_ELEMENT(pipeline.getElement()),
                               state) == GST_STATE_CHANGE_FAILURE)
    {
        if (!isStopped())
        {
            addEvent(ERROR);
        }
        throw Exception(__FILE__, __LINE__,
                        "gst_element_set_state() was failed");
    }
}

// Starts playing the pipeline
void Thread::playPipeline()
{
    setState(GST_STATE_PLAYING);
#if 0
    klk_log(KLKLOG_DEBUG, "GST pipeline play was OK");
#endif
}

// Starts playing the pipeline
void Thread::pausePipeline()
{
    setState(GST_STATE_PAUSED);
#if 0
    klk_log(KLKLOG_DEBUG, "GST pipeline pause was OK");
#endif
}


//  Starts the thread
void  Thread::start()
{
    try
    {
        playPipeline();
        klk_log(KLKLOG_INFO, "GST thread has been started. This: 0x%lx. UUID: %s",
                this, m_uuid.c_str());
        g_main_loop_run(getLoop());
    }
    catch(...)
    {
        clean();
        throw;
    }

    clean();
}

// Check if the thread stopped
bool Thread::isStopped()
{
    Locker lock(&m_bus_lock);
    return (m_bus_id == 0);
}

// Free pipeline resource
void Thread::freePipeline()
{
    Locker lock(&m_pipeline_lock);
    if (m_pipeline)
    {
        gst_object_unref (m_pipeline);
        m_pipeline = NULL;
    }
}

// Free loop resource
void Thread::freeLoop()
{
    Locker lock(&m_loop_lock);
    if (m_loop)
    {
        // no need to quit loop because it has been already quited
        g_main_loop_unref(m_loop);
        m_loop = NULL;
    }
}


// cleans the processor internals
void Thread::clean() throw()
{
    klk_log(KLKLOG_DEBUG, "GST thread cleanup. This: 0x%lx. UUID: %s",
            this, m_uuid.c_str());

    try
    {
        // clear events handler
        freeBus();

        // pipeline state -> NULL
        setState(GST_STATE_NULL);
        // free pipeline
        freePipeline();
        // free loop
        freeLoop();
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "GST thread cleanup error. "
                "This: 0x%lx. UUID: %s. Error: %s",
                this, m_uuid.c_str(), err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "GST thread cleanup error. "
                "This: 0x%lx. UUID: %s. Error: unknown",
                this, m_uuid.c_str());
    }
}

// Call bus callback
gboolean  Thread::callBus(GstMessage* msg)
{
#if 0
    klk_log(KLKLOG_DEBUG, "GST got a message with type: %s",
            gst_message_type_get_name(GST_MESSAGE_TYPE (msg)));
#endif
    switch (GST_MESSAGE_TYPE (msg))
    {
    case GST_MESSAGE_EOS:
    {
        addEvent(EOS);
        klk_log(KLKLOG_DEBUG,  "GST EOS detected. GST thread info: "
                "This: 0x%lx. UUID: %s",
                this, m_uuid.c_str());
        // the EOS event equvivalent to change state
        processStageChange();
        // should stop the processing bus event
        return FALSE;
    }
    case GST_MESSAGE_WARNING:
    {
        gchar *debug = NULL;
        GError *err = NULL;

        gst_message_parse_warning(msg, &err, &debug);
        g_free (debug);

        klk_log(KLKLOG_ERROR, "GST Thread Warning: %s. GST thread info: "
                "This: 0x%lx. UUID: %s",
                err->message,
                this, m_uuid.c_str());
        g_error_free (err);
        break;
    }
    case GST_MESSAGE_ERROR:
    {
        gchar *debug = NULL;
        GError *err = NULL;

        gst_message_parse_error (msg, &err, &debug);
        g_free (debug);

        const std::string errstr = err->message;
        g_error_free (err);
        addEvent(ERROR);
        klk_log(KLKLOG_ERROR, "GST Thread Error: %s. GST thread info: "
                        "This: 0x%lx. UUID: %s",
                        errstr.c_str(),
                        this, m_uuid.c_str());
        // should stop the processing bus event
        return FALSE;
    }
#if 0
    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState oldstate, newstate, pending;
        gst_message_parse_state_changed (msg, &oldstate, &newstate, &pending);
        klk_log(KLKLOG_DEBUG, "Changing state %d -> %d. Pending: %d",
                oldstate, newstate, pending);
        if (oldstate == GST_STATE_PLAYING && newstate == GST_STATE_PAUSED)
        {
            // this one recalculates duration time
            // should be called when state changed from playing to paused
            processStageChange();
        }
        break;
    }
#endif
    default:
        break;
    }
    // we should continue recieve messages thus return TRUE
    return TRUE;
}

// Retrives pipeline
Element Thread::getPipeline()
{
    Locker lock(&m_pipeline_lock);
    if (!m_pipeline)
    {
        throw Exception(__FILE__, __LINE__, "Pipeline has not been initialized");
    }
    return Element(m_pipeline);
}

// Retrives main loop
GMainLoop* Thread::getLoop()
{
    Locker lock(&m_loop_lock);
    return m_loop;
}

/**
   @brief IEvent implementation

   IEvent implementation

   @ingroup grGST
*/
class Event__ : public gst::IEvent
{
public:
    /**
       Constructor
    */
    Event__(EventType type, const std::string& uuid) :
        m_type(type), m_uuid(uuid), m_lock(), m_execution_time(0){
    }

    /**
       Destructor
    */
    virtual ~Event__(){}
private:
    const EventType m_type; ///< the type
    const std::string m_uuid; ///< uuid
    mutable Mutex m_lock; ///< locker
    time_t m_execution_time; ///< when the event has to be processed

    /**
       Retrives event type assosiated with the holder
    */
    virtual EventType getType() const {return m_type;}

    /**
       Retrives the gst thread uuid
    */
    virtual const std::string getThreadUUID() const {return m_uuid;}

    /**
       Sets duration for the event

       @param[in] duration - the duration
       before the event processing
    */
    virtual void setDuration(time_t duration){
        Locker lock(&m_lock);
        m_execution_time = time(NULL) + duration;
    }

    /**
       Retrives duration for the event

       @return time interval left before execution
    */
    virtual time_t getTimeLeft() const{
        Locker lock(&m_lock);
        return m_execution_time - time(NULL);
    }

private:
    /**
       Assigment operator
       @param[in] value - the copy param
    */
    Event__& operator=(const Event__& value);

    /**
       Copy constructor
       @param[in] value - the copy param
    */
    Event__(const Event__& value);
};

// Puts an event for processing
void Thread::addEvent(EventType type)
{
    if (m_processor)
    {
        m_processor->addEvent(gst::IEventPtr(new Event__(type, m_uuid)));
    }
}
