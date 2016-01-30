/* 
 * File:   HelloPrimState.cpp
 * Author: miles
 * 
 * Created on January 29, 2015
 */

#include <cassert>
#include <ctime>
#include <memory>
#include <string>

#include "lightsky/setup/Macros.h"

#include "lightsky/math/Math.h"

#include "lightsky/utils/Log.h"
#include "lightsky/utils/Pointer.h"

#include "lightsky/game/GameState.h"
#include "lightsky/game/GameSystem.h"

#include "lightsky/draw/FontResource.h"
#include "lightsky/draw/ShaderObject.h"
#include "lightsky/draw/ShaderUniform.h"
#include "lightsky/draw/VertexAttrib.h"
#include "lightsky/draw/VertexUtils.h"

#include "HelloPrimState.h"
#include "ControlState.h"

namespace math = ls::math;
    
///////////////////////////////////////////////////////////////////////////////
//  Private Variables
///////////////////////////////////////////////////////////////////////////////
namespace {
/*-------------------------------------
 * Model Matrix Uniform Name
-------------------------------------*/
//const char* TEXT_MODEL_MATRIX_UNIFORM = "modelMatrix";

/*-------------------------------------
 * Camera/VP Matrix Uniform Name
-------------------------------------*/
//const char* TEXT_VP_MATRIX_UNIFORM = "vpMatrix";

/*-------------------------------------
 * Camera/VP Matrix Uniform Name
-------------------------------------*/
//const char* TEXT_COLOR_UNIFORM = "primColor";

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

uniform vec4 primColor;

in vec2 uvCoords;

out vec4 outFragCol;

void main() {
    outFragCol = primColor;
}
)***"
};
} // end anonymous namespace

/*-------------------------------------
 * Destructor
-------------------------------------*/
HelloPrimState::~HelloPrimState() {
}

/*-------------------------------------
 * Constructor
-------------------------------------*/
HelloPrimState::HelloPrimState() {
}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
HelloPrimState::HelloPrimState(HelloPrimState&& state) :
    GameState{}
{
    *this = std::move(state);
}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
HelloPrimState& HelloPrimState::operator=(HelloPrimState&& state) {
    GameState::operator=(std::move(state));
    
    pControlState = state.pControlState;
    pControlState->set_render_state(this);
    state.pControlState = nullptr;
    
    return *this;
}

/*-------------------------------------
-------------------------------------*/
void HelloPrimState::setup_camera() {
    camera.set_projection_params(LS_DEG2RAD(60.f), 800.f, 600.f, 0.1f, 1000.f);
    camera.look_at(ls::math::vec3{20.0}, ls::math::vec3{LS_EPSILON});
    camera.make_perspective();
    camera.lock_y_axis(true);
    camera.set_view_mode(ls::draw::camera_mode_t::ARCBALL);
    
    update_camera();
}

/*-------------------------------------
-------------------------------------*/
void HelloPrimState::update_camera() {
    camera.update();
    
    shader.bind();
    ls::draw::set_shader_uniform(0, math::scale(math::mat4{1.f}, math::vec3{10.f}));
    ls::draw::set_shader_uniform(1, ls::math::vec4{0.f, 1.f, 0.f, 1.f});
    ls::draw::set_shader_uniform(2, camera.get_vp_matrix());
    shader.unbind();
}

/*-------------------------------------
-------------------------------------*/
void HelloPrimState::setup_shaders() {
    ls::draw::vertexShader vShader;
    ls::draw::fragmentShader fShader;
    
    if (!vShader.init(LS_ARRAY_SIZE(vData), vData)) {
        LS_LOG_GL_ERR();
        assert(false);
    }
    
    if (!fShader.init(LS_ARRAY_SIZE(fData), fData)) {
        LS_LOG_GL_ERR();
        assert(false);
    }
    
    if (!shader.init(vShader, fShader)) {
        LS_LOG_GL_ERR();
        assert(false);
    }
    
    if (!shader.link()) {
        LS_LOG_GL_ERR();
        assert(false);
    }
}

