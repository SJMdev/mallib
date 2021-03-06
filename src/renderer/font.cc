#include <font.h>

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <fmt/core.h>

#include <gl_lite/gl_lite.h>
#include <graphics.h>

#include <on_leaving_scope.h>

#include <mgl/vec.h> // lerp
#include <mgl/mat.h> // mgl::ortho
#include <cmath> // sin, PI

#include <game.h> //@Refactor: replace this with globals for previous_frame_time.
#include <globals.h> // for the window dimensions.



font::Font& font::default_font()
{
    static Font default_font;
    return default_font;
}

font::gl_Objects& font::gl_objects() //@Note:VAO & VBO
{
    static gl_Objects objects;
    return objects;
}

//@Note:For now, init font inits a default font & inits the VAO & VBO.
void font::init_font()
{
    //@Memory: reserve enough space. 
    font::default_font().characters.resize(255);
    font::generate_font_at_size(font::default_font(), "assets/fonts/arial.ttf", 48);
    font::init_font_gl_objects();
}

void font::init_font_gl_objects()
{
    font::gl_Objects& objects = font::gl_objects();
    glGenVertexArrays(1, &objects.VAO);
    glGenBuffers(1, &objects.VBO);
    glBindVertexArray(objects.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, objects.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);      
}

void font::gl_text_mode()
{
    //@Note: this now happens in the init_font function.
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int32_t window_width = globals.window_width;
    int32_t window_height = globals.window_height;
    glViewport(0, 0,static_cast<int>(window_width),static_cast<int>(window_height));
}


void font::draw_text(std::string text,
                     Font& font,
                     uint32_t start_x,
                     uint32_t start_y,
                     float scale,
                     mgl::vec3 color,
                     Text_Effect effect)
{

    font::gl_text_mode();
    font::gl_Objects& gl_font = font::gl_objects();

    if (effect == Text_Effect::COLOUR_SHIFT)
    {
        //@FIXME:: think about how to do this.
        static float accumulator = 0;
        accumulator += game::previous_frame_time().count();
        // fmt::print("previous_frame_time, accumulator: {}, {}\n", game::previous_frame_time().count(), accumulator);
        float pulse_time = 2000.0f;
        accumulator = fmod(accumulator, pulse_time);
        float ratio = accumulator / pulse_time;
        float sin_t = ratio * 2 * 3.14;
        // use previous_frame_time? how do we calculate this?
        float distance_t = (std::sin(sin_t) + 1.0f) /2.0f;
        color = mgl::lerp(color, {1.0f,1.0f,1.0f}, distance_t);
    }

    graphics::set_shader("text"); 
    int32_t text_color_uniform = glGetUniformLocation(graphics::shaders()["text"], "text_color");
    glUniform3f(text_color_uniform, color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(gl_font.VAO);   
    //@Refactor: this should match the Active Texture (i.e. GL_TEXTURE0). we're lucky that it does right now.
    glUniform1i(glGetUniformLocation(graphics::shaders()["text"], "text"), 0);

    const uint32_t window_height = globals.window_height;
    const uint32_t window_width = globals.window_width;
    const float top   = window_height; // viewport 
    const float bot   = 0.0f;
    const float left  = 0.0f;
    const float right = window_width; // viewport
    const float z_near  = 0.0f;
    const float z_far   = 1.0f; // near and far are reserved by windows???

    mgl::mat4 projection_matrix = mgl::ortho(left, right, top, bot, z_near, z_far); 
    glUniformMatrix4fv(glGetUniformLocation(graphics::shaders()["text"], "projection_matrix"), 1, true, &projection_matrix[0][0]);

    
    const auto& glyph_array = font.characters;
    for (const auto& single_char: text)
    {
        const auto& char_glyph = glyph_array.at(single_char); //Character&?

        GLfloat xpos = start_x + char_glyph.bearing.x * scale;
        GLfloat ypos = start_y - (char_glyph.size.y - char_glyph.bearing.y) * scale;

        GLfloat width  = char_glyph.size.x * scale;
        GLfloat height = char_glyph.size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] =
        {
            { xpos,     ypos + height,   0.0, 0.0 },            
            { xpos,          ypos,       0.0, 1.0 },
            { xpos + width,  ypos,       1.0, 1.0 },

            { xpos,         ypos + height,   0.0, 0.0 },
            { xpos + width,      ypos,       1.0, 1.0 },
            { xpos + width, ypos + height,   1.0, 0.0 }           
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, char_glyph.textureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gl_font.VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        start_x += (char_glyph.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);   
}

//@Note: pixel_count is both x and y dim of the letter.
//@Refactor: it's kind of weird how this function sets the target.pixel_count.

void font::generate_font_at_size(font::Font& target, std::string font_name, const uint32_t pixel_count)
{
    //@TODO: this is hardcoded to only generate 255 characters. how many do we need? or does that depend on the locale?
    const uint8_t num_characters = 255; 

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        fmt::print("generate_font_at_size: failed to init freetype.");

    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face))
        fmt::print("generate_font_at_size: failed to create new font face.\n");

    FT_Set_Pixel_Sizes(face, 0, pixel_count);
    // specify 1-byte alignment. Let us undo this after.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
    auto defer_pixel_store = On_Leaving_Scope([]{glPixelStorei(GL_UNPACK_ALIGNMENT,4);});

    //@Memory: assume an empty vector, this function reserves N.
    target.characters.resize(num_characters);

    // create character glyphs and store them in the array.
    for (uint8_t char_index = 0; char_index != num_characters;  ++char_index)
    {
        if (FT_Load_Char(face, char_index, FT_LOAD_RENDER))
        {
            fmt::print("error: freetype: failed to load glyph %", char_index);
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character_glyph = {
            texture,
            {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            {face->glyph->bitmap_left, face->glyph->bitmap_top},
            face->glyph->advance.x
        };

        target.characters[char_index] = character_glyph;
    }
    //@NOTE: we need to unbind textures after binding them.
    glBindTexture(GL_TEXTURE_2D, 0); 

    // free all FreeType resources.
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


uint32_t font::get_string_width_in_pixels(const std::string text,font::Font& font, const uint32_t scale)
{
    uint32_t total_width = 0;

    const auto& glyph_array = font.characters;
    for (const auto single_char: text)
    {
        const auto& char_glyph = glyph_array.at(single_char); //Character&?
        total_width += (char_glyph.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    return total_width;
}