/**************************************************************************
 *   settings.cpp  --  This file is part of Afelirin.                     *
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

#include "settings.h"

Settings::Settings() {
    this->settings_file = AssetManager::get().get_root_directory() + "assets/configuration/settings.json";
    this->load();
}

void Settings::load() {
    try {
        boost::property_tree::read_json(this->settings_file, this->root);
    } catch(std::exception const& ex) {
        std::cerr << "[ERROR] There was an error parsing the JSON tree" << std::endl;
        std::cerr << ex.what() << std::endl;
        std::cerr << "[ERROR] Terminating program" << std::endl;
        exit(-1);
    }
}

glm::vec3 Settings::get_color_from_keyword(const std::string& keyword) {
    const std::string col = this->root.get<std::string>(keyword);
    return this->rgb2vec3(col);
}

std::string Settings::get_string_from_keyword(const std::string& keyword) {
    return this->root.get<std::string>(keyword);
}

float Settings::get_float_from_keyword(const std::string& keyword) {
    return boost::lexical_cast<float>(this->root.get<std::string>(keyword));
}

unsigned int Settings::get_uint_from_keyword(const std::string& keyword) {
    return boost::lexical_cast<unsigned int>(this->root.get<std::string>(keyword));
}

bool Settings::get_boolean_from_keyword(const std::string& keyword) {
    return this->root.get<std::string>(keyword).compare("true");
}

glm::vec3 Settings::rgb2vec3(const std::string& rgb) {
    const std::string r = rgb.substr(0,2);
    const std::string g = rgb.substr(2,2);
    const std::string b = rgb.substr(4,2);

    unsigned int rr = std::stoul("0x" + r, nullptr, 16);
    unsigned int gg = std::stoul("0x" + g, nullptr, 16);
    unsigned int bb = std::stoul("0x" + b, nullptr, 16);

    return glm::vec3((float)rr / 255.0f,
                     (float)gg / 255.0f,
                     (float)bb / 255.0f);
}
