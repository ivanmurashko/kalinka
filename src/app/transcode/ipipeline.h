/**
   @file ipipeline.h
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

#ifndef KLK_IPIPELINE_H
#define KLK_IPIPELINE_H

#include <gst/gst.h>

#include <boost/shared_ptr.hpp>

#include "task.h"

namespace klk
{
    namespace trans
    {
        /** @addtogroup grTrans

            Base interfaces

            @{
        */

        /**
           @brief Pipeline interface

           It uses to retrive pipeline specifics at some GST actions
        */
        class IPipeline
        {
        public:
            /**
               Destructor
            */
            virtual ~IPipeline(){}

            /**
               Starts playing the pipeline

               @exception klk::Exception - there was an error
            */
            virtual void play() = 0;

            /**
               Sets pipeline state into paused

               @exception klk::Exception - there was an error
            */
            virtual void pause() = 0;
        };

        /**
           @brief The interface is used for transcoding branches creation

           It analized the task and created GStreamer branch as GST_BIN
           element for it. The sink is accesable via sink ghost pad.
        */
        class IBranchFactory
        {
        public:
            /**
               Destructor
            */
            virtual ~IBranchFactory(){}

            /**
               Creates the branch

               @param[in] task - the task info container

               @return the branch packed to a bin
            */
            virtual GstElement* createBranch(const TaskPtr& task) = 0;
        };

        /**
           Smart pointer for the branch factory
        */
        typedef boost::shared_ptr<IBranchFactory> IBranchFactoryPtr;

        /**
           @brief The factory object to create branch factories

           The interface is used to create klk::trans::IBranchFactory objects
        */
        class IFactoryMaker
        {
        public:
            /**
               Destructor
            */
            virtual ~IFactoryMaker(){}

            /**
               The main method to create the branch factory

               @return the branch factory instance
            */
            virtual const IBranchFactoryPtr makeFactory() const = 0;
        };

        /**
           Smart pointer for the factory maker
        */
        typedef boost::shared_ptr<IFactoryMaker> IFactoryMakerPtr;

        /** @} */

    }
}

#endif //KLK_IPIPELINE_H
