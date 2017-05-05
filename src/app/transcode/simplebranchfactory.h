/**
   @file simplebranchfactory.h
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

#ifndef KLK_SIMPLEBRANCHFACTORY_H
#define KLK_SIMPLEBRANCHFACTORY_H

#include "basebranchfactory.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief Simple barnch creation factory

           Simple barnch creation factory

           @ingroup grTrans
        */
        class SimpleBranchFactory : public SimpleBaseBranchFactory,
            public IFactoryMaker
        {
        public:
            /**
               Constructor
            */
            SimpleBranchFactory();

            /**
               Destructor
            */
            virtual ~SimpleBranchFactory();
        private:
            /**
               Creates the simple branch

               @param[in] task - the task info container

               @return the branch packed to a bin
            */
            virtual GstElement* createBranch(const TaskPtr& task);

            /// @copydoc klk::trans::IFactoryMaker::makeFactory
            virtual const IBranchFactoryPtr makeFactory() const
            {
                return IBranchFactoryPtr(new SimpleBranchFactory());
            }
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SimpleBranchFactory(const SimpleBranchFactory& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SimpleBranchFactory& operator=(const SimpleBranchFactory& value);
        };
    }
}

#endif //KLK_SIMPLEBRANCHFACTORY_H
