/**************************************************************************
 *   asset_manager.cpp  --  This file is part of Afelirin.                *
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

#include "asset_manager.h"

/**
 * @brief       default constructor
 *
 * @return      AssetManager instance
 */
AssetManager::AssetManager() {
    // nothing to load
}

/**
 * @brief       initialize the path variables
 *
 * This function should be called at the start of the program. From there on,
 * you can use the Singleton pattern of this class to get the absolute path
 * to the assets from anywhere in the program.
 *
 * @return      void
 */
void AssetManager::init(const char* argv0) {
    this->execution_directory = executable_path(argv0);

    boost::filesystem::path p(this->execution_directory.c_str());
    boost::filesystem::path dir = p.parent_path();
    this->root_directory = boost::filesystem::canonical(dir).branch_path().string() + "/";
}
