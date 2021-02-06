/// MIT License
/// 
/// Copyright (c) 2020 Konstantin Rolf
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
/// 
/// Written by Konstantin Rolf (konstantin.rolf@gmail.com)
/// July 2020

#pragma once

#ifndef MAPWORLD_HPP
#define MAPWORLD_HPP

#include <pmast/internal.hpp>
#include <engine/listener.hpp>
#include <engine/shader.hpp>
#include <engine/window.hpp>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace traffic {

class World;
class OSMSegment;

class MapWorld : public nyrem::EngineStage {
public:
    MapWorld(
        const std::shared_ptr<nyrem::Engine> &engine,
		const std::shared_ptr<traffic::World> &world);

    void loadWorld(const std::shared_ptr<traffic::OSMSegment> &map) noexcept;
    void loadHighway(const std::shared_ptr<traffic::OSMSegment> &highwayMap) noexcept;
    void createKeyBindings();

    virtual void render(const nyrem::RenderContext &context) override;

    virtual void activate(nyrem::Navigator &nav) override;
    virtual void deactivate(nyrem::Navigator &nav) override;
protected:
    static constexpr float cameraSpeedLeft = 0.01f;
    static constexpr float cameraSpeedForward = 0.01f;
    static constexpr float cameraSpeedUp = 0.01f;
    std::vector<std::shared_ptr<nyrem::GLModel>> models;

    nyrem::RenderPipeline m_pipeline;

    std::shared_ptr<nyrem::Engine> m_engine;
    std::shared_ptr<World> m_world;

    std::shared_ptr<nyrem::RenderList<nyrem::Entity>> m_entities;
    std::shared_ptr<nyrem::Camera3D<>> m_camera;
    std::shared_ptr<nyrem::PhongShader> m_shader;
    std::shared_ptr<nyrem::PhongListStage> m_shader_stage;

    nyrem::CallbackReturn<void(nyrem::KeyEvent)> m_key_w,
        m_key_s, m_key_a, m_key_d, m_key_g,
        m_key_space, m_key_shift;
};

}
#endif