/**
   @file rater.h
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
   - 2009/05/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_RATER_H
#define KLK_RATER_H

#include "thread.h"

namespace klk
{
    /**
       @brief The class mesure bit rate

       The class mesure bit rate (speed) of data transfer

       @ingroup grSocket
    */
    class Rater
    {
    public:
        /**
           Constructor
        */
        Rater();

        /**
           Destructor
        */
        virtual ~Rater();

        /**
           Updates received data size

           @param[in] size - the size to be added
        */
        void updateInput(const size_t size);

        /**
           Updates sent data size

           @param[in] size - the size to be added
        */
        void updateOutput(const size_t size);

        /**
           Retrives input rate

           @return the input rate
        */
        const double getInputRate() const;

        /**
           Retrives output rate

           @return the output rate
        */
        const double getOutputRate() const;
    private:
        mutable Mutex m_lock; ///< locker
        double m_in_rate; ///< last mesured input rate
        double m_out_rate; ///< last mesured out rate
        size_t m_in_size; ///< input data received
        size_t m_out_size; ///< output data sent
        time_t m_in_last; ///< last mesured time (for input data)
        time_t m_out_last; ///< last mesured time (for output data)
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Rater(const Rater& value);


        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Rater& operator=(const Rater& value);
    };
};

#endif //KLK_RATER_H
