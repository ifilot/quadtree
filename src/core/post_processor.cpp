/**************************************************************************
 *   post_processor.cpp  --  This file is part of Afelirin.               *
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

#include "post_processor.h"

/**
 * @brief       post_processor constructor
 *
 * @return      post_processor instance
 */
PostProcessor::PostProcessor() : texture_slot(POSTPROCESSOR_TEXTURE_SLOT) {
    this->msaa = 4;
    this->filter_flags = 0x00000000;

    this->load_mesh();

    glActiveTexture(GL_TEXTURE2);

    // msaa buffer
    this->create_msaa_buffer(&this->depth_msaa, &this->texture_msaa, &this->frame_buffer_msaa);

    // primary buffer
    this->create_buffer(&this->depth_p, &this->texture_p, &this->frame_buffer_p);

    // secondary buffer
    this->create_buffer(&this->depth_s, &this->texture_s, &this->frame_buffer_s);

    this->create_shader(&this->shader_default, "assets/shaders/postproc");
    this->create_shader(&this->shader_invert, "assets/filters/invert");

    // set blur shaders
    const float blur_radius = 2.0f;
    this->create_shader(&this->shader_blur_h, "assets/filters/blur");
    const float width = (float)Screen::get().get_resolution_x();
    this->shader_blur_h->set_uniform("resolution", &width);
    this->shader_blur_h->set_uniform("radius", &blur_radius);
    this->shader_blur_h->set_uniform("dir", &glm::vec2(1,0)[0]);

    this->create_shader(&this->shader_blur_v, "assets/filters/blur");
    const float height = (float)Screen::get().get_resolution_y();
    this->shader_blur_v->set_uniform("resolution", &height);
    this->shader_blur_v->set_uniform("radius", &blur_radius);
    this->shader_blur_v->set_uniform("dir", &glm::vec2(0,1)[0]);

    // unbind vertex array
    glBindVertexArray(0);
}

/**
 * @brief      bind the msaa frame buffer
 */
void PostProcessor::bind_frame_buffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer_msaa);
    glEnable(GL_MULTISAMPLE);
    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE && (Screen::get().get_resolution_x() != 0 || Screen::get().get_resolution_y() != 0)) {
        std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
        std::cerr << __FILE__ << "(" << __LINE__ << ")" << std::endl;
    }

    this->frame_buffer_active = this->frame_buffer_p;
    this->texture_active = this->texture_p;
    this->depth_active = this->depth_p;

    this->frame_buffer_passive = this->frame_buffer_s;
    this->texture_passive = this->texture_s;
    this->depth_passive = this->depth_s;
}

/**
 * @brief      unbind all frame buffers
 */
void PostProcessor::unbind_frame_buffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief      draw the result of the post processing to the screen
 */
void PostProcessor::draw() {
    // resample the buffer from msaa to regular
    this->resample_buffer();

    // apply filtering operations
    this->apply_filters();

    // render the output to the screen
    this->render(this->shader_default);
}

/**
 * @brief      updates all render buffers and textures
 */
void PostProcessor::window_reshape() {
    this->set_msaa_buffer(this->texture_msaa, this->depth_msaa);
    this->set_buffer(this->texture_p, this->depth_p);
    this->set_buffer(this->texture_s, this->depth_s);

    this->shader_blur_h->link_shader();
    const float width = Screen::get().get_resolution_x() != 0 ? (float)Screen::get().get_resolution_x() : 1.0f;
    this->shader_blur_h->set_uniform("resolution", &width);

    this->shader_blur_h->link_shader();
	const float height = Screen::get().get_resolution_y() != 0 ? (float)Screen::get().get_resolution_y() : 1.0f;
    this->shader_blur_v->set_uniform("resolution", &height);
}

/**
 * @brief      class destructor
 */
PostProcessor::~PostProcessor() {
    glDeleteRenderbuffers(1, &this->depth_p);
    glDeleteTextures(1, &this->texture_p);
    glDeleteFramebuffers(1, &this->frame_buffer_p);

    glDeleteRenderbuffers(1, &this->depth_s);
    glDeleteTextures(1, &this->texture_s);
    glDeleteFramebuffers(1, &this->frame_buffer_s);

    glDeleteRenderbuffers(1, &this->depth_msaa);
    glDeleteTextures(1, &this->texture_msaa);
    glDeleteFramebuffers(1, &this->frame_buffer_msaa);

    glDeleteBuffers(2, this->vbo);
    glDeleteVertexArrays(1, &this->vao);
}

/**
 * @brief      blit the content of the msaa fbo to the primary fbo
 */
void PostProcessor::resample_buffer() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->frame_buffer_msaa);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frame_buffer_p);

    const unsigned int width = Screen::get().get_resolution_x() != 0 ? Screen::get().get_resolution_x() : 1;
    const unsigned int height = Screen::get().get_resolution_y() != 0 ? Screen::get().get_resolution_y() : 1;

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief      perform series of filter passes on the active texture
 */
void PostProcessor::apply_filters() {
    if(this->filter_flags & FILTER_BLUR) {
        this->blur();
    }

    if(this->filter_flags & FILTER_INVERT) {
        this->pass(this->shader_invert);
    }
}

/**
 * @brief      perform single filter pass on the active texture
 *
 * @param[in]   pointer to the Shader class containing the filter
 */
