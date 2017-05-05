/**
   @file xml.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "xml.h"
#include "log.h"
#include "exception.h"

using namespace klk;

/**
   @def X
   @brief X is necessary for
   X is necessary for
*/
#define X(val) XERCES_CPP_NAMESPACE::XMLString::transcode(val)

//
// XML class
//

// Constructor
XML::XML() :
    m_parser(NULL),
    m_error_handler(NULL)
{
    m_parser = new XERCES_CPP_NAMESPACE::XercesDOMParser();
    KLKASSERT(m_parser);
    m_error_handler =
        static_cast<XERCES_CPP_NAMESPACE::ErrorHandler*>
        (new XERCES_CPP_NAMESPACE::HandlerBase());
    KLKASSERT(m_error_handler);

    m_parser->setValidationScheme(
        XERCES_CPP_NAMESPACE::XercesDOMParser::Val_Always);
    m_parser->setDoNamespaces(true);    // optional
    m_parser->setErrorHandler(m_error_handler);

}

// Destructor
XML::~XML()
{
    KLKDELETE(m_parser);
    KLKDELETE(m_error_handler);
}

// Parses an XML file
// @param[in] path - the path to a file
void XML::parseFromFile(const std::string& path)
{
    try
    {
        m_parser->parse(path.c_str());
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& to_catch)
    {
        char* msg =
            XERCES_CPP_NAMESPACE::XMLString::transcode(to_catch.getMessage());
        BOOST_ASSERT(msg);
        std::string descr = std::string("XML parser error: ") + msg +
            ". File name: " + path;
        XERCES_CPP_NAMESPACE::XMLString::release(&msg);
        throw klk::Exception(__FILE__, __LINE__, descr);
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& to_catch)
    {
        char* msg = XERCES_CPP_NAMESPACE::XMLString::transcode(to_catch.msg);
        std::string descr = std::string("DOM XML parser error: ") + msg +
            ". File name: " + path;
        XERCES_CPP_NAMESPACE::XMLString::release(&msg);
        throw klk::Exception(__FILE__, __LINE__, descr);
    }
    catch (const XERCES_CPP_NAMESPACE::SAXParseException& to_catch)
    {
        char* msg =
            XERCES_CPP_NAMESPACE::XMLString::transcode(to_catch.getMessage());
        std::string descr = std::string("SAX XML parser error: ") + msg +
            ". File name: " + path;
        XERCES_CPP_NAMESPACE::XMLString::release(&msg);
        throw klk::Exception(__FILE__, __LINE__, descr);
    }
    catch (...)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Unexpected XML parser exception");
    }
 }

// Parses a string with XML
void XML::parseFromData(const std::string& data)
{
    try
    {
        const char* c_data = data.c_str();
        XERCES_CPP_NAMESPACE::MemBufInputSource
            source(reinterpret_cast<const XMLByte*>(c_data),
                   strlen(c_data), "klkxmldatabuffer", false);
        //klk_log(KLKLOG_DEBUG, data.c_str());
        m_parser->parse(source);
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& to_catch)
    {
        char* msg =
            XERCES_CPP_NAMESPACE::XMLString::transcode(to_catch.getMessage());
        std::string descr = std::string("XML parser error: ") + msg;
        XERCES_CPP_NAMESPACE::XMLString::release(&msg);
        throw klk::Exception(__FILE__, __LINE__, descr);
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& to_catch)
    {
        char* msg = XERCES_CPP_NAMESPACE::XMLString::transcode(to_catch.msg);
        std::string descr = std::string("DOM XML parser error: ") + msg;
        XERCES_CPP_NAMESPACE::XMLString::release(&msg);
        throw klk::Exception(__FILE__, __LINE__, descr);
    }
    catch (...)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Unexpected XML parser exception");
    }
}


// Gets a value of an XML tag by its XPath
// @note the tag (node) should be unique
// @exception @ref klk::ERROR - the node does not exists or
// there are more than one such node exist
std::string XML::getValue(const std::string& xpath) const

{
    BOOST_ASSERT(m_parser);

    // parse xpath
    std::vector<std::string> nodes;
    boost::split(nodes, xpath, boost::is_any_of("/"));

    // trim all
    std::for_each(nodes.begin(), nodes.end(),
                  boost::bind(&boost::trim<std::string>, _1, std::locale()));

    XERCES_CPP_NAMESPACE::DOMDocument *doc = m_parser->getDocument();
    BOOST_ASSERT(doc);

    const XERCES_CPP_NAMESPACE::DOMNode* node = doc;
    const XERCES_CPP_NAMESPACE::DOMNode* parent = doc;
    for (std::vector<std::string>::iterator i = nodes.begin();
         i != nodes.end(); i++)
    {
        if (i->empty())
        {
            continue;
        }
        parent = node;
        node = getNode(*i, parent);
    }
    BOOST_ASSERT(node);
    char* value =
        XERCES_CPP_NAMESPACE::XMLString::transcode(node->getTextContent());
    std::string res = value;
    XERCES_CPP_NAMESPACE::XMLString::release(&value);
    boost::trim(res);
    return res;
}

// Gets a child node by its name
// @param[in] name - the node's name
// @param[in] parent
// @exception @ref klk::ERROR - the node does not exists or
// there are more than one such node exist
// @return the node
const XERCES_CPP_NAMESPACE::DOMNode* XML::getNode(
    const std::string& name,
    const XERCES_CPP_NAMESPACE::DOMNode* parent) const
{
    const XERCES_CPP_NAMESPACE::DOMNode* res = NULL;
    BOOST_ASSERT(parent);
    BOOST_ASSERT(name.empty() == false);
    XMLCh *tag_name =
        XERCES_CPP_NAMESPACE::XMLString::transcode(name.c_str());
    try
    {
        XERCES_CPP_NAMESPACE::DOMNodeList* list = parent->getChildNodes();
        for (uint i = 0; i < list->getLength(); i++)
        {
            const XERCES_CPP_NAMESPACE::DOMNode* node =
                list->item(i);
            BOOST_ASSERT(node);
            const XMLCh* node_name = node->getNodeName();
            BOOST_ASSERT(node_name);
            if (XERCES_CPP_NAMESPACE::XMLString::compareIString(node_name,
                                                                tag_name) == 0)
            {
                BOOST_ASSERT(res == NULL); // only one time
                res = node;
            }
        }

        if (res == NULL)
        {
            throw klk::Exception(__FILE__, __LINE__,
                                 "Node '%s' has not been found", name.c_str());
        }
    }
    catch(klk::Exception&)
    {
        XERCES_CPP_NAMESPACE::XMLString::release(&tag_name);
        throw;
    }
    catch(...)
    {
        XERCES_CPP_NAMESPACE::XMLString::release(&tag_name);
        throw klk::Exception(__FILE__, __LINE__, "Unknown XML exception");
    }
    XERCES_CPP_NAMESPACE::XMLString::release(&tag_name);

    return res;
}
