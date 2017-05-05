/**
   @file launchdev.h
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
   - 2010/09/19 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_LAUNCHDEV_H
#define KLK_LAUNCHDEV_H

#include "iresources.h"
#include "ifactory.h"
#include "adapter/devprotocol.h"

namespace klk
{
    namespace launcher
    {
        /**
           @brief IDev iface realization

           The realizaion retrives all info about a particular dev via RPC
           from main mediaserver

           @ingroup grLauncher
        */
        class Dev : public IDev
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory instance top construct another objects
               @param[in] dev_uuid - the device uuid
            */
            Dev(IFactory* factory, const std::string& dev_uuid);

            /// Destructor
            virtual ~Dev(){}
        private:
            const std::string m_dev_uuid; ///< the dev uuid to be accessed via the class
            mutable adapter::DevProtocol m_proxy;


            ///@copydoc klk::IDev::getState()
            virtual klk::dev::State getState() const;

            ///@copydoc klk::IDev::setState()
            virtual void setState(klk::dev::State state);

            ///@copydoc klk::IDev::hasParam()
            virtual bool hasParam(const std::string& key) const;

            ///@copydoc klk::IDev::getStringParam()
            virtual const std::string
                getStringParam(const std::string& key) const;

            ///@copydoc klk::IDev::getIntParam()
            virtual const int getIntParam(const std::string& key) const;

            /**
               Sets a value for a dev's parameter

               @param[in] key - the key for retriiving
               @param[in] value - the value to be set
               @param[in] mutable_flag - do update (false) the last or not (true)

               @see IDev::getParam
            */
            virtual void setParam(const std::string& key,
                                  const std::string& value,
                                  bool mutable_flag = false);

            /**
               Sets a value for a dev's parameter

               @param[in] key - the key for retriiving
               @param[in] value - the value to be set
               @param[in] mutable_flag - do update (false) the last or not (true)

               @see IDev::getParam
            */
            virtual void setParam(const std::string& key,
                                  const int value,
                                  bool mutable_flag = false);

            ///@copydoc klk::IDev::getLastUpdateTime()
            virtual const time_t getLastUpdateTime() const;

            ///@copydoc klk::IDev::update()
            virtual void update();

            ///@copydoc klk::IDev::updateDB()
            virtual void updateDB();

            ///@copydoc klk::IDev::equal_to()
            virtual bool equal_to(const boost::shared_ptr<IDev>& dev) const;
        private:
            /// Fake copy constructor
            Dev(const Dev&);

            /// Fake assigment operator
            Dev& operator=(const Dev&);
        };
    }
}

#endif //KLK_LAUNCHDEV_H
