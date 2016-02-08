/* 
 * File:   HelloTextState.cpp
 * Author: miles
 * 
 * Created on October 27, 2015, 10:44 PM
 */

#include <cassert>
#include <ctime>
#include <string>

#include "lightsky/setup/Macros.h"

#include "lightsky/math/Math.h"

#include "lightsky/utils/Log.h"
#include "lightsky/utils/Pointer.h"

#include "lightsky/game/GameState.h"
#include "lightsky/game/GameSystem.h"

#include "lightsky/draw/BlendObject.h"
#include "lightsky/draw/FontResource.h"
#include "lightsky/draw/ShaderObject.h"
#include "lightsky/draw/ShaderUniform.h"
#include "lightsky/draw/VertexAttrib.h"
#include "lightsky/draw/VertexUtils.h"

#include "HelloTextState.h"
#include "TextGeometryLoader.h"
#include "ControlState.h"

namespace math = ls::math;
using ls::draw::VertexAttrib;
using ls::draw::ShaderUniform;


#ifndef LS_GAME_TEST_FONT
    #define LS_GAME_TEST_FONT "testfont.ttf"
#endif
    
///////////////////////////////////////////////////////////////////////////////
//  Private Variables
///////////////////////////////////////////////////////////////////////////////
namespace {
/*-------------------------------------
 * Model Matrix Uniform Name
-------------------------------------*/
const char* TEXT_MODEL_MATRIX_UNIFORM_STR = "modelMatrix";
unsigned TEXT_MODEL_MATRIX_UNIFORM_ID = 0;

/*-------------------------------------
 * Camera/VP Matrix Uniform Name
-------------------------------------*/
const char* TEXT_VP_MATRIX_UNIFORM_STR = "vpMatrix";
unsigned TEXT_VP_MATRIX_UNIFORM_ID = 0;

/*-------------------------------------
 * Camera/VP Matrix Uniform Name
-------------------------------------*/
const char* TEXT_COLOR_UNIFORM_STR = "textColor";
unsigned TEXT_COLOR_UNIFORM_ID = 0;

/*-------------------------------------
 * Text Vertex Shader
-------------------------------------*/
constexpr char const* vData[] = {
u8R"***(
#version 330 core

precision mediump float;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 2) in vec3 inNorm;

uniform mat4 vpMatrix;
uniform mat4 modelMatrix;

out vec2 uvCoords;

void main() {
    mat4 mvp = vpMatrix * modelMatrix;
    gl_Position = mvp * vec4(inPos, 1.0);
    uvCoords = inUv;
}
)***"
};

/*-------------------------------------
 * Text Fragment Shader (with pseudo signed-distance field)
-------------------------------------*/
constexpr char const* fData[] = {
u8R"***(
#version 330 core

precision mediump float;

in vec2 uvCoords;

out vec4 outFragCol;

uniform sampler2D texSampler;
uniform vec4 textColor;

void main() {
    float mask = texture(texSampler, uvCoords).r;
    outFragCol = textColor*step(0.5, mask);
}
)***"
};
} // end anonymous namespace

/*-------------------------------------
 * Destructor
-------------------------------------*/
HelloTextState::~HelloTextState() {
}

