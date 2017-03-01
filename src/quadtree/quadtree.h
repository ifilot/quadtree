#ifndef _QUAD_TREE
#define _QUAD_TREE

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/shader.h"

template <class T>
class QuadTreeObject {
public:
    QuadTreeObject(T* _objptr, double _x, double _y) :
    x(_x),
    y(_y),
    objptr(_objptr) {}

    T* objptr;
    double x;
    double y;
};

template <class T>
class QuadTreeNode {
private:
    std::vector<QuadTreeObject<T>> objects;
    QuadTreeNode* parent;
    QuadTreeNode* children[4];

    double cx;  // center x position
    double cy;  // center y position

    double width;   // bounding box width
    double height;  // bounding box height

    unsigned int level;

public:
    QuadTreeNode(double _cx, double _cy, double _width, double _height, unsigned int _level, QuadTreeNode* _parent):
        cx(_cx),
        cy(_cy),
        width(_width),
        height(_height),
        level(_level),
        parent(_parent) {
            this->children[0] = nullptr;
            this->children[1] = nullptr;
            this->children[2] = nullptr;
            this->children[3] = nullptr;
        }

    inline bool has_children() const {
        return (this->children[0] != nullptr);
    }

    void print() {
        std::cout << "NODE: " << cx << "\t" << cy << "\t" << level << std::endl;
        for(auto obj: this->objects) {
            std::cout << obj.x << "\t" << obj.y << "\t" << obj.objptr << std::endl;
        }

        for(unsigned int i=0; i<4; i++) {
            if(this->children[i] != nullptr) {
                this->children[i]->print();
            }
        }
    }

    void draw(Shader* shader) {
        const glm::mat4 projection = Camera::get().get_projection();

        float scale = this->width / 1.0;
        glm::vec3 color = glm::vec3((float)level / 10.0f, (float)level / 10.0f, (float)level / 10.0f, 0.1f);
        glm::mat4 mvp = projection * glm::translate(glm::mat4(1.0f), glm::vec3(this->cx - this->width / 2, this->cy - this->height / 2, (float)level / 10.0f)) * glm::scale(glm::vec3(scale,scale,1.0));
        shader->set_uniform("mvp", &mvp);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

        for(auto obj: this->objects) {
            glm::mat4 mvp = projection * glm::translate(glm::mat4(1.0f), glm::vec3(obj.x, obj.y, 1.0f)) * glm::scale(glm::vec3(0.005f,0.005f,1.0));
            shader->set_uniform("mvp", &mvp);
            glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
        }

        for(unsigned int i=0; i<4; i++) {
            if(this->children[i] != nullptr) {
                this->children[i]->draw(shader);
            }
        }
    }

    void split() {
        // dp not split if the children are not nullpointers
        if(children[0] != nullptr) {
            return;
        }

        const double new_width = this->width / 2.0;
        const double new_height = this->height / 2.0;

        // create four new nodes
        this->children[0] = new QuadTreeNode(this->cx + new_width / 2.0, this->cy + new_height / 2.0, new_width, new_height, this->level+1, this);
        this->children[1] = new QuadTreeNode(this->cx - new_width / 2.0, this->cy + new_height / 2.0, new_width, new_height, this->level+1, this);
        this->children[2] = new QuadTreeNode(this->cx - new_width / 2.0, this->cy - new_height / 2.0, new_width, new_height, this->level+1, this);
        this->children[3] = new QuadTreeNode(this->cx + new_width / 2.0, this->cy - new_height / 2.0, new_width, new_height, this->level+1, this);

        // migrate objects
        for(auto obj: this->objects) {
            this->add(obj);
        }

        this->objects.clear();
    }

    void add(const QuadTreeObject<T> &obj) {
        if(!this->has_children()) {
            this->objects.push_back(obj);

            if(this->objects.size() >= 5) {
                this->split();
            }

            return;
        }

        if(obj.x > this->cx && obj.y > this->cy) {
            this->children[0]->add(obj);
        } else if(obj.x < this->cx && obj.y > this->cy) {
            this->children[1]->add(obj);
        } else if(obj.x < this->cx && obj.y < this->cy) {
            this->children[2]->add(obj);
        } else {
            this->children[3]->add(obj);
        }
    }
};

template <class T>
class QuadTree {
private:
    QuadTreeNode<T>* root;

public:
    QuadTree() {
        this->root = nullptr;
    }

    QuadTree(double _cx, double _cy, double _width, double _height) {
        this->root = new QuadTreeNode<T>(_cx, _cy, _width, _height, 0, nullptr);
    }

    void add(T* _obj, double x, double y) {
        if(this->root != nullptr) {
            QuadTreeObject<T> obj(_obj, x, y);
            this->root->add(obj);
        } else {
            std::cerr << "Cannot add objects to quadtree with NULL root" << std::endl;
        }
    }

    void print() {
        if(this->root != nullptr) {
            this->root->print();
        }
    }

    void draw(Shader* shader) {
        if(this->root != nullptr) {
            this->root->draw(shader);
        }
    }

private:

};

#endif //_QUAD_TREE
