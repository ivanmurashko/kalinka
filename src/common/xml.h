/**
   @file xml.h
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
   - 2008/10/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_XML_H
#define KLK_XML_H

#include <string>

#include <xercesc/parsers/XercesDOMParser.hpp>

#include "common.h"

namespace klk
{
    /** @defgroup grXML XML processing
    @{

    XML processing

    Kalinka xml's has some restrinctions - each node can have several children
    but all children should have different names

    No attributes inside the xml

    @ingroup grCommon
*/

    /**
       @brief The XML class

       The XML document holder
    */
    class XML
    {
    public:
        /**
           Constructor
        */
        XML();

        /**
           Destructor
        */
        virtual ~XML();

        /**
           Parses an XML file

           @param[in] path - the path to a file

           @exception @ref klk::Exception
        */
        void parseFromFile(const std::string& path);

        /**
           Parses a string with XML

           @param[in] data - the data to be parsed

           @exception @ref klk::Exception
        */
        void parseFromData(const std::string& data);

        /**
           Gets a value of an XML tag by its XPath

           @note the tag (node) should be unique

           @exception @ref klk::ERROR - the node does not exists or
           there are more than one such node exist

           @return the tag's data value
        */
        std::string getValue(const std::string& xpath) const;
    private:
        XERCES_CPP_NAMESPACE::XercesDOMParser* m_parser; ///< parser
        XERCES_CPP_NAMESPACE::ErrorHandler* m_error_handler; ///< error handler

        /**
           Gets a child node by its name

           @param[in] name - the node's name
           @param[in] parent

           @exception @ref klk::ERROR - the node does not exists or
           there are more than one such node exist

           @return the node
        */
        const XERCES_CPP_NAMESPACE::DOMNode* getNode(
            const std::string& name,
            const XERCES_CPP_NAMESPACE::DOMNode* parent) const;
    private:
        /**
           Copy constructor
        */
        XML(const XML&);

        /**
           Assignment opearator
        */
        XML& operator=(const XML&);
    };
    /** @} */
}

#endif //KLK_XML_H
