/**
   @file dep.h
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
   - 2008/12/03 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MODDEP_H
#define KLK_MODDEP_H

#include <sys/types.h>

#define BOOST_NO_HASH //FIXME!!! - to make gcc happy
#include <boost/graph/adjacency_list.hpp>
#undef BOOST_NO_HASH
#include <boost/graph/graph_traits.hpp>

#include "stringlist.h"
#include "thread.h"

namespace klk
{
    namespace mod
    {
        /**
           A helper structure
        */
        struct property_tag_t
        {
            typedef boost::vertex_property_tag kind;
        };

        /**
            dependency property
        */
        typedef boost::property<property_tag_t, std::string> Property;

        /**
           The graph type for dependency
        */
        typedef boost::adjacency_list<boost::vecS, boost::vecS,
            boost::directedS, Property> DependencyGraph;

        /**
           Vertex descriptor
        */
        typedef boost::graph_traits<DependencyGraph>::vertex_descriptor Vertex;


        /**
           @brief The module dependency info container

           The module dependecy container

           @ingroup grModuleFactory
        */
        class Dependency : public klk::Mutex
        {
        public:
            /**
               Constructor
            */
            Dependency();

            /**
               Destructor
            */
            ~Dependency(){}

            /**
               Adds a dependency between modules

               @param[in] child - the child element
               @param[in] parent - the parent element

               @return
               - @ref klk::OK - the dependency was successfully added
               - @ref klk::ERROR - the dependency can not be added
            */
            klk::Result addDependency(const std::string& child,
                                      const std::string& parent);

            /**
               Removes a dependency between modules

               @param[in] child - the child element
               @param[in] parent - the parent element
            */
            void rmDependency(const std::string& child,
                              const std::string& parent) throw();

            /**
               Gets dependency list for the specified module

               @param[in] modid - the module id

               @return the list with modules id from that the module
               specified at the arg depends on

               @note it returns only adjacent vertexes
            */
            const StringList getDependency(const std::string& modid);

            /**
               Gets topological sort of the graph

               First element is a module id with the highest dependency
               (depends on everything thus should be stopped first)

               @return the topological sorted list
            */
            const StringList getSortedList();

            /**
               Clears the dependency list
            */
            void clear();
        private:
            DependencyGraph m_graph; ///< the graph object
            uint m_count; ///< indexes count

            /**
               Retrives id for a vertex id by it's name

               @param[in] name - the vertex name

               @return the id assigned to the vertex

               @exception @ref klk::Exception
            */
            Vertex getVertexID(const std::string& name);

            /**
               Check is there a vertex with the specified name

               @param[in] name - the vertex name

               @return
               - true
               - false
            */
            bool hasVertex(const std::string& name);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Dependency(const Dependency& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Dependency& operator=(const Dependency& value);
        };
    }
}


#endif //KLK_MODDEP_H
