/* 
 * File:   draw/GeometryLoader.cpp
 * Author: miles
 * 
 * Created on August 3, 2015, 9:50 PM
 */

#include <cctype> // isgraph(...)

#include "lightsky/setup/Macros.h"

#include "lightsky/utils/Assertions.h"
#include "lightsky/utils/Log.h"

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

// draw objects
using ls::draw::Atlas;
using ls::draw::AtlasEntry;
using ls::draw::BufferObject;
using ls::draw::TextMetaData;

// draw enums
using ls::draw::common_vertex_t;
using ls::draw::index_element_t;
using ls::draw::vertex_data_t;
using ls::draw::buffer_access_t;

// draw functions
using ls::draw::get_num_attrib_bytes;
using ls::draw::get_vertex_byte_size;

// draw constants
using ls::draw::TEXT_SPACES_PER_TAB;
using ls::draw::TEXT_VERTS_PER_GLYPH;
using ls::draw::TEXT_INDICES_PER_GLYPH;
using ls::draw::TEXT_LINE_SPACING;
using ls::draw::INDEX_TYPE_UINT;

constexpr ls::draw::buffer_map_t DEFAULT_TEXT_MAPPING_FLAGS = (ls::draw::buffer_map_t)(0
    | ls::draw::VBO_MAP_BIT_INVALIDATE_RANGE
    | ls::draw::VBO_MAP_BIT_UNSYNCHRONIZED
    | ls::draw::VBO_MAP_BIT_INVALIDATE_BUFFER
    | ls::draw::VBO_MAP_BIT_WRITE
    | 0);


/*-------------------------------------
 * Calculate a portion of vertex data that a glyph should contain.
-------------------------------------*/
template <typename data_t>
inline char* set_text_vertex_data(
    char* const     pVert,
    const unsigned  stride,
    const data_t&   data
) {
    *reinterpret_cast<data_t* const>(pVert) = data;
    return pVert + stride;
}

/*-------------------------------------
 * Helper function to allocate and map an initialized buffer object.
-------------------------------------*/
char* allocate_and_map_buffer(BufferObject& buf, const unsigned numBytes) {
    // VBO Allocation
    set_buffer_data(buf, numBytes, nullptr, buffer_access_t::VBO_STREAM_DRAW);
    LS_LOG_GL_ERR();

    // VBO Mapping
    char* const pBuffer = (char*)map_buffer_data(buf, 0, numBytes, DEFAULT_TEXT_MAPPING_FLAGS);
    LS_LOG_GL_ERR();

    if (!pBuffer) {
        LS_LOG_ERR("\tAn error occurred while attempting to map a BufferObject for text geometry.");
        return nullptr;
    }

    return pBuffer;
}

/*-------------------------------------
 * Calculate the vertex positions that a glyph should represent.
-------------------------------------*/
unsigned calc_text_geometry_pos(
    const AtlasEntry&   rGlyph,
    char*               pVert,
    const unsigned      stride,
    const float         xOffset,
    const float         yOffset
) {
    pVert = set_text_vertex_data(pVert, stride, vec3{xOffset, yOffset-rGlyph.size[1], 0.f});
    pVert = set_text_vertex_data(pVert, stride, vec3{xOffset, yOffset, 0.f});
    pVert = set_text_vertex_data(pVert, stride, vec3{xOffset+rGlyph.size[0], yOffset-rGlyph.size[1], 0.f});
            set_text_vertex_data(pVert, stride, vec3{xOffset+rGlyph.size[0],yOffset, 0.f});
    
    //return byte-stride to the next vertex attrib
    static const unsigned vertOffset = get_vertex_byte_size(common_vertex_t::POSITION_VERTEX);
    return vertOffset;
}

/*-------------------------------------
 * Function to calculate the UVs which should represent a glyph.
-------------------------------------*/
unsigned calc_text_geometry_uvs(
    const AtlasEntry&   rGlyph,
    char*               pVert,
    const unsigned      stride
) {
    pVert = set_text_vertex_data(pVert, stride, vec2{rGlyph.uv[0][0], rGlyph.uv[0][1]});
    pVert = set_text_vertex_data(pVert, stride, vec2{rGlyph.uv[0][0], rGlyph.uv[1][1]});
    pVert = set_text_vertex_data(pVert, stride, vec2{rGlyph.uv[1][0], rGlyph.uv[0][1]});
            set_text_vertex_data(pVert, stride, vec2{rGlyph.uv[1][0], rGlyph.uv[1][1]});
    
    //return byte-stride to the next vertex attrib
    static const unsigned vertOffset = get_vertex_byte_size(common_vertex_t::TEXTURE_VERTEX);
    return vertOffset;
}

/*-------------------------------------
 * Function to calculate the UVs which should represent a glyph.
-------------------------------------*/
unsigned calc_text_geometry_norms(
    char*           pVert,
    const unsigned  stride,
    const vec3&     normDir
) {
    pVert = set_text_vertex_data(pVert, stride, normDir);
    pVert = set_text_vertex_data(pVert, stride, normDir);
    pVert = set_text_vertex_data(pVert, stride, normDir);
            set_text_vertex_data(pVert, stride, normDir);
    
    //return byte-stride to the next vertex attrib
    static const unsigned vertOffset = get_vertex_byte_size(common_vertex_t::NORMAL_VERTEX);
    return vertOffset;
}

