/* 
 * File:   HelloTextState.h
 * Author: miles
 *
 * Created on October 27, 2015, 10:44 PM
 */

#ifndef HELLOTEXTSTATE_H
#define HELLOTEXTSTATE_H



#include "lightsky/draw/Atlas.h"
#include "lightsky/draw/BufferObject.h"
#include "lightsky/draw/Camera.h"
#include "lightsky/draw/ShaderProgram.h"
#include "lightsky/draw/VertexArray.h"



class HelloTextState final : public ls::game::GameState {
    private:
        unsigned numTextIndices = 0;
        
        ls::draw::ShaderProgram shader;
        
        ls::draw::Atlas atlas;
        
        ls::draw::BufferObject vbo;
        
        ls::draw::BufferObject ibo;
        
        ls::draw::VertexArray vao;
        
        ls::draw::Camera camera;

    public:
        virtual ~HelloTextState();
        
        HelloTextState();
        
        HelloTextState(const HelloTextState&) = delete;
        
        HelloTextState(HelloTextState&&) = default;
        
        HelloTextState& operator=(const HelloTextState&) = delete;
        
        HelloTextState& operator=(HelloTextState&&) = default;
        
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
