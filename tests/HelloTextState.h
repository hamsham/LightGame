/*
 * File:   HelloTextState.h
 * Author: miles
 *
 * Created on October 27, 2015, 10:44 PM
 */

#ifndef HELLOTEXTSTATE_H
#define HELLOTEXTSTATE_H

#include <vector>

#include "ls/draw/Atlas.h"
#include "ls/draw/BoundingBox.h"
#include "ls/draw/Camera.h"
#include "ls/draw/FrameBuffer.h"
#include "ls/draw/PixelBuffer.h"
#include "ls/draw/RenderBuffer.h"
#include "ls/draw/SceneGraph.h"
#include "ls/draw/ShaderProgram.h"
#include "ls/draw/Texture.h"

#include "ls/game/GameState.h"



class ControlState;



class HelloTextState final : public ls::game::GameState {
    
  private:
    bool useOcclusionBuffer = false;

    ls::draw::ShaderProgram textShader;
    
    ls::draw::ShaderProgram occlusionShader;
    
    ls::draw::Atlas atlas;
    
    ls::draw::FrameBuffer occlusionFbo;
    
    ls::draw::RenderBuffer occlusionDepth;
    
    ls::draw::Texture occlusionTarget;
    
    unsigned currentPbo = 0;
    
    ls::draw::PixelBuffer occlusionPbos[2];
    
    ls::draw::Texture matrixBuf;
    
    ls::draw::SceneGraph textMesh;
    
    ls::draw::SceneGraph occlusionMeshes;
    
    std::vector<ls::draw::BoundingBox> textBoxes;
    
    std::vector<unsigned> meshesInScene;
    
  public:
    virtual ~HelloTextState();

    HelloTextState();

    HelloTextState(const HelloTextState&) = delete;

    HelloTextState(HelloTextState&&);

    HelloTextState& operator=(const HelloTextState&) = delete;

    HelloTextState& operator=(HelloTextState&&);

  private:
    void setup_text_shader();
    
    void setup_occlusion_shader();
    
    void setup_occlusion_fbo();

    void setup_atlas();

    void setup_text();

    void setup_occluders();
    
    void create_matrix_buffer();
    
    void frustum_cull_text(const ls::math::mat4& vpMatrix);
    
    void bbox_cull_text(const ls::math::mat4& vpMatrix);
    
    void draw_occlusion_data(const ls::math::mat4& vpMatrix);
    
    void read_occlusion_data();
    
    void do_frustum_cull(const ls::math::mat4& vpMatrix);
    
    void draw_text_data(const ls::math::mat4& vpMatrix);

  protected:
    virtual bool on_start() override;

    virtual void on_run() override;

    virtual void on_stop() override;
};



#endif  /* HELLOTEXTSTATE_H */
