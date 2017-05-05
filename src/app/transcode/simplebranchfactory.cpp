/**
   @file simplebranchfactory.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "simplebranchfactory.h"
#include "exception.h"

using namespace klk;
using namespace klk::trans;

//
// SimpleBranchFactory class
//

// Constructor
SimpleBranchFactory::SimpleBranchFactory() : SimpleBaseBranchFactory()
{
}

// Destructor
SimpleBranchFactory::~SimpleBranchFactory()
{
}

// Creates the simple branch
GstElement* SimpleBranchFactory::createBranch(const TaskPtr& task)
{
    // add the element
    GstElement *branch = gst_bin_new (NULL);
    BOOST_ASSERT(branch);

    GstElement *queue = makeQueue();
    GstElement *dest = task->getDestinationElement();
    BOOST_ASSERT(dest);
    gst_bin_add_many (GST_BIN (branch), queue, dest, NULL);

    // create ghost pad for future links
    GstPad *queue_pad = gst_element_get_pad (queue, "sink");
    BOOST_ASSERT(queue_pad);
    GstPad *branch_pad = gst_ghost_pad_new ("sink", queue_pad);
    BOOST_ASSERT(branch_pad);
    if (!gst_element_add_pad (branch, branch_pad))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_add_pad() was failed");
    }
    gst_object_unref (queue_pad);

    // link elements together
    if (!gst_element_link_many (queue, dest, NULL))
    {
        throw Exception(__FILE__, __LINE__,
                        "gst_element_link_many() was failed");
    }

    return branch;
}
