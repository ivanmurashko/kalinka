/**
   @file options.h
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
   - 2009/07/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_OPTIONS_H
#define KLK_OPTIONS_H

#include <string>
#include <map>

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               Options container
            */
            typedef std::map<std::string, std::string> OptionMap;


            /**
               @brief Option converter

               OptionConverter
            */
            class OptionConverter
            {
            public:
                /**
                   Constructor
                */
                OptionConverter();

                /**
                   Destructor
                */
                virtual ~OptionConverter(){}

                /**
                   Retrives code rate
                */
                const std::string getCodeRate(const std::string& key) const
                {
                    return getOption(key, m_code_rate);
                }

                /**
                   Retrives bandwidth
                */
                const std::string getBandwidth(const std::string& key) const
                {
                    return getOption(key, m_bandwidth);
                }

                /**
                   Retrives modulation
                */
                const std::string getModulation(const std::string& key) const
                {
                    return getOption(key, m_modulation);
                }

                /**
                   Retrives transmode
                */
                const std::string getTransmode(const std::string& key) const
                {
                    return getOption(key, m_transmode);
                }

                /**
                   Retrives guard
                */
                const std::string getGuard(const std::string& key) const
                {
                    return getOption(key, m_guard);
                }

                /**
                   Retrives hierarchy
                */
                const std::string getHierarchy(const std::string& key) const
                {
                    return getOption(key, m_hierarchy);
                }
            private:
                OptionMap m_code_rate; ///< code rate map
                OptionMap m_bandwidth; ///< bandwidth map
                OptionMap m_modulation; ///< modulation map
                OptionMap m_transmode; ///< transmode map
                OptionMap m_guard; ///< guard map
                OptionMap m_hierarchy; ///< hierarchy map

                /**
                   Gets an option from the specified map
                */
                const std::string getOption(const std::string& key,
                                            const OptionMap& map) const;
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                OptionConverter(const OptionConverter& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                OptionConverter& operator=(const OptionConverter& value);
            };
        }
    }
}


#endif //KLK_OPTIONS_H
