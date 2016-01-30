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



class ControlState;



class HelloPrimState final : public ls::game::GameState {
    
    friend class ControlState;
    
    private:
        ControlState* pControlState = nullptr;
        
        ls::draw::ShaderProgram shader;
        
        ls::draw::BufferObject vbo;
        
        //ls::draw::BufferObject ibo;
        
        ls::draw::VertexArray vao;
        
        ls::draw::Camera camera;
        
        void update_camera();
        
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
