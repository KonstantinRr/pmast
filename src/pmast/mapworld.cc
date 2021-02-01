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

#include <pmast/mapworld.hpp>

using namespace traffic;

MapWorld::MapWorld()
try {
    using namespace nyrem;
    //m_camera = std::make_shared<Camera3D>(0.01f, 100.0f, 90.0f, 1.0f);
    m_shader = make_shader<PhongMemoryShader>();
    m_shader_stage = std::make_shared<PhongListStage>(m_shader);

    m_pipeline.addStage(m_shader_stage);

    MeshBuilder cube = loadCube();
    auto model = std::make_shared<GLModel>(cube.exporter()
        .addColor().addNormal().addTexture().exportData());
} catch (const std::exception &excp) {
    m_shader = nullptr;
    m_shader_stage = nullptr;
    throw;
}

void MapWorld::render(const nyrem::RenderContext &context) {
    m_pipeline.render(context);
}