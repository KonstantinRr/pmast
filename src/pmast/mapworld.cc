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
#include <pmast/osm.hpp>
#include <pmast/agent.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

using namespace traffic;

MapWorld::MapWorld(
    const std::shared_ptr<nyrem::Engine> &engine,
	const std::shared_ptr<traffic::World> &world)
{
    using namespace nyrem;
    try {
        m_engine = engine;
        m_world = world;
        glm::mat4x4 mat = glm::lookAt(
            glm::vec3{1.0f, 1.0f, 1.0f},
            glm::vec3{0.0f, 1.0f, 0.0f},
            glm::vec3{0.0f, 1.0f, 0.0f});
        glm::quat q = toQuat(mat);

        m_camera = std::make_shared<Camera3D<>>(
            Projection3DSettings(0.01f, 100.0f, glm::radians(80.0f), 1.0f),
            Translation3DSettings(1.0f, 1.0f, 1.0f),
            RotationSettings3DQuat(q)
        );
        m_shader = make_shader<PhongMemoryShader>();
        m_shader_stage = std::make_shared<PhongListStage>(m_shader);
        m_shader_stage->stageBuffer().renderList =
            std::make_shared<RenderList<Entity>>();
        m_shader_stage->stageBuffer().camera = m_camera;

        m_pipeline.addStage(m_shader_stage);

        loadWorld(m_world->getMap());
        loadHighway(m_world->getHighwayMap());

        MeshBuilder cube;
        cube.addCube({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        auto expFile = cube.exporter()
            .addVertex().addNormal().exportData();
        auto model = std::make_shared<GLModel>(expFile);
        std::cout << expFile.detail();
        auto cubeEntity = std::make_shared<TransformableEntity>();
        cubeEntity->setModel(model);
        createKeyBindings();
        //m_shader_stage->stageBuffer().renderList->add(cubeEntity);
    } catch (const std::exception &excp) {
        m_shader = nullptr;
        m_shader_stage = nullptr;
        throw;
    }
}

void MapWorld::activate(nyrem::Navigator &nav)
{
    using namespace nyrem;
    using namespace nyrem::keys;
	using namespace nyrem::mouse;

    InputHandler &m_input = m_engine->input();
    m_key_w = m_input.loopKey(NYREM_KEY_W).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveForward(cameraSpeedForward);
    });
    m_key_s = m_input.loopKey(NYREM_KEY_S).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveBackward(cameraSpeedForward);
    });
    m_key_a = m_input.loopKey(NYREM_KEY_A).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveLeft(cameraSpeedLeft);
    });
    m_key_d = m_input.loopKey(NYREM_KEY_D).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveRight(cameraSpeedLeft);
    });
    m_key_space = m_input.loopKey(NYREM_KEY_SPACE).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveUp(cameraSpeedUp);
    });
    m_key_shift = m_input.loopKey(NYREM_KEY_LEFT_SHIFT).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveDown(cameraSpeedUp);
    });
    m_key_g = m_input.callbackKey(NYREM_KEY_G).listen(true, [this, &nav](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            nav.pushReplacementNamed("canvas");
    });
}

void MapWorld::deactivate(nyrem::Navigator &nav)
{
    m_key_w.remove();
    m_key_s.remove();
    m_key_a.remove();
    m_key_d.remove();
    m_key_g.remove();
    m_key_space.remove();
    m_key_shift.remove();
}

void MapWorld::createKeyBindings()
{
}

void MapWorld::loadWorld(const std::shared_ptr<traffic::OSMSegment> &map) noexcept
{
    using std::vector;
    using namespace nyrem;

    vector<vector<dvec2>> buildings = map->findBuildings();
    // calculates some meta information and the geographical center
    dvec2 center = { 0.0, 0.0};
    size_t nodeCount = 0;
    for (const auto& building : buildings) {
        nodeCount += building.size();
        for (const dvec2 vec : building)
            center += vec;
    }
    center = center / static_cast<double>(nodeCount);
    dvec2 naturalCenter = sphereToPlane(center);
    spdlog::info("Found {} Buildings with {} Nodes (Center {} {})",
        buildings.size(), nodeCount, center.x, center.y);

    // converts the information to natural space
    vector<vector<vec2>> naturalSpace(buildings.size());
    for (size_t i = 0; i < buildings.size(); i++) {
        // the used references for the buildings that are transformed
        const auto& building = buildings[i];
        auto& naturalBuilding = naturalSpace[i];

        naturalBuilding.resize(building.size());
        for (size_t k = 0; k < buildings[i].size(); k++)
            naturalBuilding[k] = vec2(
                (sphereToPlane(building[k], center) - naturalCenter) * 250.0);
    }
    // we don't need the original list of buildings anymore
    buildings.clear();

    // calculates the center in natural space

    // creates a list of MeshBuilders from the natural space
    vector<MeshBuilder> objectMeshes(naturalSpace.size());
    for (size_t i = 0; i < naturalSpace.size(); i++) {
        // creates the top polygon
        MeshBuilder2D top;
        top.addPolygon(naturalSpace[i]);
        // inverts the winding of the bottom plane
        top.invertWinding(false);
        MeshBuilder2D bottom(top);
        // adds both meshes to the parent mesh
        objectMeshes[i].add(top, 0.0f, true);
        objectMeshes[i].add(bottom, 0.3f, false);
    }

    MeshBuilder total;
    for (const auto& childMesh : objectMeshes)
        total.add(childMesh);
    
    ExportFile file = total.exporter()
        .addVertex().addNormal().exportData();
    models.push_back(std::make_shared<GLModel>(file));
    
    std::cout << file.detail() << std::endl;
    // creates the model entity
    auto modelEntity = std::make_shared<TransformableEntity>();
    modelEntity->setModel(models.front());
    m_shader_stage->stageBuffer().renderList->add(modelEntity);

    std::cout << total.info() << std::endl;
}

void MapWorld::loadHighway(const std::shared_ptr<traffic::OSMSegment> &highwayMap) noexcept
{
    using std::vector;
    using namespace nyrem;
}

void MapWorld::render(const nyrem::RenderContext &context) {
    m_pipeline.render(context);
}