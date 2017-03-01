/**************************************************************************
 *   screen.h  --  This file is part of Afelirin.                         *
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

#ifndef _SCREEN_H
#define _SCREEN_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

/**
 * @class Screen class
 *
 * @brief class handling screen settings
 *
 */
class Screen {
private:
    unsigned int width;       //!< width of the real screen
    unsigned int height;      //!< height of the real screen

    unsigned int resolution_x;      //!< width of the virtual screen
    unsigned int resolution_y;      //!< height of the virtual screen

    bool flag_focus;          //!< whether screen is in focus

public:
    /**
     * @fn          get
     *
     * @brief       get a reference to the screen
     *
     * @return      reference to the screen object (singleton pattern)
     */
    static Screen& get() {
        static Screen screen_instance;
        return screen_instance;
    }

    inline unsigned int get_width() const {
        return this->width;
    }

    inline unsigned int get_height() const {
        return this->height;
    }

    inline unsigned int get_resolution_x() const {
        return this->resolution_x;
    }

    inline unsigned int get_resolution_y() const {
        return this->resolution_y;
    }

    inline void set_width(unsigned int _width) {
        this->width = _width;
    }

    inline void set_height(unsigned int _height) {
        this->height = _height;
    }

    inline void set_resolution_x(unsigned int _resolution_x) {
        this->resolution_x = _resolution_x;
    }

    inline void set_resolution_y(unsigned int _resolution_y) {
        this->resolution_y = _resolution_y;
    }

    inline float get_aspect_ratio_screen() const {
        return (float)this->width / (float)this->height;
    }

    inline float get_aspect_ratio_resolution() const {
        return (float)this->resolution_x / (float)this->resolution_y;
    }

    inline bool in_focus() const {
        return this->flag_focus;
    }

    inline void set_focus(bool focus) {
        this->flag_focus = focus;
    }

private:
    Screen();

    // Singleton pattern
    Screen(Screen const&)          = delete;
    void operator=(Screen const&)  = delete;
};

#endif //_SCREEN_H
