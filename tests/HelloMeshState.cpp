/*
 * File:   HelloMeshState.cpp
 * Author: miles
 *
 * Created on October 27, 2015, 10:44 PM
 */

#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>

#include <SDL2/SDL.h>

#include "lightsky/setup/Setup.h"
#include "lightsky/math/Math.h"
#include "lightsky/utils/Utils.h"
#include "lightsky/draw/Draw.h"
#include "lightsky/game/Game.h"

#include "Display.h"
#include "HelloMeshState.h"
#include "ControlState.h"

namespace math = ls::math;
namespace draw = ls::draw;
namespace utils = ls::utils;
using draw::VAOAttrib;
using draw::ShaderAttribArray;


#ifndef LS_GAME_TEST_MESH
    #define LS_GAME_TEST_MESH "testdata/rover/testmesh.dae"
#endif




///////////////////////////////////////////////////////////////////////////////
//  Mesh Shader
///////////////////////////////////////////////////////////////////////////////
namespace {

typedef std::chrono::duration<uint64_t, std::milli> milli_duration;
typedef std::chrono::steady_clock scene_clock_t;

/*-------------------------------------
 * Model Matrix Buffer
-------------------------------------*/
const char* MESH_TEXTURE_UNIFORM_STR = "DIFFUSE_TEXTURE";
unsigned MESH_TEXTURE_UNIFORM_ID = -1;
unsigned MESH_TEXTURE_UNIFORM_LOCATION = 0;

/*-------------------------------------
 * Mesh Vertex Shader
-------------------------------------*/
constexpr char vsShaderData[] = u8R"***(
//#version 300 es
#version 330 core

precision mediump float;

layout (location = 0) in vec3 posAttrib;
layout (location = 1) in vec2 uvAttrib;
layout (location = 2) in vec3 normAttrib;

layout(shared) uniform BatchProperties {
    mat4 vpMatrix;
} batchProperties;

uniform mat4 MODEL_MATRIX;

out vec2 fragVertUV;
out vec3 fragEyeDirection;
out vec3 fragVertNormal;


void main() {
    mat4 vpMat          = batchProperties.vpMatrix;
    gl_Position         = vpMat * MODEL_MATRIX * vec4(posAttrib, 1.0);
    fragVertUV          = uvAttrib;
    fragEyeDirection    = vec3(-vpMat[0][2], -vpMat[1][2], -vpMat[2][2]);
    fragVertNormal      = normalize(vec4(MODEL_MATRIX * vec4(normAttrib, 0.0)).xyz);
}
)***";

/*-------------------------------------
 * Mesh Fragment Shader
-------------------------------------*/
constexpr char fsShaderData[] = u8R"***(
//#version 300 es
#version 330 core

precision mediump float;

uniform sampler2D DIFFUSE_TEXTURE;

in vec2 fragVertUV;
in vec3 fragEyeDirection;
in vec3 fragVertNormal;

out vec4 fragOutColor;

float calc_diffuse_intensity(in vec3 vertNorm, in vec3 lightDir) {
    return max(0.0, dot(vertNorm, lightDir));
}


void main() {
    //const vec4 baseCol  = vec4(0.5, 0.5, 0.5, 1.0);
    vec4 baseCol        = texture(DIFFUSE_TEXTURE, fragVertUV);
    vec3 e              = normalize(fragEyeDirection);
    vec3 n              = normalize(fragVertNormal);
    float brightness    = calc_diffuse_intensity(n, e);
    fragOutColor        = baseCol * brightness;
}
)***";



/******************************************************************************
 * Normal/Tangent/Bitangent Visualizer
 ******************************************************************************/
constexpr char enbtVS[] = u8R"***(
//#version 300 es
#version 330 core

precision mediump float;

layout (location = 0) in vec3 posAttrib;
layout (location = 1) in vec3 normAttrib;

layout(shared) uniform BatchProperties {
    mat4 vpMatrix;
} batchProperties;

uniform mat4 MODEL_MATRIX;

out vec4 normPos;

void main() {
    mat4 vpMat  = batchProperties.vpMatrix;
    mat4 mvpMat = vpMat * MODEL_MATRIX;
    gl_Position = mvpMat * vec4(posAttrib, 1.0);
    normPos     = mvpMat * vec4(normAttrib, 0.0);
}
)***";



