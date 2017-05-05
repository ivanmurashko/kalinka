/**
   @file cmd.cpp
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
   - 2009/11/30 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cmd.h"
#include "db.h"
#include "exception.h"

using namespace klk;
using namespace klk::trans;

//
// BaseCommand class
//

// Constructor
BaseCommand::BaseCommand(const std::string& name,
                         const std::string& summary,
                         const std::string& usage) : cli::Command(name, summary, usage)
{
}

// Retrives a list with possible source types names
const StringList BaseCommand::getSourceNameList()
{
    StringList list;

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_app_transcode_source_show_cli",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["source_name"].toString());
    }

    return list;
}

// Retrives a list with asigned task names
const StringList
BaseCommand::getTaskNameList()
{
    StringList list;

    db::DB db(getFactory());
    db.connect();

    db::Parameters dbparams;

    db::ResultVector rv = db.callSelect("klk_app_transcode_get",
                                        dbparams, NULL);
    for (db::ResultVector::iterator item = rv.begin();
         item != rv.end(); item++)
    {
        list.push_back((*item)["task_name"].toString());
    }

    return list;
}
