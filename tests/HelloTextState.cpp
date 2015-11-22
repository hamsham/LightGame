/* 
 * File:   HelloTextState.cpp
 * Author: miles
 * 
 * Created on October 27, 2015, 10:44 PM
 */

#include <cassert>

#include "lightsky/setup/macros.h"

#include "lightsky/utils/Pointer.h"

#include "lightsky/game/GameState.h"

#include "lightsky/draw/FontResource.h"
#include "lightsky/draw/ShaderObject.h"
#include "lightsky/draw/VertexAttrib.h"
#include "lightsky/draw/VertexUtils.h"

#include "HelloTextState.h"
#include "TextGeometryLoader.h"

#ifndef LS_GAME_TEST_FONT
    #define LS_GAME_TEST_FONT "testfont.ttf"
#endif

/*-------------------------------------
 * Destructor
-------------------------------------*/
HelloTextState::~HelloTextState() {
}

/*-------------------------------------
 * Contstructor
-------------------------------------*/
HelloTextState::HelloTextState() {
}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
HelloTextState::HelloTextState(HelloTextState&& s) :
    GameState{std::move(s)}
{}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
HelloTextState& HelloTextState::operator=(HelloTextState&& s) {
    GameState::operator=(std::move(s));

    return *this;
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::setup_camera() {
    
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::setup_shaders() {
    
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::setup_atlas() {
    ls::utils::Pointer<ls::draw::FontResource> pFont;

    pFont.reset(new ls::draw::FontResource{});

    assert(pFont->load_file(LS_GAME_TEST_FONT, 72));
    assert(atlas.init(*pFont));
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::setup_text() {
    using ls::draw::common_vertex_t;
    using ls::draw::vertex_data_t;
    
    common_vertex_t vertTypes = (common_vertex_t)(0 \
        | common_vertex_t::POSITION_VERTEX
        | common_vertex_t::TEXTURE_VERTEX
        | common_vertex_t::NORMAL_VERTEX
        | 0);
    
    const unsigned indicesLoaded = ls::draw::load_text_geometry(
        "Hello World!", vertTypes, this->vbo, this->ibo, this->atlas
    );

    assert(indicesLoaded > 0);
    assert(this->vao.init());
    
    this->vao.bind();
    
    ls::draw::VertexAttrib attribs[] = {
        ls::draw::create_vertex_attrib<vertex_data_t::VERTEX_DATA_VEC_3F>(),
        ls::draw::create_vertex_attrib<vertex_data_t::VERTEX_DATA_VEC_2F>(),
        ls::draw::create_vertex_attrib<vertex_data_t::VERTEX_DATA_VEC_3F>()
    };
    
    unsigned offset = 0;
    const unsigned stride = ls::draw::get_vertex_byte_size(vertTypes);
    
    for (unsigned i = 0; i < LS_ARRAY_SIZE(attribs); ++i) {
        ls::draw::VertexAttrib& attrib = attribs[i];
        
        attrib.index = i;
        attrib.offset = reinterpret_cast<void*>(offset);
        attrib.stride = stride;
        offset += get_num_attrib_bytes(attrib.type);
        
        this->vao.set_attrib_offset(attrib);
    }
    
    this->vao.unbind();
}

/*-------------------------------------
 * System Startup
-------------------------------------*/
bool HelloTextState::on_start() {
    setup_atlas();
    setup_shaders();
    setup_text();
    setup_camera();
    
    return true;
}

/*-------------------------------------
 * System Runtime
-------------------------------------*/
void HelloTextState::on_run() {
}

/*-------------------------------------
 * System Stop
-------------------------------------*/
void HelloTextState::on_stop() {
}

