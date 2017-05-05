/**
   @file branchfactory.cpp
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

#include "branchfactory.h"
#include "simplebranchfactory.h"
#include "flvbranchfactory.h"
#include "theorabranchfactory.h"
#include "exception.h"
#include "defines.h"
#include "media.h"

using namespace klk;
using namespace klk::trans;

// Constructor
BranchFactory::BranchFactory(IFactory* factory, IPipeline* pipeline) :
    m_lock(), m_makers(), m_factories()
{
    BOOST_ASSERT(factory);
    BOOST_ASSERT(pipeline);
    // fill factories
    m_makers[media::FLV] =
        IFactoryMakerPtr(new FLVBranchFactory(factory, pipeline));
    m_makers[media::THEORA] =
        IFactoryMakerPtr(new TheoraBranchFactory(factory, pipeline));
    m_makers[media::EMPTY] =
        IFactoryMakerPtr(new SimpleBranchFactory());
}

// Creates the branch
GstElement* BranchFactory::createBranch(const TaskPtr& task)
{
    BOOST_ASSERT(task);

    Locker lock(&m_lock);

    // check the task
    if (m_factories.find(task->getUUID()) != m_factories.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Task was already initalized. "
                        "UUID: %s.",
                        task->getUUID().c_str());
    }

    const std::string media_type = task->getMediaType();

    FactoryMakers::iterator find = m_makers.find(media_type);
    if (find == m_makers.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Unsupported media type: " + media_type);
    }
    // found something
    IBranchFactoryPtr factory = find->second->makeFactory();
    m_factories[task->getUUID()] = factory;
    return factory->createBranch(task);
}

