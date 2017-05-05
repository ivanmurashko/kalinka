/**
   @file dep.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>

#include "dep.h"
#include "log.h"
#include "exception.h"

using namespace klk;
using namespace klk::mod;

/**
   Cycle detection constant
*/
const std::string CYCLEDETECTED = "Cycle detected";

//
// Visitor for cycle detection
//
struct cycle_detector : public boost::dfs_visitor<>
{
    template <class Edge, class Graph>
    void back_edge(Edge e, Graph& g)
    {
        throw Exception(__FILE__, __LINE__, CYCLEDETECTED);
    }
};


//
// Class Dependency
//

// Constructor
Dependency::Dependency() :
    Mutex(), m_graph(), m_count(0)
{
}

// Adds a dependency between modules
Result Dependency::addDependency(const std::string& child,
                                           const std::string& parent)
{
    Locker lock(this);

    // check self
    if (child == parent)
        return OK;

    // check is this already exist
    StringList deps = getDependency(child);
    if (std::find(deps.begin(), deps.end(), parent) != deps.end())
        return OK; // already in


    Vertex child_id, parent_id;
    typedef boost::property<boost::vertex_index_t,
        std::size_t, Property> VertexIndex;

    if (hasVertex(child))
    {
        child_id = getVertexID(child);
    }
    else
    {
        child_id = boost::add_vertex(VertexIndex(++m_count, child), m_graph);
    }

    if (hasVertex(parent))
    {
        parent_id = getVertexID(parent);
    }
    else
    {
        parent_id = boost::add_vertex(VertexIndex(++m_count, parent), m_graph);
    }

    boost::add_edge(child_id, parent_id, m_graph);

    // detect a cycle
    try
    {
        cycle_detector vis;
        boost::depth_first_search(m_graph, boost::visitor(vis));
    }
    catch(const Exception&)
    {
        boost::remove_edge(child_id, parent_id, m_graph);
        return ERROR;
    }


    return OK;
}

// Removes a dependency between modules
void Dependency::rmDependency(const std::string& child,
                                    const std::string& parent) throw()
{
    Locker lock(this);
    try
    {
        boost::remove_edge(getVertexID(child), getVertexID(parent), m_graph);
    }
    catch(const Exception&)
    {
        // nothing to do
        return;
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Unspecified exception while "
                "removing modules deps");
        return;
    }
}

// Gets dependency list for the specified module
const StringList Dependency::getDependency(const std::string& modid)
{
    Locker lock(this);
    StringList list;
    typedef boost::graph_traits<DependencyGraph> GraphTraits;
    boost::property_map<DependencyGraph, property_tag_t>::type
        index = boost::get(property_tag_t(), m_graph);
    GraphTraits::adjacency_iterator ai, ai_end;

    for (boost::graph_traits<DependencyGraph>::vertex_iterator i =
             boost::vertices(m_graph).first;
         i != vertices(m_graph).second; i++)
    {
        if (index[*i] == modid)
        {
            for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*i, m_graph);
                 ai != ai_end; ++ai)
            {
                list.push_back(index[*ai]);
            }
            break;
        }
    }
    return list;
}

// Gets topological sort of the graph
const StringList Dependency::getSortedList()
{
    Locker lock(this);
    StringList list;

    typedef std::list<Vertex> container;
    container c;

    // do sort
    boost::topological_sort(m_graph, std::back_inserter(c));
    // retrive all vertexes
    boost::property_map<DependencyGraph, property_tag_t>::type
        index = boost::get(property_tag_t(), m_graph);
    for (container::reverse_iterator i = c.rbegin(); i != c.rend(); ++i)
        list.push_back(index[*i]);

    return list;
}

// Clears the dependency list
void Dependency::clear()
{
    Locker lock(this);
    m_count = 0;
    m_graph.clear();
}

// Retrives id for a vertex id by it's name
Vertex Dependency::getVertexID(const std::string& name)
{

    boost::property_map<DependencyGraph, property_tag_t>::type
        index = boost::get(property_tag_t(), m_graph);

    boost::graph_traits<DependencyGraph>::vertex_iterator
        i = boost::vertices(m_graph).first;
    for (; i != vertices(m_graph).second; i++)
    {
        if (index[*i] == name)
        {
            break;
        }
    }

    BOOST_ASSERT(i != vertices(m_graph).second);
    return *i;
}

// Check is there a vertex with the specified name
bool Dependency::hasVertex(const std::string& name)
{
    boost::property_map<DependencyGraph, property_tag_t>::type
        index = boost::get(property_tag_t(), m_graph);

    boost::graph_traits<DependencyGraph>::vertex_iterator
        i = boost::vertices(m_graph).first;
    for (; i != vertices(m_graph).second; i++)
    {
        if (index[*i] == name)
        {
            return true;
        }
    }

    return false;
}
