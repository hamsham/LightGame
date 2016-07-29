
#ifndef HELLOPROPERTYSTATE_H
#define HELLOPROPERTYSTATE_H

#include <vector>

#include "ls/draw/SceneGraph.h"
#include "ls/draw/SceneFileLoader.h"
#include "ls/draw/ShaderProgram.h"

#include "ls/game/GameState.h"




class HelloPropertyState final : public ls::game::GameState {
  private:
    ls::draw::ShaderProgram testShader;
    
  public:
    virtual ~HelloPropertyState();

    HelloPropertyState();

    HelloPropertyState(const HelloPropertyState&) = delete;

    HelloPropertyState(HelloPropertyState&&);

    HelloPropertyState& operator=(const HelloPropertyState&) = delete;

    HelloPropertyState& operator=(HelloPropertyState&&);

  private:
    void setup_shader(ls::draw::ShaderProgram& s, const std::string& vertData, const std::string& fragData);

  protected:
    virtual bool on_start() override;

    virtual void on_run() override;

    virtual void on_stop() override;
};



#endif  /* HELLOPROPERTYSTATE_H */
