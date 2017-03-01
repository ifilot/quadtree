#include "field.h"

Field::Field() {
    this->construct_shader();
    this->construct_objects();

    this->quadtree = QuadTree<Point>(0.5, 0.5, 1, 1);

    for(unsigned int i=0; i<50; i++) {
        double x = (double)rand() / (double)RAND_MAX;
        double y = (double)rand() / (double)RAND_MAX;

        this->quadtree.add(new Point(x,y), x, y);
    }

}

void Field::draw() {
    static const glm::vec4 color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
    const glm::mat4 projection = Camera::get().get_projection();

    this->shader->link_shader();
    glBindVertexArray(this->vao);
    this->shader->set_uniform("color", &color);
    this->shader->set_uniform("mvp", &projection);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

    this->quadtree.draw(this->shader.get());

    glBindVertexArray(0);
    this->shader->unlink_shader();
}

void Field::construct_shader() {
    // load shader
    this->shader = std::unique_ptr<Shader>(new Shader("assets/shaders/line"));
    this->shader->add_attribute(ShaderAttribute::POSITION, "position");
    this->shader->add_uniform(ShaderUniform::MAT4, "mvp", 1);
    this->shader->add_uniform(ShaderUniform::VEC4, "color", 1);
}

void Field::construct_objects() {
    static const float pos[8] = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0};
    static const unsigned int indices[4] = {0,1,2,3};

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(2, this->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), &pos[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    this->shader->bind_uniforms_and_attributes();

    glBindVertexArray(0);
}

void Field::add_point(double x, double y) {
    this->quadtree.add(new Point(x,y), x, y);
}
