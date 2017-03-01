/**************************************************************************
 *   camera.h  --  This file is part of Afelirin.                         *
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

#ifndef _CAMERA_H
#define _CAMERA_H

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <algorithm>

#include "screen.h"

/**
 * @class Camera class
 * @brief class handling the camera
 */
class Camera {
private:
    glm::mat4 projection;               //!< perspective matrix
    glm::mat4 view;                     //!< view matrix

    glm::vec2 position;                 //!< position of the camera in world space

    float aspect_ratio;                     //!< aspect ratio of the window

public:

    /**
     * @brief       get a reference to the camera object
     *
     * @return      reference to the camera object (singleton pattern)
     */
    static Camera& get() {
        static Camera camera_instance;
        return camera_instance;
    }

    //*************************
    // GETTERS
    //*************************

    /**
     * @brief       get current view matrix
     *
     * @return      view matrix
     */
    inline const glm::mat4& get_view() const {
        return this->view;
    }

    /**
     * @brief       get current projection matrix
     *
     * @return      projection matrix
     */
    inline const glm::mat4& get_projection() const {
        return this->projection;
    }

    /**
     * @brief       get current camera position
     *
     * @return      position
     */
    inline const glm::vec2& get_position() const {
        return this->position;
    }

    //*************************
    // SETTERS
    //*************************

    /**
     * @brief       set the camera aspect ratio
     *
     * @param       aspect ratio
     * @return      void
     */
    inline void set_aspect_ratio(float _aspect_ratio) {
        this->aspect_ratio = _aspect_ratio;
    }

    /**
     * @brief       set the projection matrix
     *
     * @param       projection matrix
     * @return      void
     */
    inline void set_projection(const glm::mat4& _projection) {
        this->projection = _projection;
    }

    /**
     * @brief       update the camera perspective matrix
     *
     * @return      void
     */
    void update();

    /**
     * @brief       translate the camera in the clock-wise direction
     *
     * @return      void
     */
    void translate(const glm::vec3& trans);

    /**
     * @brief      set camera position and up direction
     *
     * @param      camera position
     * @param      up direction
     * @return     void
     */
    void set_camera_position(const glm::vec3& _position, const glm::vec3& _up);

private:
    /**
     * @brief       camera constructor
     *
     * @return      camera instance
     */
    Camera();

    Camera(Camera const&)          = delete;
    void operator=(Camera const&)  = delete;
};

#endif // _CAMERA_H