const char enbtGS[] = u8R"***(
#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 21) out;

uniform bool showEdges      = true;
uniform bool showNormals    = false;

in vec4 normPos[];
out vec4 lineCol;

void main() {

    if (showNormals) {
        for (int i = 0; i < 3; ++i) {
            vec4 origin = gl_in[i].gl_Position;
            lineCol = vec4(0.0, 1.0, 0.0, 1.0);
            gl_Position = origin;
            EmitVertex();
            gl_Position = origin + normPos[i] * 0.025;
            EmitVertex();
            EndPrimitive();
        }
    }

    if (showEdges) {
        lineCol = vec4(1.0, 0.0, 1.0, 1.0);
        vec4 a = gl_in[0].gl_Position;
        vec4 b = gl_in[1].gl_Position;
        vec4 c = gl_in[2].gl_Position;
        gl_Position = a;
        EmitVertex();
        gl_Position = b;
        EmitVertex();
        EndPrimitive();
        gl_Position = b;
        EmitVertex();
        gl_Position = c;
        EmitVertex();
        EndPrimitive();
        gl_Position = c;
        EmitVertex();
        gl_Position = a;
        EmitVertex();
        EndPrimitive();
    }
}
)***";



constexpr char enbtFS[] = u8R"***(
//#version 300 es
#version 330 core

precision mediump float;

in vec4 lineCol;
out vec4 fragCol;

void main() {
    fragCol = lineCol;
}
)***";



} // end anonymous namespace



/*-----------------------------------------------------------------------------
 * Test Class
-----------------------------------------------------------------------------*/
/*-------------------------------------
 * Destructor
-------------------------------------*/
HelloMeshState::~HelloMeshState() {
}

/*-------------------------------------
 * Constructor
-------------------------------------*/
HelloMeshState::HelloMeshState() {
}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
HelloMeshState::HelloMeshState(HelloMeshState&& state) :
    GameState{}
{
    *this = std::move(state);
}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
HelloMeshState& HelloMeshState::operator =(HelloMeshState&& state) {
    GameState::operator =(std::move(state));

    testShader = std::move(state.testShader);

    testData = std::move(state.testData);

    prevTime = std::move(state.prevTime);

    currentAnimationId = state.currentAnimationId;
    state.currentAnimationId = 0;

    currentAnimation = std::move(state.currentAnimation);

    uniformBlock = std::move(state.uniformBlock);

    meshShaderUboIndex = state.meshShaderUboIndex;
    state.meshShaderUboIndex = 0;
    enbtShaderUboIndex = state.enbtShaderUboIndex;
    state.enbtShaderUboIndex = 0;

    return *this;
}

/*-------------------------------------
-------------------------------------*/
void HelloMeshState::bind_shader_uniforms(const draw::ShaderProgram& s) {
    s.bind();
    LS_LOG_GL_ERR();

    const ShaderAttribArray& uniforms = s.get_uniforms();

    for (unsigned i = 0; i < uniforms.get_num_attribs(); ++i) {

        const utils::Pointer<GLchar[]>& uniformName = uniforms.get_attrib_name(i);
        const GLint index = s.get_uniform_location(uniformName.get());

        if (index < 0) {
            continue;
        }

        if (strcmp(uniformName.get(), MESH_TEXTURE_UNIFORM_STR) == 0) {
            LS_LOG_MSG("Found 2D Texture Uniform ", index, ": ", uniformName);
            MESH_TEXTURE_UNIFORM_ID = index;
            draw::set_shader_uniform_int(MESH_TEXTURE_UNIFORM_ID, draw::tex_slot_t::TEXTURE_SLOT_0);
        }
        else {
            LS_LOG_MSG("Unknown shader uniform found: ", uniformName);
        }
    }

    s.unbind();
    LS_LOG_GL_ERR();
}

/*-------------------------------------
-------------------------------------*/
void HelloMeshState::unbind_shader_uniforms() {
    LS_LOG_GL_ERR();

    MESH_TEXTURE_UNIFORM_ID = -1;
    MESH_TEXTURE_UNIFORM_LOCATION = 0;
}

