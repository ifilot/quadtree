/**************************************************************************
 *   visualizer.h  --  This file is part of Afelirin.                     *
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

#ifndef _VISUALIZER_H
#define _VISUALIZER_H

#include <boost/lexical_cast.hpp>

#include "core/display.h"
#include "core/mouse.h"
#include "core/shader.h"
#include "core/screen.h"
#include "core/font_writer.h"
#include "core/light.h"
#include "core/post_processor.h"

#include "quadtree/field.h"

/**
 * @class Visualizer class
 * @brief Initializes entities and handles time propagation.
 */
class Visualizer {
private:
    /**
     * @var accumulator
     * @brief Time accumulation tracker
     *
     * After each frame, the accumulator is incremented by the time corresponding
     * to the target frame rate. The accumulator can only be decremented by
     * performing update (integration) actions. This procedure guarantees that
     * the visualizer as the same pace independent of machine performance.
     */
    double accumulator;

    /**
     * @var frmae_start
     * @brief Current time after start of the execution
     */
    double frame_start;

    /**
     * @var dt
     * @brief Time between two frame intervals based on the target framerate.
     */
    double dt;

    /**
     * @var fps
     * @brief Target framerate
     */
    double fps;

    double angle;

    /**
     * @var state
     * @brief interface state important for handling input
     */
    unsigned int state;

    unsigned int num_frames;

public:
    /**
     * @fn Visualizer get
     * @brief Returns reference to Visualizer instance
     *
     * Static class member that returns a reference to a game instance.
     * This pattern is uses classic lazy evaluated and correctly destroyed singleton.
     * It is adapted from: http://stackoverflow.com/questions/1008019/c-singleton-design-pattern
     *
     * @return reference to game instance
     */
    static Visualizer& get() {
        static Visualizer instance;
        return instance;
    }

    /**
     * @fn run method
     * @brief Constructs a new game
     *
     * In the run method, a while loop is started wherin the game state is
     * propagated over time.
     *
     * @return void
     */
    void run(int argc, char* argv[]);

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
     * @return void
     */
    void handle_key_down(int key, int scancode, int action, int mods);

    /**
     * @fn handle_key_down
     * @brief Handles keyboard input
     *
     * Takes key presses as input and adjusts the game state accordingly.
     *
     * @param button the mouse button
     * @param action the mouse action
     * @param mods
     * @return void
     */
    void handle_mouse_key_down(int button, int action, int mods);

    /**
     * @fn handle_scroll
     * @brief handles mouse scrolls
     *
     * @return void
     */
    void handle_scroll(double xoffset, double yoffset);

    /**
     * @fn handle_scroll
     * @brief handles mouse scrolls
     *
     * @return void
     */
    void handle_mouse_cursor(double xpos, double ypos);

    /**
     * @fn handle character callback
     * @brief handles mouse scrolls
     *
     * @return void
     */
    void handle_char_callback(unsigned int key);

private:
    /**
     * @fn Visualizer method
     * @brief Visualizer constructor method
     *
     * Loads up the display and initializes all entities.
     *
     */
    Visualizer();

    /**
     * @fn update method
     * @brief Handles time integration
     *
     * Function handling time propagation
     *
     * @param dt Time integration constant
     */
    void update(double dt);

    /**
     * @brief Perform these actions at every second the program is running
     */
    void update_second();

    void pre_draw();

    void draw();

    void post_draw();

    /* Singleton pattern; the function below are deleted */
    Visualizer(Visualizer const&)          = delete;
    void operator=(Visualizer const&)  = delete;
};

#endif // _VISUALIZER_H
