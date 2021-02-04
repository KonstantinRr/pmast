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
#include <pmast/agent.hpp>
#include <pmast/osm.hpp>

#include <engine/listener.hpp>
#include <engine/shader.hpp>
#include <engine/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace traffic {

class MapWorld : public nyrem::Renderable {
public:
    MapWorld(
        const std::shared_ptr<traffic::OSMSegment> &map,
        const std::shared_ptr<traffic::OSMSegment> &highwayMap);

    virtual void render(const nyrem::RenderContext &context) override;

protected:
    std::shared_ptr<traffic::OSMSegment> m_highwayMap;
    std::shared_ptr<traffic::OSMSegment> m_map;

    nyrem::RenderPipeline m_pipeline;

    std::shared_ptr<nyrem::RenderList<nyrem::Entity>> m_entities;
    std::shared_ptr<nyrem::Camera3D<>> m_camera;
    std::shared_ptr<nyrem::PhongShader> m_shader;
    std::shared_ptr<nyrem::PhongListStage> m_shader_stage;
};


}

#endif