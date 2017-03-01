/**************************************************************************
 *   shader.cpp  --  This file is part of Afelirin.                       *
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

#include "shader.h"

// create an empty shader
static GLuint create_shader(const std::string &text, GLenum shader_type);

// load a shader from a file
static std::string load_shader(const std::string& filename);

// check the shader for errors
static void check_shader_error(GLuint shader, GLuint flag, bool is_program, const std::string& error_message);

/**
 * @brief       default constructor
 *
 * @param       type of the uniform (see enum below)
 * @param       name of the uniform
 * @param       number of uniforms that this class holds
 *
 * @return      ShaderUniform object
 */
ShaderUniform::ShaderUniform(unsigned int _type, const std::string& _name, unsigned int _size) {
    this->type = _type;
    this->name = _name;
    this->size = _size;
}

/**
 * @brief       default constructor
 *
 * @param       type of the attribute (see enum below)
 * @param       name of the attribute
 *
 * @return      ShaderAttribute object
 */
ShaderAttribute::ShaderAttribute(unsigned int _type, const std::string& _name) {
    this->type = _type;
    this->name = _name;
}

Shader::Shader(const std::string& _filename) {
    // constructs the program
    this->m_program = glCreateProgram();

    this->filename = AssetManager::get().get_root_directory() + _filename;

    // set vertex shader
    this->m_shaders[0] = create_shader(load_shader(this->filename + ".vs"), GL_VERTEX_SHADER);

    // set fragment shader
    this->m_shaders[1] = create_shader(load_shader(this->filename + ".fs"), GL_FRAGMENT_SHADER);

    // attach all shaders
    for(unsigned int i = 0; i < NUM_SHADERS; i++) {
        glAttachShader(this->m_program, m_shaders[i]);
    }

    this->flag_loaded = false;
    this->texture_id = 0;
}

void Shader::add_uniform(unsigned int type, const std::string& name, unsigned int size) {
    this->shader_uniforms.emplace(name, ShaderUniform(type, name, size));
}

void Shader::add_attribute(unsigned int type, const std::string& name) {
    this->shader_attributes.push_back(ShaderAttribute(type, name));
}

void Shader::bind_uniforms_and_attributes() {
    for(unsigned int i=0; i<this->shader_attributes.size(); i++) {
        glBindAttribLocation(this->m_program, i, shader_attributes[i].get_name().c_str());
    }

    // links program and checks for errors
    glLinkProgram(this->m_program);
    check_shader_error(m_program, GL_LINK_STATUS, true, "Error in \"" + this->filename + "\" : Program linking failed: ");

    // validates program
    glValidateProgram(this->m_program);
    check_shader_error(m_program, GL_VALIDATE_STATUS, true, "Error in \"" + this->filename + "\" : Program validation failed: ");

    glUseProgram(this->m_program);

    this->flag_loaded = true;
}

void Shader::set_uniform(const std::string& name, const void* val) {
    // get uniform location in program
    const GLint id = glGetUniformLocation(this->m_program, name.c_str());
    if(id == -1) {
        std::cerr << name << " does not correspond with a known uniform in this program." << std::endl;
        exit(-1);
    }

    const ShaderUniform& uni = this->get_uniform(name);

    // get uniform load function based on type
    switch(uni.get_type()) {
        case ShaderUniform::MAT4:
            glUniformMatrix4fv(id, uni.get_size(), GL_FALSE, (const GLfloat*)val);
        break;
        case ShaderUniform::MAT3:
            glUniformMatrix3fv(id, uni.get_size(), GL_FALSE, (const GLfloat*)val);
        break;
        case ShaderUniform::VEC4:
            glUniform4fv(id, uni.get_size(), (const GLfloat*)val);
        break;
        case ShaderUniform::VEC3:
            glUniform3fv(id, uni.get_size(), (const GLfloat*)val);
        break;
        case ShaderUniform::VEC2:
            glUniform2fv(id, uni.get_size(), (const GLfloat*)val);
        break;
        case  ShaderUniform::TEXTURE:
            glUniform1i(id, *(const int*)(val));
        break;
        case  ShaderUniform::FLOAT:
            glUniform1f(id, *(const float*)(val));
        break;
        case  ShaderUniform::FRAME_MATRIX:
            glUniformMatrix4fv(id, uni.get_size(), GL_FALSE, (const GLfloat*)val);
        break;
        case  ShaderUniform::OFFSET_MATRIX:
            glUniformMatrix4fv(id, uni.get_size(), GL_FALSE, (const GLfloat*)val);
        break;
        default:
            // do nothing
        break;
    }
}

const ShaderUniform& Shader::get_uniform(const std::string& name) const {
    auto got = this->shader_uniforms.find(name);

    if(got == this->shader_uniforms.end()) {
        std::cerr << "Invalid uniform name: " << name << std::endl;
        exit(-1);
    } else {
        return got->second;
    }
}

/*
 * destructor function
 * Handles shape deconstruction
 */
Shader::~Shader() {
    // detach and delete the allocated shaders
    for(unsigned int i = 0; i < NUM_SHADERS; i++) {
        glDetachShader(this->m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }

    // finally delete the program
    glDeleteProgram(this->m_program);
}

static GLuint create_shader(const std::string& text, GLenum shader_type) {
    GLuint shader = glCreateShader(shader_type);

    if(shader == 0) {
        std::cerr << "Error: Shader creation failed!" << std::endl;
    }

    const GLchar* shader_source_strings[1];
    GLint shader_source_strings_length[1];

    shader_source_strings[0] = text.c_str();
    shader_source_strings_length[0] = text.length();

    glShaderSource(shader, 1, shader_source_strings, shader_source_strings_length);
    glCompileShader(shader);

    check_shader_error(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed: ");

    return shader;
}

static std::string load_shader(const std::string& filename) {
    std::ifstream file;
    file.open((filename).c_str());

    std::string output;
    std::string line;

    if(file.is_open()) {
        while(file.good()) {
            getline(file, line);
            output.append(line + "\n");
        }
    } else {
        std::cerr << "Unable to load shader: " << filename << std::endl;
    }

    return output;
}

static void check_shader_error(GLuint shader, GLuint flag, bool is_program, const std::string& error_message) {
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(is_program) {
        glGetProgramiv(shader, flag, &success);
    } else {
        glGetShaderiv(shader, flag, &success);
    }

    if(success == GL_FALSE) {
        if(is_program) {
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        } else {
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);
        }

        std::cerr << error_message << ": '" << error << "'" << std::endl;
    }
}
