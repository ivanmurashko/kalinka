/**
   @file branchfactory.h
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

#ifndef KLK_BRANCHFACTORY_H
#define KLK_BRANCHFACTORY_H

#include <string>
#include <map>

#include "ipipeline.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief The object is used for transcoding branches creation

           It analized the task and created GStreamer branch as GST_BIN
           element for it. The sink is accesable via sink ghost pad.

           @ingroup grTrans
        */
        class BranchFactory : public IBranchFactory
        {
        public:
            /**
               Constructor

               @param[in] factory - the base factory
               @param[in] pipeline - the pipeline for callbacks
            */
            BranchFactory(IFactory* factory, IPipeline* pipeline);

            /**
               Destructor
            */
            virtual ~BranchFactory(){}
        private:
            Mutex m_lock; ///< locker

            /**
               Branch factories makers container

               media uuid -> factory maker shared pointer
            */
            typedef std::map<std::string, IFactoryMakerPtr> FactoryMakers;
            FactoryMakers m_makers; ///< media uuid -> factory makers

            /**
               Branch factories container

               task uuid -> shared pointer to branch factory
            */
            typedef std::map<std::string, IBranchFactoryPtr> Storage;
            Storage m_factories; ///< task uuid -> branch factory

            /**
               Creates the branch

               @param[in] task - the task info container

               @return the branch packed to a bin
            */
            virtual GstElement* createBranch(const TaskPtr& task);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            BranchFactory(const BranchFactory& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            BranchFactory& operator=(const BranchFactory& value);
        };

    }
}

#endif //KLK_BRANCHFACTORY_H
