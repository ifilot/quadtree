/**************************************************************************
 *   font_writer.cpp  --  This file is part of Afelirin.                  *
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

#include "font_writer.h"

/**
 * @brief       FontWriter constructor
 *
 * @return      FontWriter instance
 */
FontWriter::FontWriter() : texture_slot(FONT_TEXTURE_SLOT) {
    this->shader = std::shared_ptr<Shader>(new Shader("assets/shaders/text_sdf"));
    this->shader->add_attribute(ShaderAttribute::POSITION, "position");
    this->shader->add_attribute(ShaderAttribute::TEXTURE_COORDINATE, "texture_coordinate");
    this->shader->add_uniform(ShaderUniform::MAT4, "mvp", 1);
    this->shader->add_uniform(ShaderUniform::VEC3, "textcolor", 1);
    this->shader->add_uniform(ShaderUniform::TEXTURE, "text", 1);
    this->shader->add_uniform(ShaderUniform::FLOAT, "width", 1);
    this->shader->add_uniform(ShaderUniform::FLOAT, "edge", 1);
}

/**
 * @brief       add a font (Character Atlas) to the FontWriter class
 *
 * @param[in]   fontfile    url of font file
 * @paran[in]   pt          pointsize of the font
 * @param[in]   cstart      start char
 * @param[in]   ccount      number of chars to collect (starting from start char)
 *
 * @return      reference to the CharacterAtlas object
 */
unsigned int FontWriter::add_font(const std::string& _fontfile, unsigned int pt, float _width, float _edge, unsigned int cstart, unsigned int ccount) {
    std::string filename = AssetManager::get().get_root_directory() + _fontfile;
    this->fonts.push_back(CharacterAtlas(filename, pt, _width, _edge, this->shader, cstart, ccount));
    return this->fonts.size() - 1;
}

/**
 * @brief       render a string of wchar_t characters on the screen
 *
 * @param[in]   font        id of CharacterAtlas
 * @paran[in]   x           x position of the string
 * @param[in]   y           start char
 * @param[in]   color       color of the characters
 * @param[in]   line        string holding the characters
 *
 */
void FontWriter::write_text(unsigned int font, float x, float y, float z, const glm::vec3& color, const std::string& line) {
    if(font < this->fonts.size()) {
        this->fonts[font].write_text(x, y, z, color, line);
    }
}

/**
 * @brief       obtain bounding box of the line
 *
 * @param[in]   font        id of CharacterAtlas
 * @paran[in]   width       pointer to uint that stores the width
 * @param[in]   height      pointer to uint that stores the height
 * @param[in]   line        string holding the characters
 *
 */
void FontWriter::get_bounding_box(unsigned int font, int* width, int* height, const std::string& line) {
    if(font < this->fonts.size()) {
        this->fonts[font].get_bounding_box(width, height, line);
    } else {
        std::cerr << "Invalid font index requested." << std::endl;
    }
}

/*
 * @brief   Utility method that prints the texture of the 1st CharacterAtlas on the screen
 */
void FontWriter::draw() {
    this->fonts[0].draw_charmap_on_screen();
}

/*
 * @brief       CharacterAtlas constructor
 *
 * @param[in]   font_file       url of the font file
 * @param[in]   _pt             point size of the font
 * @param[in]   _shader         shared pointer to the Shader class
 * @param[in]   _cstart         starting index of the font
 * @param[in]   _ccount         number of characters to load
 *
 */
FontWriter::CharacterAtlas::CharacterAtlas(const std::string& font_file, unsigned int _pt, float _width, float _edge,
        std::shared_ptr<Shader> _shader, unsigned int _cstart, unsigned int _ccount) : texture_slot(FONT_TEXTURE_SLOT) {
    this->display_charmap = false;
    this->is_cached = false;
    this->shader = _shader;
    this->cstart = _cstart;
    this->ccount = _ccount;
    this->pt = _pt;
    this->sdf_width = _width;
    this->sdf_edge = _edge;

    FT_Library library;         //!< FreeType library
    if(FT_Init_FreeType(&library)) {
        std::cerr << "FT_Init_FreeType failed" << std::endl;
    }

    this->glyphs.resize(this->ccount);
    this->generate_character_map(font_file, library);

    this->static_load();

    FT_Done_FreeType(library);
}

/*link_shaderlink_shader
 * @brief       write a line of characters
 *
 * @param[in]   x           x position of the line
 * @param[in]   y           y position of the line
 * @param[in]   color       color of the text
 * @param[in]   line        line holding characters
 *
 */
