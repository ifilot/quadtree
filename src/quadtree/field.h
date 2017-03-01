#ifndef _FIELD_H
#define _FIELD_H

#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/shader.h"
#include "quadtree.h"

class Point {
public:
    float x;
    float y;

    Point(float _x, float _y) : x(_x), y(_y) {}
};

class Field {
private:
    GLuint vao;
    GLuint vbo[2];
    std::unique_ptr<Shader> shader;
    std::vector<Point> points;
    QuadTree<Point> quadtree;

public:

    /**
     * @brief       get a reference to the camera object
     *
     * @return      reference to the camera object (singleton pattern)
     */
    static Field& get() {
        static Field field_instance;
        return field_instance;
    }

    void add_point(double x, double y);

    void draw();

private:
    /**
     * @brief       camera constructor
     *
     * @return      camera instance
     */
    Field();

    Field(Field const&)          = delete;
    void operator=(Field const&)  = delete;

    void construct_shader();

    void construct_objects();
};

#endif //_FIELD_H
