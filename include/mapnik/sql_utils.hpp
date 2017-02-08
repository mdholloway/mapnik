/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2016 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MAPNIK_SQL_UTILS_HPP
#define MAPNIK_SQL_UTILS_HPP

// mapnik
#include <mapnik/util/trim.hpp> // for trim

// boost
#pragma GCC diagnostic push
#include <mapnik/warning_ignore.hpp>
#include <boost/algorithm/string/replace.hpp>
#pragma GCC diagnostic pop

// stl
#include <sstream>
#include <vector>
#include <string>

namespace mapnik { namespace sql_utils {

    struct quoted_string
    {
        std::string const* operator-> () const { return &str; }
        std::string const& str;
        char const quot;
    };

    inline quoted_string identifier(std::string const& str)
    {
        return { str, '"' };
    }

    inline quoted_string literal(std::string const& str)
    {
        return { str, '\'' };
    }

    inline std::ostream& operator << (std::ostream& os, quoted_string qs)
    {
        std::size_t pos = 0, next;

        os.put(qs.quot);
        while ((next = qs->find(qs.quot, pos)) != std::string::npos)
        {
            os.write(qs->data() + pos, next - pos + 1);
            os.put(qs.quot);
            pos = next + 1;
        }
        if ((next = qs->size()) > pos)
        {
            os.write(qs->data() + pos, next - pos);
        }
        return os.put(qs.quot);
    }

    // Does nothing if `str` doesn't start with `quot`.
    // Otherwise erases the opening quote, collapses inner quote pairs,
    // and erases everything from the closing quote to the end of the
    // string. The closing quote is the first non-paired quote after the
    // opening one. For a well-formed quoted string, it is also the last
    // character, so nothing gets lost.
    inline void unquote(char quot, std::string & str)
    {
        if (!str.empty() && str.front() == quot)
        {
            std::size_t di = 0;
            for (std::size_t si = 1; si < str.size(); ++si)
            {
                char c = str[si];
                if (c == quot && (++si >= str.size() || str[si] != quot))
                    break;
                str[di++] = c;
            }
            str.erase(di);
        }
    }

    inline std::string unquote_copy(char quot, std::string const& str)
    {
        std::string tmp(str);
        sql_utils::unquote(quot, tmp);
        return tmp;
    }

    [[deprecated("flawed")]]
    inline std::string unquote_double(std::string const& sql)
    {
        std::string table_name = sql;
        util::unquote_double(table_name);
        return table_name;
    }

    [[deprecated("flawed")]]
    inline std::string unquote(std::string const& sql)
    {
        std::string table_name = sql;
        util::unquote(table_name);
        return table_name;
    }

    [[deprecated("flawed")]]
    inline void quote_attr(std::ostringstream & s, std::string const& field)
    {
        s << ",\"" << field << "\"";
    }

    inline std::string table_from_sql(std::string const& sql)
    {
        std::string table_name = sql;
        boost::algorithm::replace_all(table_name,"\n"," ");
        boost::algorithm::ireplace_all(table_name," from "," FROM ");

        std::string::size_type idx = table_name.rfind(" FROM ");
        if (idx!=std::string::npos)
        {
            idx = table_name.find_first_not_of(" ",idx+5);
            if (idx != std::string::npos)
            {
                table_name=table_name.substr(idx);
            }
            idx = table_name.find_first_of(", )");
            if (idx != std::string::npos)
            {
                table_name = table_name.substr(0,idx);
            }
        }
        return table_name;
    }
}}

#endif // MAPNIK_SQL_UTILS_HPP