/*-------------------------------------
 * Constructor
-------------------------------------*/
HelloTextState::HelloTextState() {
}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
HelloTextState::HelloTextState(HelloTextState&& state) :
    TestRenderState{}
{
    *this = std::move(state);
}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
HelloTextState& HelloTextState::operator=(HelloTextState&& state) {
    TestRenderState::operator=(std::move(state));
    
    pControlState = state.pControlState;
    pControlState->set_render_state(this);
    state.pControlState = nullptr;
    
    return *this;
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::setup_camera() {
    camera.set_projection_params(LS_DEG2RAD(60.f), 800.f, 600.f, 0.1f, 1000.f);
    camera.look_at(ls::math::vec3{1.0}, math::vec3{LS_EPSILON});
    camera.make_perspective();
    camera.lock_y_axis(true);
    camera.set_view_mode(ls::draw::camera_mode_t::ARCBALL);
    
    update_camera();
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::update_camera() {
    camera.update();
    
    shader.bind();
    LS_LOG_GL_ERR();
    
    ls::draw::set_shader_uniform(TEXT_MODEL_MATRIX_UNIFORM_ID, math::mat4{1.f});
    LS_LOG_GL_ERR();
    
    ls::draw::set_shader_uniform(TEXT_VP_MATRIX_UNIFORM_ID, camera.get_vp_matrix());
    LS_LOG_GL_ERR();
    
    ls::draw::set_shader_uniform(TEXT_COLOR_UNIFORM_ID, ls::math::vec4{0.f, 1.f, 0.f, 1.f});
    LS_LOG_GL_ERR();
    
    shader.unbind();
    LS_LOG_GL_ERR();
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::setup_shaders() {
    ls::draw::vertexShader vShader;
    ls::draw::fragmentShader fShader;
    
    if (!vShader.init(LS_ARRAY_SIZE(vData), vData)) {
        LS_LOG_GL_ERR();
        assert(false);
    }
    LS_LOG_GL_ERR();
    
    if (!fShader.init(LS_ARRAY_SIZE(fData), fData)) {
        LS_LOG_GL_ERR();
        assert(false);
    }
    LS_LOG_GL_ERR();
    
    if (!shader.init(vShader, fShader)) {
        LS_LOG_GL_ERR();
        assert(false);
    }
    LS_LOG_GL_ERR();
    
    if (!shader.link()) {
        LS_LOG_GL_ERR();
        assert(false);
    }
    LS_LOG_GL_ERR();
        
    const std::vector<ShaderUniform>&& uniforms = shader.get_uniforms();
    const std::vector<VertexAttrib>&& attribs = shader.get_attribs();
    unsigned i = 0;
    
    for (const VertexAttrib a : attribs) {
        LS_LOG_MSG("Shader Attribute ", i++,
            "\n\tName:       ", a.name,
            "\n\tIndex:      ", a.index,
            "\n\tComponents: ", a.components,
            "\n\tNormalized: ", (int)a.normalized,
            "\n\tStride:     ", a.stride,
            "\n\tOffset:     ", a.offset
        );
    }
    
    i = 0;
    for (const ShaderUniform u : uniforms) {
        LS_LOG_MSG("Shader Uniform ", i++,
            "\n\tName:       ", u.name,
            "\n\tIndex:      ", u.index,
            "\n\tComponents: ", u.components,
            "\n\tNormalized: ", (int)u.normalized,
            "\n\tStride:     ", u.stride,
            "\n\tOffset:     ", u.offset
        );
        
        if (u.name == TEXT_VP_MATRIX_UNIFORM_STR) {
            TEXT_VP_MATRIX_UNIFORM_ID = u.index;
        }
        else if (u.name == TEXT_MODEL_MATRIX_UNIFORM_STR) {
            TEXT_MODEL_MATRIX_UNIFORM_ID = u.index;
        }
        else if (u.name == TEXT_COLOR_UNIFORM_STR) {
            TEXT_COLOR_UNIFORM_ID = u.index;
        }
    }
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
void HelloTextState::set_text(const std::string& text) {
    using ls::draw::common_vertex_t;
    using ls::draw::STANDARD_VERTEX;
    
    static constexpr common_vertex_t vertTypes = STANDARD_VERTEX;
    
    numTextIndices = ls::draw::load_text_geometry(text, vertTypes, this->vbo, this->ibo, this->atlas);
    assert(numTextIndices > 0);
}

/*-------------------------------------
-------------------------------------*/
void HelloTextState::setup_text() {
    using ls::draw::common_vertex_t;
    using ls::draw::vertex_data_t;
    using ls::draw::STANDARD_VERTEX;
    
    assert(init_buffer(vbo, ls::draw::VBO_BUFFER_ARRAY));
    LS_LOG_GL_ERR();
    assert(init_buffer(ibo, ls::draw::VBO_BUFFER_ELEMENT));
    LS_LOG_GL_ERR();
    
    assert(this->vao.init());
    LS_LOG_GL_ERR();
    this->vao.bind();
    LS_LOG_GL_ERR();
    
    ls::draw::bind_buffer(vbo);
    LS_LOG_GL_ERR();
    ls::draw::bind_buffer(ibo);
    LS_LOG_GL_ERR();
    
    //set_text("Hello World!");
    set_text("Hello World!");
    LS_LOG_GL_ERR();
    
    this->vao.set_attrib_offsets(vbo.pAttribs.get(), vbo.numAttribs, ls::draw::get_vertex_byte_size(STANDARD_VERTEX));
    LS_LOG_GL_ERR();
    this->vao.unbind();
    LS_LOG_GL_ERR();
    
    ls::draw::unbind_buffer(vbo);
    LS_LOG_GL_ERR();
    ls::draw::unbind_buffer(ibo);
    LS_LOG_GL_ERR();
}

/*-------------------------------------
 * System Startup
-------------------------------------*/
bool HelloTextState::on_start() {
    using ls::draw::ShaderUniform;
    using ls::draw::VertexAttrib;
    
    setup_atlas();
    setup_shaders();
    setup_text();
    setup_camera();
    
    glDisable(GL_CULL_FACE);
    LS_LOG_GL_ERR();
    
    pControlState = new ControlState{};
    if (!pControlState) {
        LS_LOG_ERR("Error: Unable to create a hardware control state.");
        return false;
    }
    else {
        pControlState->set_render_state(this);
        ls::game::GameSystem& sys = get_parent_system();
        if (!sys.push_game_state(pControlState)) {
            LS_LOG_ERR("Error: Unable to start the hardware control state.");
            delete pControlState;
            return false;
        }
    }
    
    LS_LOG_GL_ERR();
    
    return true;
}

/*-------------------------------------
 * System Runtime
-------------------------------------*/
void HelloTextState::on_run() {
    ls::draw::BlendObject blender;
    blender.set_state(true);
    blender.set_blend_equation(ls::draw::BLEND_EQU_ADD, ls::draw::BLEND_EQU_ADD);
    blender.set_blend_function(
        ls::draw::BLEND_FNC_ONE, ls::draw::BLEND_FNC_1_SUB_SRC_ALPHA,
        ls::draw::BLEND_FNC_ONE, ls::draw::BLEND_FNC_ZERO
    );
    blender.bind();
    
    this->shader.bind();
    LS_LOG_GL_ERR();
    
    camera.update();
    ls::draw::set_shader_uniform(TEXT_VP_MATRIX_UNIFORM_ID, camera.get_vp_matrix());
    LS_LOG_GL_ERR();
    
    this->vao.bind();
    LS_LOG_GL_ERR();
    
    this->atlas.atlasTex.bind();
    LS_LOG_GL_ERR();
    
    glDrawElements(GL_TRIANGLES, numTextIndices, ls::draw::INDEX_TYPE_USHORT, nullptr);
    LS_LOG_GL_ERR();
    
    this->atlas.atlasTex.unbind();
    LS_LOG_GL_ERR();
    
    this->vao.unbind();
    LS_LOG_GL_ERR();
    
    this->shader.unbind();
    LS_LOG_GL_ERR();
    
    //blender.unbind();
}

/*-------------------------------------
 * System Stop
-------------------------------------*/
void HelloTextState::on_stop() {
    shader.terminate();
    vao.terminate();
    
    if (pControlState) {
        pControlState->set_state(ls::game::game_state_t::STOPPED);
        pControlState = nullptr;
    }
}