/*-------------------------------------
-------------------------------------*/
void HelloMeshState::setup_shader(
    draw::ShaderProgram& s,
    const char* const vertData,
    const char* const fragData,
    const char* const geomData
) {
    draw::ShaderProgramAssembly shaderMaker;
    draw::ShaderObject vMeshShader, fMeshShader;

    LS_ASSERT(vMeshShader.init(draw::shader_stage_t::SHADER_STAGE_VERTEX, vertData, 0));
    LS_LOG_GL_ERR();
    LS_ASSERT(shaderMaker.set_vertex_shader(vMeshShader));

    LS_ASSERT(fMeshShader.init(draw::shader_stage_t::SHADER_STAGE_FRAGMENT, fragData, 0));
    LS_LOG_GL_ERR();
    LS_ASSERT(shaderMaker.set_fragment_shader(fMeshShader));

#ifdef LS_DRAW_BACKEND_GL
    draw::ShaderObject gMeshShader;
    if (geomData) {
        LS_ASSERT(gMeshShader.init(draw::shader_stage_t::SHADER_STAGE_GEOMETRY, geomData, 0));
        LS_LOG_GL_ERR();
        LS_ASSERT(shaderMaker.set_geometry_shader(gMeshShader));
    }
#endif
    // unused parameter warning
    (void)geomData;
    (void)enbtVS;
    (void)enbtFS;
    (void)enbtGS;

    LS_ASSERT(shaderMaker.is_assembly_valid());
    LS_ASSERT(shaderMaker.assemble(s, false));
    LS_ASSERT(shaderMaker.link(s));

    bind_shader_uniforms(s);

    vMeshShader.terminate();
    fMeshShader.terminate();

#ifdef LS_DRAW_BACKEND_GL
    gMeshShader.terminate();
#endif

    LS_LOG_GL_ERR();
}

/*-------------------------------------
 * Mesh Setup
-------------------------------------*/
void HelloMeshState::setup_meshes() {
    if (testData.meshes.empty()) {
        LS_LOG_ERR("ERROR: Failed to load the mesh \"", LS_GAME_TEST_MESH, ".\"");
        return;
    }

    testShader.bind();
    LS_LOG_GL_ERR();

    const GLint MESH_MODEL_MAT_UNIFORM_ID = glGetUniformLocation(testShader.gpu_id(), "MODEL_MATRIX");
    LS_LOG_GL_ERR();

    draw::set_shader_uniform(MESH_MODEL_MAT_UNIFORM_ID, math::mat4{1.f});
    LS_LOG_GL_ERR();

    testShader.unbind();
    LS_LOG_GL_ERR();
}

/*-------------------------------------
 * Animation Setup
-------------------------------------*/
void HelloMeshState::setup_animations() {
    std::vector<draw::Animation>& sceneAnims = testData.animations;

    for (std::vector<draw::AnimationChannel>& animList : testData.nodeAnims) {
        for (draw::AnimationChannel& track : animList) {
            track.animationMode = draw::animation_flag_t::ANIM_FLAG_INTERPOLATE;
        }
    }

    LS_LOG_MSG("Running ", sceneAnims.size(), " animations.");

    currentAnimationId = 0;
    currentAnimation.set_play_state(draw::ANIM_STATE_PLAYING);
    currentAnimation.set_num_plays(draw::AnimationPlayer::PLAY_ONCE);
    currentAnimation.set_time_dilation(1.0);
}

/*-------------------------------------
 * System Startup
-------------------------------------*/
void HelloMeshState::setup_uniform_blocks() {
    int uboIndex = -1;

    LS_ASSERT(uniformBlock.init());
    LS_LOG_GL_ERR();

    uniformBlock.bind();
    LS_LOG_GL_ERR();

    LS_ASSERT(uniformBlock.setup_attribs(testShader, 0));
    LS_LOG_GL_ERR();

    {
        uboIndex = testShader.get_matching_uniform_block_index(uniformBlock.get_attribs().get_block_name());
        LS_ASSERT(uboIndex >= 0);

        meshShaderUboIndex = (unsigned)uboIndex;
        LS_ASSERT(draw::are_attribs_compatible(testShader, meshShaderUboIndex, uniformBlock) >= 0);
    }

#ifdef LS_DRAW_BACKEND_GL
    {
        uboIndex = enbtShader.get_matching_uniform_block_index(uniformBlock.get_attribs().get_block_name());
        LS_ASSERT(uboIndex >= 0);

        enbtShaderUboIndex = (unsigned)uboIndex;
        LS_ASSERT(draw::are_attribs_compatible(enbtShader, enbtShaderUboIndex, uniformBlock) >= 0);
    }
#endif

    uniformBlock.unbind();
    LS_LOG_GL_ERR();
}

