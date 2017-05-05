/**
   @file basedev.h
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
   - 2009/01/24 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_BASEDEV_H
#define KLK_BASEDEV_H

#include <map>
#include <string>

#include "ifactory.h"
#include "iresources.h"
#include "thread.h"
#include "stringwrapper.h"

namespace klk
{
namespace dev
{
    /**
       @brief Base class for devices

       Base class for devices

       @ingroup grResources
    */
    class Base : public IDev
    {
    public:
        /**
           Constructor

           @param[in] factory - the factory
           @param[in] type - the dev type
        */
        Base(IFactory* factory, const std::string& type);

        /**
           Destructor
        */
        virtual ~Base(){}

    protected:
        /**
           @copydoc IDev::setParam

           @note sets an integer parameter
        */
        virtual void setParam(const std::string& key,
                              const std::string& value,
                              bool mutable_flag = false);
        /**
           @copydoc IDev::setParam

           @note sets a string parameter
        */
        virtual void setParam(const std::string& key,
                              const int value,
                              bool mutable_flag = false);
        /// @copydoc IDev::getStringParam
        virtual const std::string getStringParam(const std::string& key) const;

        /// @copydoc IDev::getIntParam
        virtual const int getIntParam(const std::string& key) const;

        /// @copydoc IDev::hasParam()
        virtual bool hasParam(const std::string& key) const;

        /**
           Gets the factory

           @return the factory

           @exception @ref klk::Exception
        */
        IFactory* getFactory();

        /**
           Gets the factory const method

           @return the factory

           @exception klk::Exception
        */
        const IFactory* getFactory() const;
    private:
        /**
           Parameters container
        */
        typedef std::map<std::string, klk::StringWrapper> ParamContainer;

        IFactory* const m_factory; ///< factory for getting all necessary info
        mutable Mutex m_lock; ///< mutex for locks
        ParamContainer m_params; ///< parameters
        time_t m_last_update_time; ///< last update time
        dev::State m_state; ///< dev's state

        /**
           Retrives variant parameter

           @param[in] key - the key to retrive the parameter

           @return the parameter
        */
        const klk::StringWrapper getParam(const std::string& key) const;

        /// @copydoc IDev::getState()
        virtual klk::dev::State getState() const;

        /// @copydoc IDev::setState
        virtual void setState(klk::dev::State state);

        /// @copydoc IDev::getLastUpdateTime()
        virtual const time_t getLastUpdateTime() const;

        /**
           Sets a parameter

           @param[in] key - the key
           @param[in] value - the value
           @param[in] mutable_flag - the mutable flag
        */
        template <class T> void set(const std::string& key, const T& value,
                                    bool mutable_flag);
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Base(const Base& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Base& operator=(const Base& value);
    };
}
}

#endif //KLK_BASEDEV_H
