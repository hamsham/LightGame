/* 
 * File:   draw/GeometryLoader.cpp
 * Author: miles
 * 
 * Created on August 3, 2015, 9:50 PM
 */

#include <cctype> // isgraph(...)

#include "lightsky/setup/macros.h"

#include "lightsky/utils/assertions.h"

#include "lightsky/math/vec2.h"
#include "lightsky/math/vec3.h"

#include "lightsky/draw/Atlas.h"
#include "lightsky/draw/BufferObject.h"
#include "lightsky/draw/VertexAttrib.h"
#include "lightsky/draw/VertexUtils.h"

//#include "lightsky/draw/TextGeometryLoader.h"
#include "TextGeometryLoader.h"

/*-----------------------------------------------------------------------------
 * Anonymous, Private Functions
-----------------------------------------------------------------------------*/
namespace {

using ls::math::vec2;
using ls::math::vec3;

using ls::draw::Atlas;
using ls::draw::AtlasEntry;
using ls::draw::BufferObject;
using ls::draw::common_vertex_t;
using ls::draw::vertex_data_t;
using ls::draw::get_num_attrib_bytes;
using ls::draw::index_element_t;

using ls::draw::MESH_SPACES_PER_TAB;
using ls::draw::MESH_INDICES_PER_GLYPH;
using ls::draw::INDEX_TYPE_UINT;

constexpr ls::draw::buffer_map_t DEFAULT_TEXT_MAPPING_FLAGS = (ls::draw::buffer_map_t)(0
    | ls::draw::VBO_MAP_BIT_INVALIDATE_RANGE
    | ls::draw::VBO_MAP_BIT_UNSYNCHRONIZED
    | ls::draw::VBO_MAP_BIT_INVALIDATE_BUFFER
    | ls::draw::VBO_MAP_BIT_WRITE
    | 0);

/*-------------------------------------
 * Calculate the vertex positions that a glyph should represent.
-------------------------------------*/
unsigned calc_text_geometry_pos(
    const float xOffset,
    const float yOffset,
    const AtlasEntry& rGlyph,
    char* const pPos,
    const unsigned stride
) {
    char* pVert = pPos;
    
    *reinterpret_cast<vec3*>(pVert) = vec3{xOffset, yOffset+rGlyph.size[1], 0.f};
    pVert += stride;
    *reinterpret_cast<vec3*>(pVert) = vec3{xOffset, yOffset, 0.f};
    pVert += stride;
    *reinterpret_cast<vec3*>(pVert) = vec3{xOffset+rGlyph.size[0], yOffset+rGlyph.size[1], 0.f};
    pVert += stride;
    *reinterpret_cast<vec3*>(pVert) = vec3{xOffset+rGlyph.size[0],yOffset, 0.f};
    
    return get_num_attrib_bytes((vertex_data_t)common_vertex_t::POSITION_VERTEX);
}

/*-------------------------------------
 * Function to calculate the UVs which should represent a glyph.
-------------------------------------*/
unsigned calc_text_geometry_uvs(
    const AtlasEntry& rGlyph,
    char* const pUv,
    const unsigned stride
) {
    char* pVert = pUv;
    
    *reinterpret_cast<vec2*>(pVert) = vec2{rGlyph.uv[0][0], rGlyph.uv[0][1]};
    pVert += stride;
    *reinterpret_cast<vec2*>(pVert) = vec2{rGlyph.uv[0][0], rGlyph.uv[1][1]};
    pVert += stride;
    *reinterpret_cast<vec2*>(pVert) = vec2{rGlyph.uv[1][0], rGlyph.uv[0][1]};
    pVert += stride;
    *reinterpret_cast<vec2*>(pVert) = vec2{rGlyph.uv[1][0], rGlyph.uv[1][1]};
    
    return get_num_attrib_bytes((vertex_data_t)common_vertex_t::TEXTURE_VERTEX);
}

/*-------------------------------------
 * Function to calculate the UVs which should represent a glyph.
-------------------------------------*/
unsigned calc_text_geometry_norms(
    char* const pNrm,
    const unsigned stride,
    const vec3& normDir
) {
    char* pVert = pNrm;
    
    *reinterpret_cast<vec3*>(pVert) = normDir;
    pVert += stride;
    *reinterpret_cast<vec3*>(pVert) = normDir;
    pVert += stride;
    *reinterpret_cast<vec3*>(pVert) = normDir;
    pVert += stride;
    *reinterpret_cast<vec3*>(pVert) = normDir;
    
    return get_num_attrib_bytes((vertex_data_t)common_vertex_t::NORMAL_VERTEX);
}

/*-------------------------------------
 * Function to dispatch all text-loading responsibilities to their respective loaders.
-------------------------------------*/
char* gen_text_geometry_vert(
    const ls::draw::AtlasEntry& rGlyph,
    char* pVert,
    unsigned stride,
    const float xOffset,
    const float yOffset,
    const ls::draw::common_vertex_t vertTypes
) {
    using ls::math::vec2;
    using ls::math::vec3;
    using ls::draw::common_vertex_t;
    
    if (vertTypes & common_vertex_t::POSITION_VERTEX) {
        pVert += calc_text_geometry_pos(xOffset, yOffset, rGlyph, pVert, stride);
    }
    
    if (vertTypes & common_vertex_t::TEXTURE_VERTEX) {
        pVert += calc_text_geometry_uvs(rGlyph, pVert, stride);
    }
    
    if (vertTypes & common_vertex_t::NORMAL_VERTEX) {
        pVert += calc_text_geometry_norms(pVert, stride, vec3{0.f, 0.f, 1.f});
    }
    
    if (vertTypes & common_vertex_t::TANGENT_VERTEX) {
        pVert += calc_text_geometry_norms(pVert, stride, vec3{1.f, 0.f, 0.f});
    }
    
    if (vertTypes & common_vertex_t::BITANGENT_VERTEX) {
        pVert += calc_text_geometry_norms(pVert, stride, vec3{0.f, 1.f, 0.f});
    }
    
    if (vertTypes & common_vertex_t::MODEL_MAT_VERTEX) {
        LS_ASSERT(false);
    }
    
    if (vertTypes & common_vertex_t::BONE_ID_VERTEX) {
        LS_ASSERT(false);
    }
    
    if (vertTypes & common_vertex_t::BONE_WEIGHT_VERTEX) {
        LS_ASSERT(false);
    }
    
    return pVert + stride;
}

/*-------------------------------------
 * Set the index data required by geometry text.
-------------------------------------*/
char* set_text_geometry_indices(
    char* pIndices,
    const index_element_t indexType,
    const unsigned indexStride,
    const unsigned indexOffset
) {
    const unsigned indices[] = {
        indexOffset+0,
        indexOffset+1,
        indexOffset+2,
        indexOffset+2,
        indexOffset+1,
        indexOffset+3
    };
    
    if (indexType == index_element_t::INDEX_TYPE_USHORT) {
        for (unsigned i = 0; i < LS_ARRAY_SIZE(indices); ++i) {
            unsigned short* pUshort = (unsigned short*)pIndices;
            *pUshort = indices[i];
            pIndices += indexStride;
        }
    }
    else {
        for (unsigned i = 0; i < LS_ARRAY_SIZE(indices); ++i) {
            unsigned int* pUint = (unsigned int*)pIndices;
            *pUint = indices[i];
            pIndices += indexStride;
        }
    }
    
    return pIndices;
}

/*-------------------------------------
 * Text/String Generation
-------------------------------------*/
unsigned gen_text_geometry(
    const std::string& str,
    const common_vertex_t vertexTypes,
    BufferObject& vbo,
    BufferObject& ibo,
    const Atlas& atlas
) {
    using ls::draw::get_num_drawable_chars;
    using ls::draw::get_vertex_byte_size;
    using ls::draw::geometry_property_t;
    using ls::draw::map_buffer_data;
    using ls::draw::get_required_index_type;
    
    const unsigned numDrawable      = get_num_drawable_chars(str);
    const unsigned vertStride       = get_vertex_byte_size(vertexTypes);
    const unsigned totalVerts       = numDrawable*geometry_property_t::MESH_VERTS_PER_GLYPH;
    char* pVerts                    = (char*)map_buffer_data(vbo, 0, totalVerts, DEFAULT_TEXT_MAPPING_FLAGS);
    const index_element_t indexType = get_required_index_type(totalVerts);
    const unsigned indexStride      = (indexType == INDEX_TYPE_UINT) ? sizeof(unsigned int) : sizeof(unsigned short);
    unsigned totalIndices           = 0;
    char* pIndices                  = (char*)map_buffer_data(ibo, 0, numDrawable*indexStride, DEFAULT_TEXT_MAPPING_FLAGS);
    const AtlasEntry* const pGlyphs = atlas.pEntries; // Get pointers to the buffer data that will be filled with quads
    
    // The y-origin was found using a lot of testing. This was for resolution independence
    constexpr unsigned nl = (unsigned)'\n';
    float yPos = -((pGlyphs[nl].bearing[1]*2.f)+pGlyphs[nl].bearing[1]-pGlyphs[nl].size[1]);
    float xPos = 0.f;
    
    for (unsigned i = 0; i < str.size(); ++i) {
        const unsigned currChar = (unsigned)str[i];
        const AtlasEntry& rGlyph = pGlyphs[currChar];
        const float vertHang = (rGlyph.bearing[1]-rGlyph.size[1]);
        
        if (currChar == '\n') {
            yPos -= (rGlyph.bearing[1]*2.f)+vertHang; // formula found through trial and error
            xPos = 0.f;
        }
        else if (currChar == '\v') {
            yPos -= ((rGlyph.bearing[1]*2.f)+vertHang)*MESH_SPACES_PER_TAB;
            xPos = 0.f;
        }
        else if (currChar == '\r') {
            xPos = 0.f;
        }
        else if (currChar == ' ') {
            xPos += rGlyph.advance[0];
        }
        else if (currChar == '\t') {
            xPos += rGlyph.advance[0]*MESH_SPACES_PER_TAB;
        }
        else {
            const float yOffset = yPos+vertHang;
            const float xOffset = xPos+rGlyph.bearing[0];
            xPos += rGlyph.advance[0];
            pVerts = gen_text_geometry_vert(rGlyph, pVerts, vertStride, xOffset, yOffset, vertexTypes);
            pIndices = set_text_geometry_indices(pIndices, indexType, indexStride, totalIndices);
            totalIndices += MESH_INDICES_PER_GLYPH;
        }
    }
    
    return totalIndices;
}

} // end anonymous namespace