void FontWriter::CharacterAtlas::write_text(float x, float y, float z, const glm::vec3& color, const std::string& line) {
    if(line.size() == 0) {
        return;
    }

    const glm::mat4 projection = glm::ortho(0.0f,
                                      (float)Screen::get().get_resolution_x(),
                                      0.0f,
                                      (float)Screen::get().get_resolution_y());

    float xx = x;
    float yy = y;

    const float scale = pt / (float)this->base_font_size;

    // load the texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->texture);

    // load the vertex array
    glBindVertexArray(this->vao);

    this->shader->link_shader();
    this->shader->set_uniform("textcolor", &color[0]);
    this->shader->set_uniform("text", &this->texture_slot);
    this->shader->set_uniform("width", &this->sdf_width);
    this->shader->set_uniform("edge", &this->sdf_edge);

    auto it = line.begin();
    auto end = line.end();
    unsigned int count = 0;

    do {
        uint32_t c = utf8::next(it, end);

        count++;

        if(c - this->cstart >= this->glyphs.size()) {
            std::cerr << "[ERROR] (write_text) Invalid Glyph requested: " << c << std::endl;
            std::cerr << *it << std::endl;
            continue;
        }

        float cxx = xx;
        float cyy = yy;// + (this->glyphs[c - this->cstart].height -this->glyphs[c - this->cstart].vertical_bearing) * scale;

        glm::mat4 mvp = glm::translate(projection, glm::vec3(cxx, cyy, z + count / 1e6f));

        this->shader->set_uniform("mvp", &mvp[0][0]);

        // draw the mesh using the indices
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid*) (sizeof(GL_UNSIGNED_INT) * (c - this->cstart) * 4));

         xx += this->glyphs[c - this->cstart].horizontal_advance * scale;

    } while(it < end);

    // after this command, any commands that use a vertex array will
    // no longer work
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief       obtain bounding box of the line
 *
 * @paran[in]   width       pointer to uint that stores the width
 * @param[in]   height      pointer to uint that stores the height
 * @param[in]   line        string holding the characters
 *
 */
void FontWriter::CharacterAtlas::get_bounding_box(int* width, int* height, const std::string& line) {
    const float scale = this->pt / (float)this->base_font_size;

    *width = 0;
    *height = 0;

    if(line.size() == 0) {
        return;
    }

    auto it = line.begin();
    auto end = line.end();

    do {
        uint32_t c = utf8::next(it, end);

        if(c - this->cstart < this->glyphs.size()) {
            *width += this->glyphs[c - this->cstart].horizontal_advance * scale;
            *height = std::max(*height, (int)(this->glyphs[c - this->cstart].vertical_bearing * scale));
        } else {
            std::cerr << "[ERROR] (get_bounding_box) Invalid Glyph position requested, line: " << line << std::endl;
            std::cerr << "[ERROR] (get_bounding_box) Character: " << *it << std::endl;
        }
    } while(it < end);
}

/**
 * @brief Load the characters on the GPU
 */