void PostProcessor::pass(const std::unique_ptr<Shader>& shader) {
    //set buffer (draw to passive buffer)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->frame_buffer_passive);

    // perform draw
    this->render(shader);

    // unset buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // swap buffers
    this->swap_active_buffer();
}

/**
 * @brief      perform blurring
 */
void PostProcessor::blur() {
    this->pass(this->shader_blur_h);
    this->pass(this->shader_blur_v);
}

/**
 * @brief      swap passive and active buffers
 */
void PostProcessor::swap_active_buffer() {
    if(this->frame_buffer_active == this->frame_buffer_p) {

        this->frame_buffer_active = this->frame_buffer_s;
        this->texture_active = this->texture_s;
        this->depth_active = this->depth_s;

        this->frame_buffer_passive = this->frame_buffer_p;
        this->texture_passive = this->texture_p;
        this->depth_passive = this->depth_p;

    } else {

        this->frame_buffer_active = this->frame_buffer_p;
        this->texture_active = this->texture_p;
        this->depth_active = this->depth_p;

        this->frame_buffer_passive = this->frame_buffer_s;
        this->texture_passive = this->texture_s;
        this->depth_passive = this->depth_s;
    }
}

/**
 * @brief      perform a texture render (used for the filters)
 */
void PostProcessor::render(const std::unique_ptr<Shader>& shader) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->texture_active);

    shader->link_shader();

    // calculate scaling
    float sx, sy;

    // try letterbox mode
    float scale = (float)Screen::get().get_width() / (float)Screen::get().get_resolution_x();
    unsigned int height = scale * (float)Screen::get().get_resolution_y();

    if(height < Screen::get().get_height()) { // use letterbox mode
        sx = 1.0f;
        sy = Screen::get().get_aspect_ratio_screen() / Screen::get().get_aspect_ratio_resolution();
    } else {
        sx = Screen::get().get_aspect_ratio_resolution() / Screen::get().get_aspect_ratio_screen();
        sy = 1.0f;
    }

    const glm::mat4 mvp = glm::scale(glm::vec3(sx, sy, 1.0f));

    // set shader uniforms
    shader->set_uniform("text", &this->texture_slot); // set texture id
    shader->set_uniform("mvp", &mvp[0][0]);

    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
}

/**
 * @brief      create the multisampling fbo
 */
void PostProcessor::create_msaa_buffer(GLuint* render_buffer, GLuint* texture, GLuint* frame_buffer) {
    const unsigned int width = Screen::get().get_resolution_x();
    const unsigned int height = Screen::get().get_resolution_y();

	glGenRenderbuffers(1, render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *render_buffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, this->msaa, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->msaa, GL_RGBA, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glGenFramebuffers(1, frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, *frame_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *render_buffer);
    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief      create the renderbuffer objects
 */
void PostProcessor::create_buffer(GLuint* render_buffer, GLuint* texture, GLuint* frame_buffer) {
    const unsigned int width = Screen::get().get_resolution_x();
    const unsigned int height = Screen::get().get_resolution_y();

    glGenRenderbuffers(1, render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    // set minmag filters (GL_NEAREST is faster, GL_LINEAR is higher quality)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, *frame_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *render_buffer);

    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief      set width and height to msaa buffers
 */
void PostProcessor::set_msaa_buffer(GLuint texture, GLuint frame_buffer) {
    // resize regular buffer
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->msaa, GL_RGBA, Screen::get().get_resolution_x(), Screen::get().get_resolution_y(), GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, frame_buffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, this->msaa, GL_DEPTH24_STENCIL8, Screen::get().get_resolution_x(), Screen::get().get_resolution_y());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/**
 * @brief      set with and height to regular buffers
 */
void PostProcessor::set_buffer(GLuint texture, GLuint frame_buffer) {
    const unsigned int width = Screen::get().get_resolution_x();
    const unsigned int height = Screen::get().get_resolution_y();

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, frame_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, Screen::get().get_resolution_x(), Screen::get().get_resolution_y());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/**
 * @brief      create a shader for filtering
 */
void PostProcessor::create_shader(std::unique_ptr<Shader>* shader, const std::string& filename) {
    *shader = std::unique_ptr<Shader>(new Shader(filename));
    shader->get()->add_attribute(ShaderAttribute::POSITION, "position");
    shader->get()->add_uniform(ShaderUniform::TEXTURE, "text", 1);
    shader->get()->add_uniform(ShaderUniform::MAT4, "mvp", 1);

    if(filename.compare("assets/filters/blur") == 0) {
        shader->get()->add_uniform(ShaderUniform::FLOAT, "resolution", 1);
        shader->get()->add_uniform(ShaderUniform::FLOAT, "radius", 1);
        shader->get()->add_uniform(ShaderUniform::VEC2, "dir", 1);
    }

    glBindVertexArray(this->vao);
    shader->get()->bind_uniforms_and_attributes();
    glBindVertexArray(0);
}

void PostProcessor::load_mesh() {
    std::vector<glm::vec3> positions;
    std::vector<unsigned int> indices;

    positions.push_back(glm::vec3(-1, -1, 0));
    positions.push_back(glm::vec3( 1, -1, 0));
    positions.push_back(glm::vec3( 1,  1, 0));
    positions.push_back(glm::vec3(-1,  1, 0));
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(2, this->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), &positions[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}
