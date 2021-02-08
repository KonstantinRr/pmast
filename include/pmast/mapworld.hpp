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
    static constexpr float
        cameraSpeedLeft = 1.0f,
        cameraSpeedForward = 1.0f,
        cameraSpeedUp = 1.0f,
        cameraSpeedRotateUp = 0.02f,
        cameraSpeedRotateDown = 0.02f,
        streetHeight = 0.5f,
        streetSelectedHeight = 0.8f,
        streetWidth = 1.0f,
        agentHeight = 1.0f,
        streetLengthAdd = 0.5f;

    void generateWayMesh(nyrem::MeshBuilder &mesh,
        const std::vector<nyrem::vec2> &points,
        float height, float width);

    std::shared_ptr<nyrem::GLModel> m_cubeModel;
    std::vector<std::shared_ptr<nyrem::GLModel>> models;

    nyrem::RenderPipeline m_pipeline;

    std::shared_ptr<nyrem::Engine> m_engine;
    std::shared_ptr<World> m_world;

    std::shared_ptr<nyrem::TransformableEntity> m_highway_entity,
        m_world_entity, m_plane_entity; 

    std::shared_ptr<nyrem::RenderList<nyrem::Entity>> m_entities;
    std::shared_ptr<nyrem::Camera3D<>> m_camera;
    std::shared_ptr<nyrem::PhongShader> m_shader;
    std::shared_ptr<nyrem::PhongListStage> m_shader_stage;

    bool m_hasStart=false, m_hasEnd=false;
    nyrem::vec3 m_start, m_end;

    enum KeyIndices {
        m_key_w = 0, m_key_s = 1, m_key_a = 2,
        m_key_d = 3, m_key_g = 4, m_key_space = 5,
        m_key_shift = 6, m_key_up = 7, m_key_down = 8,
        m_key_left = 9, m_key_right = 10, m_key_r = 11,
        m_key_t = 12, m_key_enter = 13, m_key_h = 14,
        m_key_last = 15,
    };

    nyrem::CallbackReturn<void(nyrem::KeyEvent)> m_keys[m_key_last];
};

}
#endif