/*-------------------------------------
 * Function to dispatch all text-loading responsibilities to their respective loaders.
-------------------------------------*/
char* gen_text_geometry_vert(
    const AtlasEntry&   rGlyph,
    char* const         pData,
    const float         xOffset,
    const float         yOffset,
    const TextMetaData& metaData
) {
    const common_vertex_t vertTypes = metaData.vertTypes;
    char* pVert = pData;
    
    if (vertTypes & common_vertex_t::POSITION_VERTEX) {
        pVert += calc_text_geometry_pos(rGlyph, pVert, metaData.vertStride, xOffset, yOffset);
    }
    
    if (vertTypes & common_vertex_t::TEXTURE_VERTEX) {
        pVert += calc_text_geometry_uvs(rGlyph, pVert, metaData.vertStride);
    }
    
    if (vertTypes & common_vertex_t::NORMAL_VERTEX) {
        pVert += calc_text_geometry_norms(pVert, metaData.vertStride, vec3{0.f, 0.f, 1.f});
    }
    
    if (vertTypes & common_vertex_t::TANGENT_VERTEX) {
        pVert += calc_text_geometry_norms(pVert, metaData.vertStride, vec3{1.f, 0.f, 0.f});
    }
    
    if (vertTypes & common_vertex_t::BITANGENT_VERTEX) {
        pVert += calc_text_geometry_norms(pVert, metaData.vertStride, vec3{0.f, 1.f, 0.f});
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
    
    return pData + (metaData.vertStride * TEXT_VERTS_PER_GLYPH);
}

/*-------------------------------------
 * Set the index data required by geometry text (helper function).
-------------------------------------*/
template <typename data_t>
char* fill_geometry_indices(void* const pIndices, const unsigned indexOffset) {
    data_t* pData = reinterpret_cast<data_t*>(pIndices);
    
    *(pData++) = indexOffset+0;
    *(pData++) = indexOffset+1;
    *(pData++) = indexOffset+2;
    *(pData++) = indexOffset+2;
    *(pData++) = indexOffset+1;
    *(pData++) = indexOffset+3;
    
    return reinterpret_cast<char*>(pData);
}

/*-------------------------------------
 * Set the index data required by geometry text.
-------------------------------------*/
inline char* set_text_geometry_indices(
    char* pIndices,
    const unsigned indexOffset,
    const TextMetaData& metaData
) {
    if (metaData.indexType == index_element_t::INDEX_TYPE_USHORT) {
        return fill_geometry_indices<unsigned short>(pIndices, indexOffset);
    }
    
    return fill_geometry_indices<unsigned int>(pIndices, indexOffset);
}

/*-------------------------------------
 * Text/String Generation
-------------------------------------*/
void gen_text_geometry(
    const std::string& str,
    char* pVerts,
    char* pIndices,
    const Atlas& atlas,
    const TextMetaData& metaData
) {
    
    // Get pointers to the buffer data that will be filled with quads
    const AtlasEntry* const pGlyphs = atlas.pEntries;
    constexpr float lineSpacing = float{TEXT_LINE_SPACING};
    
    // The y-origin (starting 'yPos') was found using a lot of testing. This
    // was for resolution independence
    constexpr unsigned nl = (unsigned)'\n';
    float yPos = pGlyphs[nl].bearing[1] - (pGlyphs[nl].bearing[1]-pGlyphs[nl].size[1]);
    float xPos = 0.f;
    unsigned indexOffset = 0;
    
    for (unsigned i = 0; i < str.size(); ++i) {
        const unsigned currChar = (unsigned)str[i];
        const AtlasEntry& rGlyph = pGlyphs[currChar];
        
        // Amount the each glyph "hangs" below its Y-origin
        const float vertHang = rGlyph.bearing[1] - rGlyph.size[1];
        
        if (currChar == u' ') {
            xPos += rGlyph.advance[0];
        }
        else if (currChar == u'\t') {
            xPos += rGlyph.advance[0]*TEXT_SPACES_PER_TAB;
        }
        else if (currChar == u'\n') {
            // formula found through trial and error.
            // TODO: The numerical constant '2.f' stands for double-point line
            // spacing. Extract this function into a class so this can be
            // adjusted as a member variable.
            yPos += (rGlyph.bearing[1] + lineSpacing) + vertHang;
            xPos = 0.f;
        }
        else if (currChar == u'\r') {
            xPos = 0.f;
        }
        else if (currChar == u'\v') {
            yPos += ((rGlyph.bearing[1] + lineSpacing) + vertHang) * TEXT_SPACES_PER_TAB;
        }
        else {
            const float yOffset = yPos - vertHang;
            const float xOffset = xPos + rGlyph.bearing[0];
            xPos                = xPos + rGlyph.advance[0];
            pVerts              = gen_text_geometry_vert(rGlyph, pVerts, xOffset, yOffset, metaData);
            pIndices            = set_text_geometry_indices(pIndices, indexOffset, metaData);
            indexOffset         = indexOffset + TEXT_VERTS_PER_GLYPH;
        }
    }
}

} // end anonymous namespace