/*-------------------------------------
 * Rendering Scene Nodes
-------------------------------------*/
uint32_t HelloMeshState::render_scene_node(uint32_t currentVao, const uint32_t modelMatId, const draw::SceneNode& n) const {
    unsigned materialId = draw::material_property_t::INVALID_MATERIAL;
    unsigned meshDataId = n.dataId;

    const std::vector<unsigned>& meshCounts = testData.nodeMeshCounts;
    const unsigned numMeshes = meshCounts[meshDataId];

    const std::vector<utils::Pointer<draw::DrawCommandParams[]>>& drawParamArray = testData.nodeMeshes;
    const utils::Pointer<draw::DrawCommandParams[]>& drawParams = drawParamArray[meshDataId];

    const std::vector<math::mat4>& matrices = testData.modelMatrices;
    const math::mat4& modelMatrix = matrices[n.nodeId];

    for (unsigned i = 0; i < numMeshes; ++i) {
        const draw::DrawCommandParams& params = drawParams[i];

        if (currentVao != params.vaoId) {
            currentVao = params.vaoId;
            glBindVertexArray(currentVao);
        }

#ifdef LS_DRAW_BACKEND_GL
        if (params.drawMode != draw::draw_mode_t::DRAW_MODE_TRIS) {
            continue;
        }
#endif

        materialId = params.materialId;

        if (materialId != draw::material_property_t::INVALID_MATERIAL) {
            testData.materials[materialId].bind();
        }

        draw::set_shader_uniform(modelMatId, modelMatrix);
        LS_LOG_GL_ERR();

        switch (params.drawFunc) {
            case draw::draw_func_t::DRAW_ARRAYS:
                glDrawArrays(params.drawMode, params.first, params.count);
                LS_LOG_GL_ERR();
                break;

            case draw::draw_func_t::DRAW_ELEMENTS:
                glDrawElements(params.drawMode, params.count, params.indexType, params.offset);
                LS_LOG_GL_ERR();
                break;

            default:
                LS_ASSERT(false);
        }

        LS_LOG_GL_ERR();

        if (materialId != draw::material_property_t::INVALID_MATERIAL) {
            testData.materials[materialId].unbind();
        }
    }

    return currentVao;
}

/*-------------------------------------
 * Scene Graph Rendering
-------------------------------------*/
void HelloMeshState::render_scene_graph(const draw::ShaderProgram& s, const unsigned uboBindIndex) const {
    uint32_t currentVao = 0;

    s.bind();
    LS_LOG_GL_ERR();

    uniformBlock.bind_base(uboBindIndex);
    LS_LOG_GL_ERR();

    const GLint modelMatId = glGetUniformLocation(s.gpu_id(), "MODEL_MATRIX");
    LS_LOG_GL_ERR();

    for (const draw::SceneNode& node : testData.nodes) {
        if (node.type != draw::scene_node_t::NODE_TYPE_MESH) {
            continue;
        }

        currentVao = render_scene_node(currentVao, modelMatId, node);
    }

    glBindVertexArray(0);

    s.unbind();
    LS_LOG_GL_ERR();
}

/*-------------------------------------
 * Animation updating
-------------------------------------*/
void HelloMeshState::update_animations() {
    if (testData.animations.empty()) {
        return;
    }

    const scene_clock_t::time_point&& currTime = scene_clock_t::now();
    const scene_clock_t::duration&& tickDuration = currTime - prevTime;
    prevTime = currTime;
    const milli_duration&& tickTime = std::chrono::duration_cast<milli_duration>(tickDuration);

    // Play the current animation until it stops. Then move onto the next animation.
    if (currentAnimation.is_stopped()) {
        std::vector<draw::Animation>& animations = testData.animations;
        currentAnimationId = (currentAnimationId + 1) % animations.size();
        
        // reset the transformations in a graph to those at the beginning of an animation
        draw::Animation& initialState = testData.animations[currentAnimationId];
        initialState.init(testData);

        currentAnimation.set_play_state(draw::animation_state_t::ANIM_STATE_PLAYING);
        currentAnimation.set_num_plays(draw::AnimationPlayer::PLAY_ONCE);
    }

    currentAnimation.tick(testData, currentAnimationId, tickTime.count());
}

