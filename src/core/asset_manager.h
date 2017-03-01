/**************************************************************************
 *   asset_manager.h  --  This file is part of Afelirin.                  *
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

#ifndef _ASSET_MANAGER_H
#define _ASSET_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <string>

/**
 * @class AssetManager class
 * @brief class that finds absolute directory on hard drive to get assets from
 *
 * Load this class in the beginning of the program by calling right after the main function:
 *
 * AssetManager::get().init(argc[0])
 *
 */

class AssetManager{
private:
    std::string execution_directory;    //!< absolute path to folder where the executable resides
    std::string root_directory;         //!< absolute path to the root folder of the program

public:
    /**
     * @brief       get a reference to the AssetManager object
     *
     * @return      reference to the AssetManager object (singleton pattern)
     */
    static AssetManager& get() {
        static AssetManager am_instance;
        return am_instance;
    }

    /**
     * @brief       get the execution directory
     *
     * @return      absolute path of the execution directory
     */
    const std::string& get_execution_directory() const {
        return this->execution_directory;
    }

    /**
     * @brief       get the root directory
     *
     * @return      absolute path of the root directory
     */
    const std::string& get_root_directory() const {
        return this->root_directory;
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
    void init(const char* argv0);

private:
    /**
     * @brief       default constructor
     *
     * @return      AssetManager instance
     */
    AssetManager();

    std::string executable_path_fallback(const char *argv0) {
        if (argv0 == nullptr || argv0[0] == 0) {
            return "";
        }
        boost::system::error_code ec;
        boost::filesystem::path p(
            boost::filesystem::canonical(
                argv0, boost::filesystem::current_path(), ec));
        return p.make_preferred().string();
    }

    #if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS) // {

    std::string executable_path(const char *argv0) {
        return executable_path_fallback(argv0);
    }

    #elif (BOOST_OS_MACOS) // } {

    std::string executable_path(const char *argv0) {
        return executable_path_fallback(argv0);
    }

    #elif (BOOST_OS_SOLARIS) // } {

    #include <stdlib.h>

    std::string executable_path(const char *argv0) {
        std::string ret = getexecname();
        if (ret.empty()) {
            return executable_path_fallback(argv0);
        }

        boost::filesystem::path p(ret);
        if (!p.has_root_directory()) {
            boost::system::error_code ec;
            p = boost::filesystem::canonical(
                p, boost::filesystem::current_path(), ec);
            ret = p.make_preferred().string();
        }
        return ret;
    }

    #elif (BOOST_OS_BSD) // } {

    #include <sys/sysctl.h>

    std::string executable_path(const char *argv0) {
        int mib[4] = {0};
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_PATHNAME;
        mib[3] = -1;
        char buf[1024] = {0};
        size_t size = sizeof(buf);
        sysctl(mib, 4, buf, &size, NULL, 0);

        if (size == 0 || size == sizeof(buf)) {
            return executable_path_fallback(argv0);
        }

        std::string path(buf, size);
        boost::system::error_code ec;
        boost::filesystem::path p(
            boost::filesystem::canonical(
                path, boost::filesystem::current_path(), ec));
        return p.make_preferred().string();
    }

    #elif (BOOST_OS_LINUX) // } {

    #include <unistd.h>

    std::string executable_path(const char *argv0) {
        char buf[1024] = {0};
        ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
        if (size == 0 || size == sizeof(buf)) {
            return executable_path_fallback(argv0);
        }
        std::string path(buf, size);
        boost::system::error_code ec;
        boost::filesystem::path p(
            boost::filesystem::canonical(
                path, boost::filesystem::current_path(), ec));
        return p.make_preferred().string();
    }

    #else // } {

    std::string executable_path(const char *argv0) {
        return executable_path_fallback(argv0);
    }

    #endif // }

    AssetManager(AssetManager const&)          = delete;
    void operator=(AssetManager const&)  = delete;
};

#endif //_ASSET_MANAGER_H
