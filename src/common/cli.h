/**
   @file cli.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/11/13 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLI_H
#define KLK_CLI_H

#include <boost/assert.hpp>

#include "icli.h"
#include "ifactory.h"
#include "log.h"

namespace klk
{
    namespace cli
    {
        /**
           All parameter

           @ingroup grCLI
        */
        const std::string ALL = "all";

        /**
           @brief The base class for CLI command

           The base class for CLI command

           @ingroup grCLI
        */
        class Command : public ICommand
        {
        public:
            /**
               Constructor

               @param[in] name - the command's name
               @param[in] summary - the summary for the command
               @param[in] usage - the usage for the command
            */
            Command(const std::string& name,
                    const std::string& summary,
                    const std::string& usage);

            /**
               Destructor
            */
            virtual ~Command(){}
        protected:
            template <class T> const boost::shared_ptr<T> getModule() const
            {
                BOOST_ASSERT(m_factory);
                BOOST_ASSERT(!m_modid.empty());
                IModulePtr module = getModuleFactory()->getModule(m_modid);
                BOOST_ASSERT(module);
                return boost::dynamic_pointer_cast<T, IModule>(module);
            }


            /**
               Factory getter method

               @return the factory
            */
            IFactory* getFactory()
            {
                BOOST_ASSERT(m_factory);
                return m_factory;
            }

            /**
               Factory getter method (const method)

               @return the factory
            */
            const IFactory* getFactory() const
            {
                BOOST_ASSERT(m_factory);
                return m_factory;
            }


            /**
               Module factory getter method

               @return the factory

               @note const method
            */
            const IModuleFactory* getModuleFactory() const
            {
                return m_factory->getModuleFactory();
            }

            /**
               Module factory getter method

               @return the factory
            */
            IModuleFactory* getModuleFactory()
            {
                return m_factory->getModuleFactory();
            }
        protected:
            /**
               There is a list with data for output

               It represents a table as a list of rows
            */
            typedef std::list<StringList> TableData;

            /**
               Gets the command usage description

               @return the command's usage description
            */
            virtual const std::string getUsage() const {return m_usage;}

            /**
               Is the command require module be loaded or not

               By default it returns true

               @return
               - true - the module should be loaded
               - false - the module can be unloaded
            */
            virtual bool isRequireModule() const throw() {return true;}
        private:
            std::string m_name; ///< the command's name
            std::string m_summary; ///< the summary for the command
            std::string m_usage; ///< the usage for the command

            IFactory* m_factory; ///< factory object
            std::string m_modid; ///< module id

            /**
               Gets the command name

               @return the command's name
            */
            virtual const std::string getName() const {return m_name;}

            /**
               Gets the command description

               @return the command's description
            */
            virtual const std::string getSummary() const {return m_summary;}

            /**
               Sets factory object

               @param[in] factory - the factory to be set
            */
            virtual void setFactory(IFactory* factory);

            /**
               Sets module ID

               @param[in] id - the id to be set
            */
            virtual void setModuleID(const std::string& id);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Command(const Command& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Command& operator=(const Command& value);
        };
    }
}

#endif //KLK_CLI_H
