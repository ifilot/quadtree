/**************************************************************************
 *   display.cpp  --  This file is part of Afelirin.                      *
 *                                                                        *
 *   Copyright (C) 2016, Ivo Filot                                        *
 *                                                                        *
 *   QuadTree is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published    *
 *   by the Free Software Foundation, either version 3 of the License,    *
 *   or (at your option) any later version.                               *
 *                                                                        *
 *   QuadTree is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "display.h"

/**
 * @brief Display constructor
 *
 * Initializes the GLFW library, constructs a window and put it into context.
 * Callbacks are set-up and the GLEW library is initialized.
 *
 */
Display::Display() {
    // set the error callback
    glfwSetErrorCallback(error_callback);

    // initialize the library
    if(!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // set window hints
    //glfwWindowHint(GLFW_FLOATING, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);

    //
    if(Settings::get().get_boolean_from_keyword("settings.screen.full_screen")) {
        const unsigned int width = Settings::get().get_uint_from_keyword("settings.screen.resolution_x");
        const unsigned int height = Settings::get().get_uint_from_keyword("settings.screen.resolution_y");
        this->m_window = glfwCreateWindow(width, height, "QuadTree" , NULL, NULL);
    } else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        this->m_window = glfwCreateWindow(mode->width, mode->height, "QuadTree" , monitor, NULL);
    }

    // check if the window is properly constructed
    if (!this->m_window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // set the context for OpenGL
    glfwMakeContextCurrent(this->m_window);
    glfwSwapInterval(1);

    // set the key callback
    glfwSetKeyCallback(this->m_window, key_callback);

    // set mouse key callback
    glfwSetMouseButtonCallback(this->m_window, mouse_button_callback);

    // set framebuffer callback when a window is resized
	glfwSetFramebufferSizeCallback(this->m_window, framebuffer_size_callback);

    // set mouse cursor position callback
    glfwSetCursorPosCallback(this->m_window, mouse_cursor_callback);

    // set scroll callback
    glfwSetScrollCallback(this->m_window, scroll_callback);

    // set drop callback
    glfwSetDropCallback(this->m_window, drop_callback);

    // set character callback
    glfwSetCharCallback(this->m_window, this->char_callback);

    // initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Could not initialize GLEW" << std::endl;
    }

    // enable transparency
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // set line width
    glEnable(GL_LINE_SMOOTH);

    // disable cursor (we are going to use our own)
    //glfwSetInputMode(this->m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // configure camera dimensions
    int width, height;
    glfwGetWindowSize(this->m_window, &width, &height);
    Screen::get().set_width(width);
    Screen::get().set_height(height);
    Screen::get().set_resolution_x(Settings::get().get_uint_from_keyword("settings.screen.resolution_x"));
    Screen::get().set_resolution_y(Settings::get().get_uint_from_keyword("settings.screen.resolution_y"));
    Camera::get().set_aspect_ratio(Screen::get().get_aspect_ratio_resolution());
    Camera::get().update();
    PostProcessor::get().window_reshape();
}

/**
 * @brief close frame function
 *
 * Perform these instructions at the start of each frame
 */
void Display::open_frame() {

}

/**
 * @brief open frame function
 *
 * Perform these instructions at the end of each frame
 */
void Display::close_frame() {
    glfwSwapBuffers(this->m_window);
    glfwPollEvents();
}

/*
 * @brief Checks if the window is closed and if so, terminates the program
 */
bool Display::is_closed() {
    return glfwWindowShouldClose(this->m_window);
}

/**
 * @brief Display destructor
 *
 * Destructs the display class and terminates the window and the glfw library
 */
Display::~Display() {
    glfwDestroyWindow(this->m_window);
    glfwTerminate();
}

 /*
 * @brief center the mouse pointer
 */
void Display::center_mouse_pointer() {
    glfwSetCursorPos(this->m_window, (float)Screen::get().get_width() / 2.0f, (float)Screen::get().get_height() / 2.0f);
}

/*
 * @brief center the mouse pointer
 *
 * @param window_name   the window name
 */
void Display::set_window_title(const std::string& window_name) {
    glfwSetWindowTitle(this->m_window, window_name.c_str());
}

/*
 * @brief get the position of the cursor
 *
 * @return the position of the cursor
 */
const glm::vec2 Display::get_cursor_position() const {
    double xpos, ypos;
    glfwGetCursorPos(this->m_window, &xpos, &ypos);
    return glm::vec2((float)xpos / (float)Screen::get().get_width(),
                     (float)ypos / (float)Screen::get().get_height());
}

/*
 * *********
 * CALLBACKS
 * *********
 */

/**
 * @brief error callback function
 *
 * @param error         error code
 * @param description   error description
 *
 */
void Display::error_callback(int error, const char* description) {
    std::cerr << description << std::endl;
}

/**
 * @brief Registers and handles key presses
 *
 * @param[in] window   pointer to window
 * @param[in] key      keyboard key
 * @param[in] scancode
 * @param[in] action
 * @param[in] mods
 *
 */
void Display::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Visualizer::get().handle_key_down(key, scancode, action, mods);
}

/**
 * @brief registers and handles mouse button presses
 *
 * @param[in] window   pointer to window
 * @param[in] button
 * @param[in] action
 * @param[in] mods
 *
 */
void Display::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Visualizer::get().handle_mouse_key_down(button, action, mods);
}

/**
 * @brief registers and handles mouse button presses
 *
 * @param[in] window   pointer to window
 * @param[in] xoffset  scroll wheel offset in x direction
 * @param[in] yoffset  scroll wheel offset in y direction
 *
 */
void Display::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Visualizer::get().handle_scroll(xoffset, yoffset);
}

/**
 * @brief handles file dropping in the window
 *
 * @param[in] window    pointer to window
 * @param[in] count     number of files
 * @param[in] paths     pathnames
 *
 */
void Display::drop_callback(GLFWwindow* window, int count, const char** paths) {

}

/**
 * @brief registers and handles mouse button presses
 *
 * @param[in] window    pointer to window
 * @param[in] xpos      x position of the mouse cursor on screen
 * @param[in] ypos      y position of the mouse cursor on screen
 *
 */
void Display::mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    Visualizer::get().handle_mouse_cursor(xpos, ypos);
}

/**
 * @brief parses input characters from keyboard
 *
 * @param[in] window    pointer to window
 * @param[in] window    key to parse
 *
 */
void Display::char_callback(GLFWwindow* window, unsigned int key) {
    Visualizer::get().handle_char_callback(key);
}

/**
 * @brief perform window resizing
 */
void Display::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // update screen settings
    Screen::get().set_width(width);
    Screen::get().set_height(height);

    // update camera settings
    Camera::get().set_aspect_ratio(Screen::get().get_aspect_ratio_resolution());
    Camera::get().update();

    // update post processor
    PostProcessor::get().window_reshape();
}
