/**
   @file options.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/assert.hpp>

#include "options.h"
#include "exception.h"

using namespace klk;
using namespace klk::dvb::scan;

//
// OptionConverter class
//

// Constructor
OptionConverter::OptionConverter() :
    m_code_rate(), m_bandwidth(),
    m_modulation(), m_transmode(),
    m_guard(), m_hierarchy()
{
    m_code_rate["NONE"] = "0";
    m_code_rate["1/2"] = "1";
    m_code_rate["2/3"] = "2";
    m_code_rate["3/4"] = "3";
    m_code_rate["4/5"] = "4";
    m_code_rate["5/6"] = "5";
    m_code_rate["6/7"] = "6";
    m_code_rate["7/8"] = "7";
    m_code_rate["8/9"] = "8";
    m_code_rate["AUTO"] = "9";

    m_bandwidth["8MHz"] = "0";
    m_bandwidth["7MHz"] = "1";
    m_bandwidth["6MHz"] = "2";
    m_bandwidth["AUTO"] = "3";

    m_modulation["QPSK"] = "0";
    m_modulation["QAM16"] = "1";
    m_modulation["QAM32"] = "2";
    m_modulation["QAM64"] = "3";
    m_modulation["QAM128"] = "4";
    m_modulation["QAM256"] = "5";
    m_modulation["AUTO"] = "6";
    m_modulation["8VSB"] = "7";
    m_modulation["16VSB"] = "8";

    m_transmode["2k"] = "0";
    m_transmode["8k"] = "1";
    m_transmode["AUTO"] = "2";


    m_guard["1/32"] = "0";
    m_guard["1/16"] = "1";
    m_guard["1/8"] = "2";
    m_guard["1/4"] = "3";
    m_guard["AUTO"] = "4";

    m_hierarchy["NONE"] = "0";
    m_hierarchy["1"] = "1";
    m_hierarchy["2"] = "2";
    m_hierarchy["4"] = "3";
    m_hierarchy["AUTO"] = "4";

}

// Gets an option from the specified map
const std::string
OptionConverter::getOption(const std::string& key,
                           const OptionMap& map) const
{
    OptionMap::const_iterator find = map.find(key);
    BOOST_ASSERT(find != map.end());
    return find->second;
}
