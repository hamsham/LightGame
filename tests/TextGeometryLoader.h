/* 
 * File:   draw/GeometryLoader.h
 * Author: miles
 *
 * Created on August 3, 2015, 9:50 PM
 */

#ifndef __LS_DRAW_GEOMETRY_LOADER_H__
#define __LS_DRAW_GEOMETRY_LOADER_H__

#include <string>

/*-----------------------------------------------------------------------------
 * Forward Declarations
-----------------------------------------------------------------------------*/
namespace ls {

/*-----------------------------------------------------------------------------
 * Text Loader Functions
-----------------------------------------------------------------------------*/
namespace draw {

enum common_vertex_t : unsigned;
class BufferObject;
class Atlas;
class AtlasEntry;
class VertexArray;

/**------------------------------------
 * @brief Mesh Properties
 * 
 * These enumerations contains properties for different types of geometry
 * objects. These constants used within contain vital information that's used
 * within the geometry object's implementation.
-------------------------------------*/
enum geometry_property_t: int {
    MESH_VERTS_PER_GLYPH = 4,
    
    MESH_INDICES_PER_GLYPH = 6,
    
    // TODO: Make this number editable within the "load_*" functions.
    MESH_SPACES_PER_TAB = 4
};

/**------------------------------------
 * @brief Initialize, generate, and emplace a set of textual geometry into an
 * OpenGL VBO and IBO.
 * 
 * The winding/index order for all text rendering follows this basic format:
 *  
 * 0--------2,3
 * |     /  |
 * |   /    |
 * | /      |
 * 1,4------5
 * 
 * @param str
 * A constant reference to a string of text, containing the characters which
 * will be represented by the geometry contained within the vbo and ibo
 * parameters.
 * 
 * @param vertexTypes
 * A bitmask, containing a set of vertex types which should be generated by
 * this function.
 * 
 * @param vbo
 * A reference to an OpenGL Vertex Buffer Object which will have a set of
 * vertices placed into it for rendering text information.
 * 
 * @param ibo
 * A reference to an OpenGL Index Buffer Object which will have a set of vertex
 * indices placed into it for rendering text information.
 * 
 * @param atlas
 * A constant reference to an atlas object which contains glyph size and text
 * bitmaps which will be represented by the resulting VBO+IBO objects.
 * 
 * @return An unsigned integral type, containing the number of indices which
 * were used to generate the vertex data in the "vbo" parameter.
-------------------------------------*/
unsigned load_text_geometry(
    const std::string& str,
    const common_vertex_t vertexTypes,
    BufferObject& vbo,
    BufferObject& ibo,
    const Atlas& atlas
);

/**------------------------------------
 * @brief Retrieve a count of the number of characters which can be rendered
 * from OpenGL.
 * 
 * This function attempts to ignore all whitespace characters that cannot be
 * rendered through OpenGL.
 * 
 * @param str
 * A constant reference to a string of text whose characters will be analyzed
 * for renderability.
 * 
 * @return An unsigned integral type, containing the number of renderable
 * characters in the input string.
-------------------------------------*/
unsigned get_num_drawable_chars(const std::string& str);

/**------------------------------------
 * @brief
 * 
 * @param str
 * A constant reference to a string of text. The number of vertices, indices,
 * and bytes required for both will be derived from this string.
 * 
 * @param vertexTypes
 * A bitmask, containing a set of vertex types which will be taken into account
 * when determining the number of vertices and indices required to render a
 * string.
 * 
 * @param outNumVertices
 * A reference to an unsigned integer, which will contain the number of
 * vertices required to render a string.
 * 
 * @param outNumVertexBytes
 * A reference to an unsigned integer, which will contain the number of bytes
 * required by the sum of all vertices to generate a graphical representation
 * of a string.
 * 
 * @param outNumIndices
 * A reference to an unsigned integer, which will contain the number of indices
 * required to render the vertices for a string.
 * 
 * @param outNumIndexBytes
 * A reference to an unsigned integer, which will contain the number of bytes
 * required by the sum of all indices to reference the index data required to
 * render a string from OpenGL.
-------------------------------------*/
void calc_text_geometry_size(
    const std::string& str,
    const common_vertex_t vertexTypes,
    unsigned& outNumVertices,
    unsigned& outNumVertexBytes,
    unsigned& outNumIndices,
    unsigned& outNumIndexBytes
);

} // end draw namespace
} // end ls namespace

#endif /* __LS_DRAW_GEOMETRY_LOADER_H__ */