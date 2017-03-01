/**************************************************************************
 *   mouse.h  --  This file is part of Afelirin.                          *
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

#ifndef _MOUSE_H
#define _MOUSE_H

#include <iostream>
#include <memory>
#include <algorithm>

#include "core/camera.h"
#include "core/screen.h"
#include "core/shader.h"

/**
 * @class Mouse class
 *
 * @brief class handling mouse actions such as raycasting
 *
 */
class Mouse {
private:
    glm::vec2 cur_pos;                    //!< current cursor position
    glm::vec2 cur_pos_sw;                 //!< current cursor position with origin in SW position

public:
    /**
     * @fn          get
     *
     * @brief       get a reference to the mouse
     *
     * @return      reference to the mouse object (singleton pattern)
     */
    static Mouse& get() {
        static Mouse mouse_instance;
        return mouse_instance;
    }

    /*
     * @brief Draw action so the mouse
     */
    void draw();

    inline const glm::vec2& get_cursor() const {
        return this->cur_pos;
    }

    inline const glm::vec2& get_cursor_sw() const {
        return this->cur_pos_sw;
    }

    inline float get_x_sw() const {
        return this->cur_pos_sw[0];
    }

    inline float get_y_sw() const {
        return this->cur_pos_sw[1];
    }

    /*
     * @brief Update the current cursor position
     *
     * @param[in] xpos      current x position of the cursor on the screen
     * @param[in] xpos      current y position of the cursor on the screen
     */
    inline void set_cursor(double xpos, double ypos) {
        this->cur_pos[0] = (float)xpos;
        this->cur_pos[1] = (float)ypos;
        this->cur_pos_sw[0] = (float)xpos;
        this->cur_pos_sw[1] = (float)Screen::get().get_height() - (float)ypos;
    }

private:
    /**
     * @brief       mouse constructor
     */
    Mouse();

    Mouse(Mouse const&)          = delete;
    void operator=(Mouse const&)  = delete;
};

#endif //_MOUSE_H