/*-----------------------------------------------------------------------------
 * Geometry Functions
-----------------------------------------------------------------------------*/
namespace ls {

/*-------------------------------------
 * Meta information about the text being generated.
-------------------------------------*/
void draw::gen_text_meta_data(
    TextMetaData& metaData,
    const std::string& str,
    const common_vertex_t vertexTypes
) {
    metaData.numDrawableChars   = get_num_drawable_chars(str);
    
    metaData.vertTypes          = vertexTypes;
    metaData.vertStride         = get_vertex_byte_size(metaData.vertTypes);
    metaData.totalVerts         = metaData.numDrawableChars * geometry_property_t::TEXT_VERTS_PER_GLYPH;
    metaData.totalVertBytes     = metaData.totalVerts * metaData.vertStride;
    
    metaData.indexType          = get_required_index_type(metaData.totalVerts);
    metaData.indexByteSize      = get_index_byte_size(metaData.indexType);
    metaData.totalIndices       = metaData.numDrawableChars * geometry_property_t::TEXT_INDICES_PER_GLYPH;
    metaData.totalIndexBytes    = metaData.totalIndices * metaData.indexByteSize;
    
    LS_LOG_MSG(
        "Text Geometry Meta Data:",
        "\n\tBytes Per Vertex:  ", metaData.vertStride,
        "\n\tVertex Count:      ", metaData.totalVerts,
        "\n\tVertex Bytes:      ", metaData.totalVertBytes,
        "\n\tBytes Per Index:   ", metaData.indexByteSize,
        "\n\tIndex Count:       ", metaData.totalIndices,
        "\n\tIndex Bytes:       ", metaData.totalIndexBytes
    );
}

/*-------------------------------------
 * Text/String loading
 * 
 * NOTE: Remove the buffer initialization, allocation, and the DMA mapping to
 * make this function thread-safe-ish.
-------------------------------------*/
unsigned draw::load_text_geometry(
    const std::string&      str,
    const common_vertex_t   vertexTypes,
    BufferObject&           vbo,
    BufferObject&           ibo,
    const Atlas&            atlas
) {
    LS_LOG_MSG("Attempting to load text geometry.");
    char* pVerts = nullptr;
    char* pIndices = nullptr;
    
    TextMetaData metaData;
    gen_text_meta_data(metaData, str, vertexTypes);
    
    // Initialize VBO
    if (!ls::draw::setup_vertex_buffer_attribs(vbo, metaData.vertTypes)) {
        LS_LOG_ERR("\tUnable to initialize text geometry meta-data.\n");
        return 0;
    }
    
    // VBO Mapping
    pVerts = allocate_and_map_buffer(vbo, metaData.totalVertBytes);
    if (!pVerts) {
        LS_LOG_ERR("\tAn error occurred while attempting to map a VBO for text geometry.");
        return false;
    }
    
    // Initialize IBO
    if (!ls::draw::setup_index_buffer_attribs(ibo, metaData.indexType)) {
        unmap_buffer_data(vbo);
        LS_LOG_ERR("\tUnable to initialize text index meta-data.\n");
        return 0;
    }
    
    // IBO Mapping
    pIndices = allocate_and_map_buffer(ibo, metaData.totalVertBytes);
    if (!pIndices) {
        unmap_buffer_data(vbo);
        LS_LOG_ERR("\tAn error occurred while attempting to map an IBO for text geometry.");
        return false;
    }
    
    // Generate the text geometry
    gen_text_geometry(str, pVerts, pIndices, atlas, metaData);
    
    unmap_buffer_data(vbo);
    unmap_buffer_data(ibo);
    
    LS_LOG_MSG(
        "\tSuccessfully sent a string to the GPU.",
        "\n\t\tCharacters:  ", metaData.numDrawableChars,
        "\n\t\tVertices:    ", metaData.totalVerts,
        "\n\t\tVert Size:   ", metaData.totalVertBytes, " bytes"
        "\n\t\tIndices:     ", metaData.totalIndices,
        "\n\t\tIndex Size:  ", metaData.totalIndexBytes, " bytes"
        "\n\t\tTotal Size:  ", metaData.totalVertBytes+metaData.totalIndexBytes, " bytes",
        '\n'
    );
    
    return metaData.totalIndices;
}

/*-------------------------------------
 * Utility function to get all of the non-whitespace characters in a string
-------------------------------------*/
unsigned draw::get_num_drawable_chars(const std::string& str) {
    unsigned charCount = 0;
    
	for (unsigned i = 0; i < str.size(); ++i) {
        if (isgraph(str[i])) {
            ++charCount;
        }
	}
    
    return charCount;
}

} // end ls namespace