/*-----------------------------------------------------------------------------
 * Geometry Functions
-----------------------------------------------------------------------------*/
namespace ls {
namespace draw {

/*-------------------------------------
 * Text/String loading
-------------------------------------*/
unsigned load_text_geometry(
    const std::string& str,
    const common_vertex_t vertexTypes,
    BufferObject& vbo,
    BufferObject& ibo,
    const Atlas& atlas
) {
    LS_LOG_MSG("Attempting to load text geometry.");
    
    // determine the number of non-whitespace characters
    unsigned numVertices = 0;
    unsigned numVertexBytes = 0;
    unsigned numIndices = 0;
    unsigned numIndexBytes = 0;
    calc_text_geometry_size(str, vertexTypes, numVertices, numVertexBytes, numIndices, numIndexBytes);
    
    // insurance/ghetto cleanup
    auto terminate_text_buffers = [&]()->void {terminate_buffer(vbo); terminate_buffer(ibo);};
    
    // Attempt to get a pointer to an unsynchronized memory buffer
    if (!init_buffer(vbo) || !init_buffer(ibo)) {
        LS_LOG_ERR("\tUnable to allocate memory for text geometry.\n");
        terminate_text_buffers();
        return 0;
    }
    
    if (!ls::draw::setup_vertex_buffer_attribs(vbo, vertexTypes)) {
        LS_LOG_ERR("\tUnable to initialize text geometry attribute meta-data.\n");
        terminate_text_buffers();
        return 0;
    }
    
    bind_buffer(vbo);
    set_buffer_data(vbo, numVertexBytes, nullptr, buffer_access_t::VBO_STREAM_DRAW);
    LOG_GL_ERR();

    bind_buffer(ibo);
    set_buffer_data(ibo, numIndexBytes, nullptr, buffer_access_t::VBO_STREAM_DRAW);
    LOG_GL_ERR();
    
    gen_text_geometry(str, vertexTypes, vbo, ibo, atlas);
    LOG_GL_ERR();
    
    unbind_buffer(vbo);
    unbind_buffer(ibo);
    LOG_GL_ERR();
    
    LS_LOG_MSG(
        "\tSuccessfully sent a string to the GPU.",
        "\n\t\tVertices:    ", numVertices,
        "\n\t\tIndices:     ", numIndices,
        '\n'
    );
    
    return numIndices;
}

/*-------------------------------------
 * Utility function to get all of the non-whitespace characters in a string
-------------------------------------*/
unsigned get_num_drawable_chars(const std::string& str) {
    unsigned charCount = 0;
    
	for (unsigned i = 0; i < str.size(); ++i) {
        if (isgraph(str[i])) {
            ++charCount;
        }
	}
    
    return charCount;
}

/*-------------------------------------
 * Determine how many bytes are required to populate a vertex buffer and index
 * buffer of textual glyphs.
-------------------------------------*/
void calc_text_geometry_size(
    const char* const str,
    const common_vertex_t vertexTypes,
    unsigned& outNumVertices,
    unsigned& outNumVertexBytes,
    unsigned& outNumIndices,
    unsigned& outNumIndexBytes
) {
    const unsigned numChars = get_num_drawable_chars(str);
    
    const unsigned vertexBytes = get_vertex_byte_size(vertexTypes);
    outNumVertices = numChars * geometry_property_t::MESH_VERTS_PER_GLYPH;
    outNumVertexBytes = outNumVertices * vertexBytes;
    
    const unsigned indexBytes =
        get_required_index_type(outNumVertices) == index_element_t::INDEX_TYPE_UINT
            ? sizeof(unsigned int)
            : sizeof(unsigned short);
    outNumIndices = numChars * geometry_property_t::MESH_INDICES_PER_GLYPH;
    outNumIndexBytes = outNumIndices * indexBytes;
}

} // end draw namespace
} // end ls namespace
