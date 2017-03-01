/**************************************************************************
 *   shader.h  --  This file is part of Afelirin.                         *
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

#ifndef _SHADER_H
#define _SHADER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>

#include "core/asset_manager.h"
#include "core/camera.h"

/**
 * @class ShaderUniform class
 * @brief Holds variable that is the same for every vertex in the shader
 */
class ShaderUniform {
public:
    /**
     * @brief       default constructor
     *
     * @param       type of the uniform (see enum below)
     * @param       name of the uniform
     * @param       number of uniforms that this class holds
     *
     * @return      ShaderUniform object
     */
    ShaderUniform(unsigned int _type, const std::string& _name, unsigned int _size);

    /**
     * @brief       set the value of the uniform
     *
     * @param       pointer to the variable
     *
     */
    void specify_value(void* val);

    void bind_uniform(GLuint program_id);

    /**
     * @brief       get the type of the uniform
     *
     * @return      uniform type
     */
    inline const std::string& get_name() const {
        return this->name;
    }

    /**
     * @brief       get the type of the uniform
     *
     * @return      uniform type
     */
    inline unsigned int get_type() const {
        return this->type;
    }

    /**
     * @brief       get the size of the uniform
     *
     * @return      uniform type
     */
    inline unsigned int get_size() const {
        return this->size;
    }

    inline void set_id(GLuint id) {
        this->uniform_id = id;
    }

    inline unsigned int get_id() const {
        return this->uniform_id;
    }

    enum {
        MAT4,
        MAT3,
        VEC4,
        VEC3,
        VEC2,
        TEXTURE,
        UINT,
        FLOAT,
        FRAME_MATRIX,
        OFFSET_MATRIX,

        NUM_VAR_TYPES
    };

private:
    unsigned int type;                           //<! type of the uniform
    std::string name;                            //<! name of the uniform
    unsigned int size;                           //<! size of the uniform
    GLuint uniform_id;                           //<! id of the uniform
};

/**
 * @class ShaderAttribute class
 * @brief Holds an attribute for the vertex
 */
class ShaderAttribute {
public:
    /**
     * @brief       default constructor
     *
     * @param       type of the attribute (see enum below)
     * @param       name of the attribute
     *
     * @return      ShaderAttribute object
     */
    ShaderAttribute(unsigned int _type, const std::string& _name);

    enum {
        POSITION,
        NORMAL,
        COLOR,
        TEXTURE_COORDINATE,
        WEIGHT,

        NUM_ATTR_TYPES
    };

    /**
     * @brief       get the name of the attribute
     *
     * @return      attribute name
     */
    inline const std::string& get_name() const {
        return this->name;
    }

    /**
     * @brief       get the type of the attribute
     *
     * @return      attribute type
     */
    inline unsigned int get_type() const {
        return this->type;
    }

private:
    unsigned int type;                           //<! type of the attribute
    std::string name;                            //<! name of the attribute
};

class Shader{
private:
    static const unsigned int NUM_SHADERS = 2;      //!< set number of shaders
    Shader(const Shader& other) = delete;           //!< initialization constructor
    void operator=(const Shader& other) = delete;   //!< copy constructor

    GLuint m_program;                               //!< reference pointer to the program
    GLuint m_shaders[NUM_SHADERS];                  //!< reference array to the shaders

    std::vector<ShaderAttribute> shader_attributes; //!< vector holding shader attributes

    std::unordered_map<std::string, ShaderUniform> shader_uniforms; //!< unordered map holding the uniforms

    //std::vector<ShaderUniform> shader_uniforms;     //!< vector holding shader uniforms
    //std::vector<GLuint> m_uniforms;                 //!< reference array to the uniforms

    bool flag_loaded;                               //!< flag whether this shader is loaded
    GLuint texture_id;                              //!< id of the texture
    std::string filename;                           //!< absolute path of the file that contains this shader program

public:
    Shader(const std::string& _filename);

    void add_uniform(unsigned int type, const std::string& name, unsigned int size);

    void add_attribute(unsigned int type, const std::string& name);

    void bind_uniforms_and_attributes();

    void set_uniform(const std::string& name, const void* val);

    inline long unsigned int get_nr_attributes() const {
        return this->shader_attributes.size();
    }

    inline long unsigned int get_nr_uniforms() const {
        return this->shader_uniforms.size();
    }

    inline void link_shader() {
        glUseProgram(this->m_program);
    }

    inline void unlink_shader() {
        glUseProgram(0);
    }

    inline bool is_loaded() const {
        return flag_loaded;
    }

    virtual ~Shader();

private:
    const ShaderUniform& get_uniform(const std::string& name) const;
};

#endif //_SHADER_H
