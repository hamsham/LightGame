/* 
 * File:   HelloPrimState.h
 * Author: miles
 *
 * Created on October 27, 2015, 10:44 PM
 */

#ifndef HELLOPRIMSTATE
#define HELLOPRIMSTATE



#include <memory>

#include "lightsky/draw/Atlas.h"
#include "lightsky/draw/BufferObject.h"
#include "lightsky/draw/Camera.h"
#include "lightsky/draw/ShaderProgram.h"
#include "lightsky/draw/VertexArray.h"

#include "TestRenderState.h"



class ControlState;



class HelloPrimState final : public TestRenderState {
    
    private:
        ControlState* pControlState = nullptr;
        
        ls::draw::ShaderProgram shader;
        
        ls::draw::BufferObject vbo;
        
        ls::draw::VertexArray vao;
        
        void update_camera();
        
        void update_vert_color(const unsigned vertPos, const bool isVisible);
        
        std::unique_ptr<char[]> gen_vertex_data();

    public:
        virtual ~HelloPrimState();
        
        HelloPrimState();
        
        HelloPrimState(const HelloPrimState&) = delete;
        
        HelloPrimState(HelloPrimState&&);
        
        HelloPrimState& operator=(const HelloPrimState&) = delete;
        
        HelloPrimState& operator=(HelloPrimState&&);
        
    private:
        void setup_camera();
        
        void setup_shaders();
        
        void setup_prims();

    protected:
        virtual bool on_start() override;

        virtual void on_run() override;

        virtual void on_stop() override;
};



#endif	/* HELLOPRIMSTATE */
