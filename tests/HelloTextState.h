/* 
 * File:   HelloTextState.h
 * Author: miles
 *
 * Created on October 27, 2015, 10:44 PM
 */

#ifndef HELLOTEXTSTATE_H
#define HELLOTEXTSTATE_H



#include "lightsky/draw/Atlas.h"
#include "lightsky/draw/VertexBuffer.h"
#include "lightsky/draw/IndexBuffer.h"
#include "lightsky/draw/Camera.h"
#include "lightsky/draw/ShaderProgram.h"
#include "lightsky/draw/VertexArray.h"

#include "TestRenderState.h"



class ControlState;



class HelloTextState final : public TestRenderState {
    
    private:
        ControlState* pControlState = nullptr;
        
        unsigned numTextIndices = 0;
        
        ls::draw::ShaderProgram shader;
        
        ls::draw::Atlas atlas;
        
        ls::draw::VertexBuffer vbo;
        
        ls::draw::IndexBuffer ibo;
        
        ls::draw::VertexArray vao;
        
        void set_text(const std::string& text);
        
        void update_camera();

    public:
        virtual ~HelloTextState();
        
        HelloTextState();
        
        HelloTextState(const HelloTextState&) = delete;
        
        HelloTextState(HelloTextState&&);
        
        HelloTextState& operator=(const HelloTextState&) = delete;
        
        HelloTextState& operator=(HelloTextState&&);
        
    private:
        void setup_camera();
        
        void setup_shaders();
        
        void setup_atlas();
        
        void setup_text();

    protected:
        virtual bool on_start() override;

        virtual void on_run() override;

        virtual void on_stop() override;
};



#endif	/* HELLOTEXTSTATE_H */