void FontWriter::CharacterAtlas::static_load() {
    const float scale = this->pt / (float)this->base_font_size;
    const float pts = scale * (float)this->font_padding;

    this->positions.clear();
    this->texture_coordinates.clear();
    this->indices.clear();

    for(unsigned int c=0; c<this->ccount; c++) {

        const float fx = (float)this->glyphs[c].horizontal_bearing * scale;
        const float fy = -(float)(this->glyphs[c].height - this->glyphs[c].vertical_bearing) * scale;

        this->positions.push_back(glm::vec2(fx - pts, fy + pts + (float)this->glyphs[c].height * scale));
        this->positions.push_back(glm::vec2(fx - pts, fy - pts));
        this->positions.push_back(glm::vec2(fx + pts + (float)this->glyphs[c].width * scale, fy + pts + (float)this->glyphs[c].height * scale));
        this->positions.push_back(glm::vec2(fx + pts + (float)this->glyphs[c].width * scale, fy - pts));

        this->texture_coordinates.push_back(glm::vec2(this->glyphs[c].tx1, this->glyphs[c].ty2));
        this->texture_coordinates.push_back(glm::vec2(this->glyphs[c].tx1, this->glyphs[c].ty1));
        this->texture_coordinates.push_back(glm::vec2(this->glyphs[c].tx2, this->glyphs[c].ty2));
        this->texture_coordinates.push_back(glm::vec2(this->glyphs[c].tx2, this->glyphs[c].ty1));

        this->indices.push_back(this->positions.size()-4);
        this->indices.push_back(this->positions.size()-3);
        this->indices.push_back(this->positions.size()-2);
        this->indices.push_back(this->positions.size()-1);
    }

    this->positions.push_back(glm::vec2(0, this->texture_height));
    this->positions.push_back(glm::vec2(0, 0));
    this->positions.push_back(glm::vec2(this->texture_width, this->texture_height));
    this->positions.push_back(glm::vec2(this->texture_width, 0));

    this->texture_coordinates.push_back(glm::vec2(0,0));
    this->texture_coordinates.push_back(glm::vec2(0,1));
    this->texture_coordinates.push_back(glm::vec2(1,0));
    this->texture_coordinates.push_back(glm::vec2(1,1));

    this->indices.push_back(this->positions.size()-4);
    this->indices.push_back(this->positions.size()-3);
    this->indices.push_back(this->positions.size()-2);
    this->indices.push_back(this->positions.size()-1);

    glGenVertexArrays(1, &this->vao);

    // generate a vertex array object and store it in the pointer
    glBindVertexArray(this->vao);

    // generate a number of buffers (blocks of data on the GPU)
    glGenBuffers(3, this->vbo);

    /*
     * POSITIONS
     */

    // bind a buffer identified by POSITION_VB and interpret this buffer as an array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // fill the buffer with data
    glBufferData(GL_ARRAY_BUFFER, this->positions.size() * 2 * sizeof(float), &this->positions[0][0], GL_STATIC_DRAW);

    // specifies the generic vertex attribute of index 0 to be enabled
    glEnableVertexAttribArray(0);
    // define an array of generic vertex attribute data
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /*
     * TEXTURE COORDINATES
     */

    // up the vertex_id
    // bind a buffer identified by POSITION_VB and interpret this buffer as an array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    // fill the buffer with data
    glBufferData(GL_ARRAY_BUFFER, this->texture_coordinates.size() * 2 * sizeof(float), &this->texture_coordinates[0][0], GL_STATIC_DRAW);

    // specifies the generic vertex attribute of index 0 to be enabled
    glEnableVertexAttribArray(1);
    // define an array of generic vertex attribute data
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /*
     * INDICES_VB
     */

    // bind a buffer identified by INDICES_VB and interpret this buffer as an array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
    // fill the buffer with data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

    if(!this->shader->is_loaded()) {
        this->shader->bind_uniforms_and_attributes();
    }

    // after this command, any commands that use a vertex array will
    // no longer work
    glBindVertexArray(0);
}

/**
 * @brief Copy constructor
 */
FontWriter::CharacterAtlas::CharacterAtlas(const FontWriter::CharacterAtlas& other) :
    glyphs(other.glyphs),
    pt(other.pt),
    sdf_width(other.sdf_width),
    sdf_edge(other.sdf_edge),
    display_charmap(other.display_charmap),
    is_cached(other.is_cached),
    texture_width(other.texture_width),
    texture_height(other.texture_height),
    texture(other.texture),
    cstart(other.cstart),
    ccount(other.ccount),
    shader(other.shader),
    texture_slot(FONT_TEXTURE_SLOT) {

    this->vao = other.vao;
    this->vbo[0] = other.vbo[0];
    this->vbo[1] = other.vbo[1];
    this->vbo[2] = other.vbo[2];
}

/**
 * @brief Move constructor
 */
FontWriter::CharacterAtlas::CharacterAtlas(FontWriter::CharacterAtlas&& other) noexcept :
    glyphs(other.glyphs),
    pt(other.pt),
    sdf_width(other.sdf_width),
    sdf_edge(other.sdf_edge),
    display_charmap(other.display_charmap),
    is_cached(other.is_cached),
    texture_width(other.texture_width),
    texture_height(other.texture_height),
    texture(other.texture),
    cstart(other.cstart),
    ccount(other.ccount),
    shader(other.shader),
    texture_slot(FONT_TEXTURE_SLOT) {

    this->vao = other.vao;
    this->vbo[0] = other.vbo[0];
    this->vbo[1] = other.vbo[1];
    this->vbo[2] = other.vbo[2];

    other.vao = 0;
    other.vbo[0] = 0;
    other.vbo[1] = 0;
    other.vbo[2] = 0;
}

FontWriter::CharacterAtlas::~CharacterAtlas() {
    glDeleteBuffers(2, this->vbo);
    glDeleteVertexArrays(1, &this->vao);
}

/**
 * @brief Place a font in a texture and store the positions
 */
