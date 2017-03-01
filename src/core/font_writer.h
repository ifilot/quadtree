/**************************************************************************
 *   font_writer.h  --  This file is part of Afelirin.                    *
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

#ifndef _FONT_WRITER
#define _FONT_WRITER

#include <iostream>
#include <memory>
#include <list>
#include <algorithm>
#include <string>

#include "util/pngfuncs.h"

#include <sys/stat.h>
#include <ft2build.h>
#include <freetype2/freetype.h>
#include <freetype2/ftglyph.h>
#include <freetype2/ftoutln.h>
#include <freetype2/fttrigon.h>
#include <boost/lexical_cast.hpp>

#include "core/asset_manager.h"
#include "core/camera.h"
#include "shader.h"
#include "screen.h"
#include "util/utf8.h"
#include "util/mathfunc.h"

#define FONT_TEXTURE_SLOT 1

/**
 * @class FontWriter
 *
 * @brief writes text to the screen
 *
 * This class should be executed after the regular draw calls. Text will be printed
 * on a Canvas which has the same dimensions in pixels as the current resolution. This
 * means that if the screen is rendered at 640x480 pixels, then printing a character
 * at position (10,10) places the character at the left-bottom part of the screen.
 *
 * It is left up to the programmer to adjust the printing position when the resolution
 * is changed. (for instance by using relative positions)
 *
 * To print the characters, a signed distant field method as described in the Valve
 * paper below is used:
 * http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
 *
 */
class FontWriter {
private:
    /**
     * @struct Glyph
     *
     * @brief contains information of a single character
     *
     */
    struct Glyph {
        int width;                  //!< width of the char
        int height;                 //!< height of the char
        float tx1;                  //!< texture coordinate x1
        float ty1;                  //!< texture coordinate y1
        float tx2;                  //!< texture coordinate x2
        float ty2;                  //!< texture coordinate y2
        int horizontal_bearing;     //!< horizontal bearing
        int vertical_bearing;       //!< vertical bearing
        int horizontal_advance;     //!< horizontal advance of char

        Glyph() :
            width(0),
            height(0),
            tx1(0),
            ty1(0),
            tx2(0),
            ty2(0),
            horizontal_bearing(0),
            vertical_bearing(0),
            horizontal_advance(0) {}
    };

    class CharacterAtlas {
    private:
        static const unsigned int font_padding = 12;    //!< padding
        static const unsigned int base_font_size = 32;  //!< font size for texture generation
        static const int sample_depth = 10;             //!< sample depth for the SDF

        std::vector<Glyph> glyphs;                      //!< vector holding collection of glyphs

        GLuint vao;                                     //!< reference of the VAO
        GLuint vbo[3];                                  //!< reference to the VBO

        std::vector<unsigned int> indices;              //!< vector holding indices for the screen characters
        std::vector<glm::vec2> positions;               //!< vector holding screen character positions
        std::vector<glm::vec2> texture_coordinates;     //!< vector holding texture coordinates for the screen characters

        unsigned int pt;                                //!< point size of the font
        float sdf_width;                                //!< shading width for the signed distance field shader
        float sdf_edge;                                 //!< edge width for the signed distance field shader

        bool display_charmap;                           //!< flag whether to print charmap to screen
        bool is_cached;                                 //!< whether current lines are cached in memory

        png_uint_32 texture_width;                      //!< width of the character atlas texture
		png_uint_32 texture_height;                     //!< height of the character atlas texture
        GLuint texture;                                 //!< id of the texture

        unsigned int cstart, ccount;                    //!< start char in map and total number of chars

        std::vector<uint8_t> expanded_data;             //!< vector holding greyscale bitmap of teture

        std::shared_ptr<Shader> shader;                 //!< shader for drawing the font
        const unsigned int texture_slot;                //!< fixed texture where fonts are stored

    public:
        /*
         * @brief       CharacterAtlas constructor
         *
         * @param[in]   font_file       url of the font file
         * @param[in]   _pt             point size of the font
         * @paran[in]   width           width of the sdf
         * @paran[in]   edge            edge of the sdf
         * @param[in]   _shader         shared pointer to the Shader class
         * @param[in]   _cstart         starting index of the font
         * @param[in]   _ccount         number of characters to load
         *
         */
        CharacterAtlas(const std::string& _font_file, unsigned int _pt, float _width, float _edge, std::shared_ptr<Shader> _shader, unsigned int _cstart, unsigned int _ccount);

