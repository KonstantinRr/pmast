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
#include <pmast/osm_mesh.hpp>
#include <iostream>


using namespace traffic;

MapWorld::MapWorld(
    const std::shared_ptr<traffic::OSMSegment> &map,
    const std::shared_ptr<traffic::OSMSegment> &highwayMap)
{
    try {
        using namespace nyrem;
        using std::vector;
        m_map = map;
        m_highwayMap = highwayMap;

        vector<vector<vec2>> buildings = m_map->findBuildings();
        // transforms the list of building coordinates to plane coordinates
        for (auto &building : buildings) {
            for (size_t i = 0; i < building.size(); i++)
                building[i] = sphereToPlane(building[i]);
        }
        spdlog::info("Found {} Buildings", buildings.size());
        
        m_camera = std::make_shared<Camera3D<>>(
            Projection3DSettings(0.01f, 100.0f, glm::radians(80.0f), 1.0f),
            Translation3DSettings(0.0f),
            RotationSettings3DEuler(0.0f)
        );
        m_shader = make_shader<PhongMemoryShader>();
        m_shader_stage = std::make_shared<PhongListStage>(m_shader);
        m_shader_stage->stageBuffer().renderList =
            std::make_shared<RenderList<Entity>>();
        m_shader_stage->stageBuffer().camera = m_camera;

        m_pipeline.addStage(m_shader_stage);

        MeshBuilder cube;
        cube.addCube({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        std::cout << cube.getVertices().size() << " " <<
            cube.getNormals().size() << " " <<
            cube.getTextureCoords().size() << " " <<
            cube.getColors().size() << std::endl;

        auto expFile = cube.exporter()
            .addVertex().addNormal().addTexture().exportData();
        auto model = std::make_shared<GLModel>(expFile);
        std::cout << expFile.detail();
        auto cubeEntity = std::make_shared<TransformableEntity>();
        cubeEntity->setModel(model);
        m_shader_stage->stageBuffer().renderList->add(cubeEntity);
    } catch (const std::exception &excp) {
        m_shader = nullptr;
        m_shader_stage = nullptr;
        throw;
    }
}

void MapWorld::render(const nyrem::RenderContext &context) {
    m_pipeline.render(context);
}