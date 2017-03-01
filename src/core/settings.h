/**************************************************************************
 *   settings.h  --  This file is part of Afelirin.                       *
 *                                                                        *
 *   Copyright (C) 2016, Ivo Filot                                        *
 *                                                                        *
 *   Netris is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published    *
 *   by the Free Software Foundation, either version 3 of the License,    *
 *   or (at your option) any later version.                               *
 *                                                                        *
 *   Netris is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <iostream>
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <glm/glm.hpp>

#include "core/asset_manager.h"

class Settings {
private:
    std::string settings_file;
    boost::property_tree::ptree root;

public:
    static Settings& get() {
        static Settings settings_instance;
        return settings_instance;
    }

    glm::vec3 get_color_from_keyword(const std::string& keyword);
    std::string get_string_from_keyword(const std::string& keyword);
    float get_float_from_keyword(const std::string& keyword);
    unsigned int get_uint_from_keyword(const std::string& keyword);
    bool get_boolean_from_keyword(const std::string& keyword);

private:
    Settings();

    void load();

    glm::vec3 rgb2vec3(const std::string& rgb);

    Settings(Settings const&)          = delete;
    void operator=(Settings const&)  = delete;
};

#endif //_SETTINGS_H