        /*
         * @brief       write a line of characters
         *
         * @param[in]   x           x position of the line
         * @param[in]   y           y position of the line
         * @param[in]   z           z position of the line
         * @param[in]   color       color of the text
         * @param[in]   line        line holding characters
         *
         */
        void write_text(float x, float y, float z, const glm::vec3& color, const std::string& line);

        /**
         * @brief       obtain bounding box of the line
         *
         * @paran[in]   width       pointer to uint that stores the width
         * @param[in]   height      pointer to uint that stores the height
         * @param[in]   line        string holding the characters
         *
         */
        void get_bounding_box(int* width, int* height, const std::string& line);

        /**
         * @brief Display the complete character map (font atlas) on the screen (used for debugging purposes)
         */
        void draw_charmap_on_screen();

        /**
         * @brief Copy constructor
         */
        CharacterAtlas(const FontWriter::CharacterAtlas& other);

        /**
         * @brief Move constructor
         */
        CharacterAtlas(FontWriter::CharacterAtlas&& other) noexcept;

        /**
         * @brief Destructor operator
         */
        ~CharacterAtlas();

    private:
        /**
         * @brief Place a font in a texture and store the positions
         */
        void generate_character_map(const std::string& filename, const FT_Library& library);

        /**
         * @brief calculate the signed distance field of a Glyph
         *
         * @param[in]   distance_field      array holding distance field values
         * @param[in]   data                vector holding monochromatic glyph
         * @param[in]   width               width of the char bitmap
         * @param[in]   height              height of the char bitmap
         *
         */
        void calculate_distance_field(std::vector<uint8_t>& distance_field, const std::vector<bool>& data, int width, int height);

        /**
         * @brief Unpack a monographic bitmap to a vector of booleans
         *
         * @param[in] FreeType bitmap
         *
         * @return vector holding boolean values for the pixels
         */
        std::vector<bool> unpack_mono_bitmap(FT_Bitmap bitmap);

        /**
         * @brief Load the characters on the GPU
         */
        void static_load();
    };

    const unsigned int texture_slot;                //!< fixed texture where fonts are stored
    std::shared_ptr<Shader> shader;                 //!< shared pointer of the Shader object
    std::vector<CharacterAtlas> fonts;              //!< vector holding all CharacterAtlas objects

public:
    /**
     * @brief       get a reference to the FontWriter
     *
     * @return      reference to the FontWriter object (singleton pattern)
     */
    static FontWriter& get() {
        static FontWriter font_writer_instance;
        return font_writer_instance;
    }

    /**
     * @brief       add a font (Character Atlas) to the FontWriter class
     *
     * @param[in]   fontfile    url of font file
     * @paran[in]   pt          pointsize of the font
     * @paran[in]   width       width of the sdf
     * @paran[in]   edge        edge of the sdf
     * @param[in]   cstart      start char
     * @param[in]   ccount      number of chars to collect (starting from start char)
     *
     * @return      reference to the CharacterAtlas object
     */
    unsigned int add_font(const std::string& fontfile, unsigned int pt, float _width, float _edge, unsigned int cstart, unsigned int ccount);


    /**
     * @brief       render a string of wchar_t characters on the screen
     *
     * @param[in]   font        id of CharacterAtlas
     * @paran[in]   x           x position of the string
     * @param[in]   y           y position of the string
     * @param[in]   z           z position of the string
     * @param[in]   color       color of the characters
     * @param[in]   line        string holding the characters
     *
     */
    void write_text(unsigned int font, float x, float y, float z, const glm::vec3& color, const std::string& line);

    /*
     * @brief       write a line of characters in 3D space
     *
     * @param[in]   font        font id
     * @paran[in]   p1          start 3d position
     * @paran[in]   p2          stop 3d position
     * @param[in]   dist        distance towards camera
     * @param[in]   color       color of the text
     *
     */
    void write_dotted_line(unsigned int font, const glm::vec3& p1, const glm::vec3& p2, float dist, const glm::vec3& color);

    /**
     * @brief       obtain bounding box of the line
     *
     * @param[in]   font        id of CharacterAtlas
     * @paran[in]   width       pointer to uint that stores the width
     * @param[in]   height      pointer to uint that stores the height
     * @param[in]   line        string holding the characters
     *
     */
    void get_bounding_box(unsigned int font, int* width, int* height, const std::string& line);

    /*
     * @brief   Utility method that prints the texture of the 1st CharacterAtlas on the screen
     */
    void draw();

private:
    /**
     * @brief       FontWriter constructor
     *
     * @return      FontWriter instance
     */
    FontWriter();

    FontWriter(FontWriter const&)          = delete;
    void operator=(FontWriter const&)  = delete;
};

#endif // _FONT_WRITER