/*-------------------------------------
 * System Startup
-------------------------------------*/
bool HelloMeshState::on_start() {
    srand(time(nullptr));
    setup_shader(testShader, vsShaderData, fsShaderData);

#ifdef LS_DRAW_BACKEND_GL
    setup_shader(enbtShader, enbtVS, enbtFS, enbtGS);
#endif

    setup_uniform_blocks();

    preloader = std::async(std::launch::async, [&]()->draw::SceneFilePreLoader {
        draw::SceneFilePreLoader loader;
        loader.load(LS_GAME_TEST_MESH);
        return loader;
    });

    //utils::Pointer<draw::SceneFileLoader> meshLoader {new draw::SceneFileLoader{}};

    LS_LOG_GL_ERR();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    //prevTime = std::move(scene_clock_t::now());
    prevTime = scene_clock_t::now();

    return true;
}

/*-------------------------------------
 * System Runtime
-------------------------------------*/
void HelloMeshState::on_run() {
    const Uint8* const keyStates = SDL_GetKeyboardState(nullptr);
    if (keyStates[SDL_SCANCODE_SPACE] && !testData.nodes.empty()) {
        const unsigned selectedNode = rand() % testData.nodes.size();
        unsigned parentNode = selectedNode;
        
        while (testData.node_is_child(selectedNode, parentNode)
        || selectedNode == parentNode
        ) {
            parentNode = rand() % testData.nodes.size();
        }
        
        testData.reparent_node(selectedNode, parentNode);
        
        /*
        const unsigned numDeletedNodes = testData.delete_node(selectedNode);
        
        LS_LOG_MSG("Deleted node ", selectedNode, " and ", numDeletedNodes-1, " others.",
            "\n\t# Node Objects:          ", testData.nodes.size(),
            "\n\t# Node Cameras:          ", testData.cameras.size(),
            "\n\t# Draw Parameters:       ", testData.nodeMeshes.size(),
            "\n\t# Node Meshes:           ", testData.nodeMeshCounts.size(),
            "\n\t# Node Names:            ", testData.nodeNames.size(),
            "\n\t# Base Transforms:       ", testData.baseTransforms.size(),
            "\n\t# Active Transforms:     ", testData.currentTransforms.size(),
            "\n\t# Model Matrices:        ", testData.modelMatrices.size(),
            "\n\t# Animation Channels:    ", testData.nodeAnims.size(),
            "\n\t# Total Animations:      ", testData.animations.size()
        );
        */
        
        LS_LOG_MSG("Node ", selectedNode, " has ",
            testData.get_num_immediate_children(selectedNode), " immediate children and ",
            testData.get_num_total_children(selectedNode), " total children."
        );
        
        LS_LOG_MSG("Checking if ", selectedNode, " is a child of ", parentNode, ' ',
            testData.node_is_child(selectedNode, parentNode), "\n\tDone."
        );
    }
    
    // this statement cannot be called from another function (GCC/CLang bug)
    if (preloader.valid() && preloader.wait_for(std::chrono::milliseconds{0}) == std::future_status::ready) {
        draw::SceneFileLoader loader;
        loader.load(preloader.get());

        testData = std::move(loader.get_loaded_data());
        setup_animations();
    }
    else {
        update_animations();
        testData.update();
    }

    uniformBlock.bind();
    LS_LOG_GL_ERR();

    const ControlState* const pController = get_parent_system().get_game_state<ControlState>();
    const math::mat4& vpMat = pController->get_camera_view_projection();

    uniformBlock.modify(0, sizeof(math::mat4), vpMat.m);
    LS_LOG_GL_ERR();

    uniformBlock.unbind();
    LS_LOG_GL_ERR();

    render_scene_graph(testShader, meshShaderUboIndex);

#ifdef LS_DRAW_BACKEND_GL
    //render_scene_graph(enbtShader, enbtShaderUboIndex);
#endif
}

/*-------------------------------------
 * System Stop
-------------------------------------*/
void HelloMeshState::on_stop() {
    testShader.terminate();
    enbtShader.terminate();
    testData.terminate();
    currentAnimationId = 0;
    currentAnimation.reset();
    uniformBlock.terminate();
}