void FontWriter::CharacterAtlas::generate_character_map(const std::string& filename, const FT_Library& library) {
    std::vector<std::vector<bool>> char_bitmaps(this->ccount);
    unsigned int img_width = 0;         // total image width
    unsigned int img_height = 0;        // total image height

    unsigned int temp_x = 0;
    unsigned int temp_y = 0;
    unsigned int counter = 0;           // iteration counter

    const unsigned int charmap_width = (unsigned int)std::sqrt(this->ccount);

    // collect all data of the glyphs and store them in temporary vectors
    for(unsigned int i=this->cstart; i<this->cstart+this->ccount; i++) {
        counter++;
        wchar_t c = (wchar_t)i;

        FT_Face face;

        if(FT_New_Face( library, filename.c_str(), 0, &face )) {
            std::cerr << "FT_New_Face failed (there is probably a problem with your font file)" << std::endl;
        }

        FT_Set_Char_Size( face, this->base_font_size * 64, this->base_font_size * 64, 128, 128);

        if(FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_MONO)) {
            std::cerr << "Error loading char." << std::endl;
        }

        const unsigned int width = face->glyph->bitmap.width + 2 * this->font_padding;
        const unsigned int height = face->glyph->bitmap.rows + 2 * this->font_padding;

        this->glyphs[i - this->cstart].width = face->glyph->bitmap.width;
        this->glyphs[i - this->cstart].height = face->glyph->bitmap.rows;
        this->glyphs[i - this->cstart].horizontal_bearing = face->glyph->metrics.horiBearingX / 64;
        this->glyphs[i - this->cstart].vertical_bearing = face->glyph->metrics.horiBearingY / 64;
        this->glyphs[i - this->cstart].horizontal_advance =  face->glyph->metrics.horiAdvance / 64;

        char_bitmaps[i - this->cstart] = this->unpack_mono_bitmap(face->glyph->bitmap);

        FT_Done_Face(face);

        temp_x += width;
        temp_y = std::max(temp_y, height);

        if(counter % charmap_width == 0) {
            counter = 0;

            img_width = std::max(img_width, temp_x);
            img_height += temp_y;

            temp_x = 0;
            temp_y = 0;
        }
    }

    if(counter % charmap_width != 0) {
        img_height += temp_y;
    }

    struct stat buffer;
    if(stat ((filename + ".png").c_str(), &buffer) == 0) {

        int col;
        int bit_depth;

        // note: the FontWriter class assumes that the bitmap is stored in memory with the origin at NW
        PNG::load_image_buffer_from_png(filename + ".png", this->expanded_data, &this->texture_width, &this->texture_height, &col, &bit_depth, false);

        if(col != 0) { // note: 0 means GREYSCALE image
            std::cerr << "Invalid number of colors read in font reader: " << col << std::endl;
            exit(-1);
        }

        if(bit_depth != 8) {
            std::cerr << "Invalid bit depth encountered in font reader: " << bit_depth << std::endl;
            exit(-1);
        }

        temp_y = 0;
        unsigned int gy = 0;
        unsigned int gx = 0;

        counter = 0;
        for(unsigned int i=this->cstart; i<this->cstart+this->ccount; i++) {
            counter++;
            const unsigned int width = this->glyphs[i - this->cstart].width + 2 * this->font_padding;
            const unsigned int height = this->glyphs[i - this->cstart].height + 2 * this->font_padding;

            this->glyphs[i - this->cstart].tx1 = (float)gx / (float)img_width;
            this->glyphs[i - this->cstart].tx2 = (float)(gx + width) / (float)img_width;
            this->glyphs[i - this->cstart].ty1 = (float)(gy + height) / (float)img_height;
            this->glyphs[i - this->cstart].ty2 = (float)(gy) / (float)img_height;

            gx += width;
            temp_y = std::max(temp_y, height);

            if(counter % charmap_width == 0) {
                gx = 0;
                gy += temp_y;
                temp_y = 0;
            }
        }

    } else {
        // store char map sizes
        this->texture_width = img_width;
        this->texture_height = img_height;

        // create a texture from the stored information, set the base value
        // to zero
        this->expanded_data.resize(img_width * img_height, 0.0);
        temp_y = 0;
        unsigned int gy = 0;
        unsigned int gx = 0;

        counter = 0;
        for(unsigned int i=this->cstart; i<this->cstart+this->ccount; i++) {
            counter++;
            const unsigned int width = this->glyphs[i - this->cstart].width + 2 * this->font_padding;
            const unsigned int height = this->glyphs[i - this->cstart].height + 2 * this->font_padding;

            std::vector<uint8_t> distance_field(width * height, 0.0f);
            this->calculate_distance_field(distance_field,
                                           char_bitmaps[i-this->cstart],
                                           width,
                                           height);

            for(unsigned int k=0; k<height; k++) {
                for(unsigned int l=0; l<width; l++){
                    this->expanded_data[(l+gx)+(k+gy)*img_width] = distance_field[l + width*k];
                }
            }

            this->glyphs[i - this->cstart].tx1 = (float)gx / (float)img_width;
            this->glyphs[i - this->cstart].tx2 = (float)(gx + width) / (float)img_width;
            this->glyphs[i - this->cstart].ty1 = (float)(gy + height) / (float)img_height;
            this->glyphs[i - this->cstart].ty2 = (float)(gy) / (float)img_height;

            gx += width;
            temp_y = std::max(temp_y, height);

            if(counter % charmap_width == 0) {
                gx = 0;
                gy += temp_y;
                temp_y = 0;
            }
        }

        // save result to assets
        PNG::write_image_buffer_to_png(filename + ".png", this->expanded_data, this->texture_width, this->texture_height, PNG_COLOR_TYPE_GRAY, true);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        this->texture_width,
        this->texture_height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        &this->expanded_data[0]
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief calculate the signed distance field of a Glyph
 *
 * @param[in]   distance_field      array holding distance field values
 * @param[in]   data                vector holding monochromatic glyph
 * @param[in]   width               width of the char bitmap
 * @param[in]   height              height of the char bitmap
 *
 */
void FontWriter::CharacterAtlas::calculate_distance_field(std::vector<uint8_t>& distance_field, const std::vector<bool>& data, int width, int height) {
    const float max_dist = std::sqrt((float)(2 * this->sample_depth * this->sample_depth));

    for(int k=0; k<height; k++) {
        for(int l=0; l<width; l++){

            float distance = max_dist;
            for(int j=k-this->sample_depth; j<=k+this->sample_depth; j++) {
                for(int i=l-this->sample_depth; i<=l+this->sample_depth; i++) {
                    if(i >= 0 && i < width && j >= 0 && j < height) {
                        if(data[l + width*k] != data[i + width*j]) {
                            float dist = std::sqrt((float)((i-l)*(i-l)) + (float)((j-k)*(j-k)));
                            distance = std::min(distance, dist);
                        }
                    }
                }
            }

            if(data[l + width*k]) { // inside
                distance_field[l+k*width] = (0.5f + (distance / max_dist) * 0.5f) * 255.0f;
            } else { // outside
                distance_field[l+k*width] = (0.5f - (distance / max_dist) * 0.5f) * 255.0f;
            }
        }
    }
}

/**
 * @brief Unpack a monographic bitmap to a vector of booleans
 *
 * @param[in] FreeType bitmap
 *
 * @return vector holding boolean values for the pixels
 */
std::vector<bool> FontWriter::CharacterAtlas::unpack_mono_bitmap(FT_Bitmap bitmap) {
    std::vector<bool> result( (bitmap.rows + 2 * this->font_padding) * (bitmap.width + 2 * this->font_padding), false);

    for (int y = 0; y < (int)bitmap.rows; y++) {
        for (int byte_index = 0; byte_index < bitmap.pitch; byte_index++) {

            unsigned char byte_value = bitmap.buffer[y * bitmap.pitch + byte_index];

            int num_bits_done = byte_index * 8;

            int rowstart = (y + this->font_padding) * (bitmap.width + 2 * this->font_padding) + byte_index * 8 + this->font_padding;

            for (int bit_index = 0; bit_index < std::min(8, (int)(bitmap.width - num_bits_done)); bit_index++) {
                int bit = byte_value & (1 << (7 - bit_index));

                result[rowstart + bit_index] = bit;
            }
        }
    }

    return result;
}

/**
 * @brief Display the complete character map (font atlas) on the screen (used for debugging purposes)
 */
void FontWriter::CharacterAtlas::draw_charmap_on_screen() {
    const glm::mat4 projection = glm::ortho(0.0f,
                                      (float)Screen::get().get_resolution_x(),
                                      0.0f,
                                      (float)Screen::get().get_resolution_y());

    // load the texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->texture);

    // load the vertex array
    glBindVertexArray(this->vao);

    glm::vec3 color(1,1,1);

    this->shader->link_shader();
    this->shader->set_uniform("mvp", &projection[0][0]);
    this->shader->set_uniform("textcolor", &color[0]);
    this->shader->set_uniform("text", &this->texture_slot);
    this->shader->set_uniform("width", &this->sdf_width);
    this->shader->set_uniform("edge", &this->sdf_edge);

    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid*) (sizeof(GL_UNSIGNED_INT) * this->ccount * 4));

    // after this command, any commands that use a vertex array will
    // no longer work
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
