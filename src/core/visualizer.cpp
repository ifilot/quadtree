/**************************************************************************
 *   visualizer.cpp  --  This file is part of Quadtree.                   *
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

#include "visualizer.h"

/**
 * @fn run method
 * @brief Constructs a new game
 *
 * In the run method, a while loop is started wherein the game state is
 * propagated over time.
 *
 * @return void
 */
void Visualizer::run(int argc, char* argv[]) {
    if(argc > 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
    }

    /**
     * @var num_frames
     * @brief Counts the number of frames in a single second
     */
    this->num_frames = 0;

    /**
     * @var last_time
     * @brief The time since the execution of the program after the
     *        last frame call.
     */
    double last_time = glfwGetTime();

    /* while the display runs, do at every frame */
    while(!Display::get().is_closed()) {

        /**
         * @var current_time
         * @brief The time since the execution of the program before the
         *        new integration step(s)
         */
        const double current_time = glfwGetTime();

        /* add passed time to the accumulator */
        this->accumulator += current_time - frame_start;

        /* keep integrating the game state until the accumulator
           is sufficiently depleted that a frame can be rendered */
        while(accumulator >= dt) {
            this->update(dt);
            this->accumulator -= dt;
        }

        /* calculate the FPS at every second and update the window
           title accordingly */
        if(current_time - last_time >= 1.0) {

            std::string sfps = boost::lexical_cast<std::string>(this->num_frames);
            Display::get().set_window_title("Quadtree");

            // place in the function any commands that should be done every second
            this->update_second();

            /* reset counters */
            last_time = glfwGetTime();
            this->num_frames = 0;
        }

        /* set the start time of the frame rendering */
        this->frame_start = current_time;

        /* render the game state */
        this->num_frames++;            /* increment frame counter */

        // perform pre-drawing operations
        this->pre_draw();

        // start drawing from here
        this->draw();

        // perform post-drawing operations (post processing)
        this->post_draw();
    }
}

/**
 * @fn handle_key_down
 * @brief Handles keyboard input
 *
 * Takes key presses as input and adjusts the game state accordingly.
 *
 * @param key the keyboard key
 * @param scancode the scancode
 * @param action the keyboard action (key down, key release)
 * @param mods
 *
 * @return void
 */
void Visualizer::handle_key_down(int key, int scancode, int action, int mods) {
    if(key == 'Q'  && mods & GLFW_MOD_CONTROL
                   && mods & GLFW_MOD_SHIFT
                   && mods & GLFW_MOD_ALT
                   && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(Display::get().get_window_ptr(), GL_TRUE);
    } else {
        // parse keys to the game engine
    }

}

/**
 * @fn handle_mouse_key_down
 * @brief Handles mouse input
 *
 * Takes key presses as input and adjusts the game state accordingly.
 *
 * @param button the mouse button
 * @param action the mouse action
 * @param mods
 *
 * @return void
 */
void Visualizer::handle_mouse_key_down(int button, int action, int mods) {
    if(action == GLFW_RELEASE) {
        float x = (float)Mouse::get().get_x_sw() / (float)Screen::get().get_width();
        float y = (float)Mouse::get().get_y_sw() / (float)Screen::get().get_height();
        Field::get().add_point(x, y);
    }
}

void Visualizer::handle_mouse_cursor(double xpos, double ypos) {
    Mouse::get().set_cursor(xpos, ypos);
}

void Visualizer::handle_scroll(double xoffset, double yoffset) {

}

void Visualizer::handle_char_callback(unsigned int key) {

}

/**
 * @fn Game method
 * @brief Game constructor method
 *
 * Loads up the display and initializes all entities.
 *
 * @return Game class
 */
Visualizer::Visualizer():
    accumulator(0.0),       /* default accumulator should be zero */
    fps(60.0),              /* set the target framerate */
    num_frames(0) {

    this->angle = 0.0;

    /* calculate the time interval */
    this->dt = 1.0 / fps;

    /* set the time at the frame start */
    this->frame_start = glfwGetTime();

    /* make sure the display is loaded before loading the shader */
    Display::get();

    // load PostProcessor
    PostProcessor::get();

    // Load any fonts
    FontWriter::get().add_font("./assets/fonts/retro.ttf",
                                14,              // font size
                                0.43f, 0.25f,    // sdf settings
                                32,             // start char
                                222             // number of chars
                                );
}

/**
 * @fn update method
 * @brief Handles time integration
 *
 * Function handling time propagation
 *
 * @param dt Time integration constant
 * @return Game class
 */
void Visualizer::update(double dt) {

}

void Visualizer::update_second() {

}

/**
 * @brief Setup drawing state
 *
 * Activates all draw classes and binds the frame buffer for the MSAA
 *
 */
void Visualizer::pre_draw() {
    Screen::get().set_focus(glfwGetWindowAttrib(Display::get().get_window_ptr(), GLFW_FOCUSED));
    Display::get().open_frame();   /* start new frame */

    // first create a texture map
    Light::get().update();

    // then perform the normal rendering in the PostProcessor texture
    PostProcessor::get().bind_frame_buffer();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, Screen::get().get_resolution_x(), Screen::get().get_resolution_y());
    Camera::get().update();
}

/**
 * @brief Draw call
 *
 * Draw all sprites on the screen
 *
 */
void Visualizer::draw() {
    Field::get().draw();
    //FontWriter::get().write_text(0, 50.f, 50.f, 0.f, glm::vec3(1,1,1), "Quadtree");
}

/**
 * @brief Post draw routine
 *
 * Unbind frame buffer, render MSAA and draw the GUI
 *
 */
void Visualizer::post_draw() {
    PostProcessor::get().unbind_frame_buffer();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glViewport(0, 0, Screen::get().get_width(), Screen::get().get_height());
    PostProcessor::get().draw();

    Display::get().close_frame();
}