/*-------------------------------------
-------------------------------------*/
std::unique_ptr<char[]> HelloPrimState::gen_vertex_data() {
    static constexpr math::vec3 positions[3] = {
        math::vec3{-1.f, -1.f, 0.f},
        math::vec3{1.f, -1.f, 0.f},
        math::vec3{0.f, 1.f, 0.f},
    };
    
    
    static constexpr math::vec2 textures[3] = {
        math::vec2{0.f, 0.f},
        math::vec2{1.f, 0.f},
        math::vec2{0.5f, 1.f}
    };
    
    static constexpr math::vec3 normals[3] = {
        math::vec3{0.f, 0.f, -1.f},
        math::vec3{0.f, 0.f, -1.f},
        math::vec3{0.f, 0.f, -1.f}
    };
    
    static constexpr unsigned stride = sizeof(math::vec3) + sizeof(math::vec2) + sizeof(math::vec3);
    std::unique_ptr<char[]> pData{new char[stride * 3]};
    
    
    for (unsigned i = 0; i < 3; ++i) {
        static constexpr unsigned posOffset = 0;
        static constexpr unsigned texOffset = sizeof(math::vec3);
        static constexpr unsigned nrmOffset = texOffset + sizeof(math::vec2);
        
        *reinterpret_cast<math::vec3*>(pData.get() + (i*stride) + posOffset) = positions[i];
        *reinterpret_cast<math::vec2*>(pData.get() + (i*stride) + texOffset) = textures[i];
        *reinterpret_cast<math::vec3*>(pData.get() + (i*stride) + nrmOffset) = normals[i];
    }
    
    return pData;
}

/*-------------------------------------
-------------------------------------*/
void HelloPrimState::setup_prims() {
    using ls::draw::common_vertex_t;
    using ls::draw::vertex_data_t;
    using ls::draw::STANDARD_VERTEX;
    
    assert(this->vao.init());
    this->vao.bind();
    
    ls::draw::VertexAttrib vboAttribs[] = {
        ls::draw::create_vertex_attrib<vertex_data_t::VERTEX_DATA_VEC_3F>(),
        ls::draw::create_vertex_attrib<vertex_data_t::VERTEX_DATA_VEC_2F>(),
        ls::draw::create_vertex_attrib<vertex_data_t::VERTEX_DATA_VEC_3F>()
    };
    const unsigned numVboAttribs = LS_ARRAY_SIZE(vboAttribs);
    
    assert(ls::draw::init_buffer(this->vbo));
    ls::draw::bind_buffer(this->vbo);
    const std::unique_ptr<char[]>&& pData = gen_vertex_data();
    ls::draw::set_buffer_data(vbo, 3 * ls::draw::get_vertex_byte_size((common_vertex_t)STANDARD_VERTEX), pData.get(), ls::draw::buffer_access_t::VBO_STATIC_DRAW);
    this->vao.set_attrib_offsets(vboAttribs, numVboAttribs, ls::draw::get_vertex_byte_size((common_vertex_t)STANDARD_VERTEX));
    
    this->vao.unbind();
    
    ls::draw::unbind_buffer(vbo);
}

/*-------------------------------------
 * System Startup
-------------------------------------*/
bool HelloPrimState::on_start() {
    using ls::draw::ShaderUniform;
    using ls::draw::VertexAttrib;
    
    setup_shaders();
    setup_prims();
    setup_camera();
    
    glDisable(GL_CULL_FACE);
    
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
    }
    
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
    
    return true;
}

/*-------------------------------------
 * System Runtime
-------------------------------------*/
void HelloPrimState::on_run() {
    
    this->shader.bind();
    LS_LOG_GL_ERR();
    
    camera.update();
    ls::draw::set_shader_uniform(2, camera.get_vp_matrix());
    LS_LOG_GL_ERR();
    
    this->vao.bind();
    LS_LOG_GL_ERR();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    LS_LOG_GL_ERR();
    this->vao.unbind();
    LS_LOG_GL_ERR();
    this->shader.unbind();
    LS_LOG_GL_ERR();
}

/*-------------------------------------
 * System Stop
-------------------------------------*/
void HelloPrimState::on_stop() {
    shader.terminate();
    vao.terminate();
    
    if (pControlState) {
        pControlState->set_state(ls::game::game_state_t::STOPPED);
        pControlState = nullptr;
    }
}
