/**************************************************************************
 *   camera.cpp  --  This file is part of Afelirin.                       *
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

#include "camera.h"

/**
 * @brief       update the camera perspective matrix
 *
 * @return      void
 */
void Camera::update() {
    this->projection = glm::ortho(0.0f, 1.0f * this->aspect_ratio, 0.0f, 1.0f, -300.0f, 300.0f);
    this->view = glm::lookAt(
                    glm::vec3(this->position, 1.0),              // cam pos
                    glm::vec3(this->position, 0.0),              // look at
                    glm::vec3(0,1,0)               // up
                );
}

/**
 * @brief       translate the camera in the clock-wise direction
 *
 * @return      void
 */
void Camera::translate(const glm::vec3& trans) {
    this->update();
}

/**
 * @brief      set camera position and up direction
 *
 * @param      camera position
 * @param      up direction
 * @return     void
 */
void Camera::set_camera_position(const glm::vec3& _position, const glm::vec3& _up) {
    this->update();
}

/**
 * @brief       camera constructor
 *
 * @return      camera instance
 */
Camera::Camera() {
    this->position = glm::vec2(0.0f, 0.0f);
    this->update();
}
