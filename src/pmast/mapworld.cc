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
            glm::vec3{0.0f, 1.0f, 1.0f},
            glm::vec3{0.0f, 1.0f, 0.0f},
            glm::vec3{0.0f, 1.0f, 0.0f});
        glm::quat q = toQuat(mat);

        m_camera = std::make_shared<Camera3D<>>(
            Projection3DSettings(0.1f, 500.0f, glm::radians(80.0f), 1.0f),
            Translation3DSettings(1.0f, 1.0f, 1.0f),
            RotationSettings3DQuat(q)
        );
        m_shader = make_shader<PhongMemoryShader>();
        m_shader_stage = std::make_shared<PhongListStage>(m_shader);
        m_entities = std::make_shared<RenderList<Entity>>();
        m_shader_stage->stageBuffer().renderList = std::make_shared<RenderList<Entity>>();
        m_shader_stage->stageBuffer().camera = m_camera;
        m_shader_stage->stageBuffer().lightPosition = {100.0f, 100.0f, 100.0f};

        m_pipeline.addStage(m_shader_stage);

        loadWorld(m_world->getMap());
        loadHighway(m_world->getHighwayMap());

        MeshBuilder cube;
        cube.addCube({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        auto expFile = cube.exporter().addVertex().addNormal().exportData();
        m_cubeModel = std::make_shared<GLModel>(expFile);

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
    m_keys[m_key_w] = m_input.loopKey(NYREM_KEY_W).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveForward(cameraSpeedForward);
    });
    m_keys[m_key_s] = m_input.loopKey(NYREM_KEY_S).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveBackward(cameraSpeedForward);
    });
    m_keys[m_key_a] = m_input.loopKey(NYREM_KEY_A).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveLeft(cameraSpeedLeft);
    });
    m_keys[m_key_d] = m_input.loopKey(NYREM_KEY_D).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveRight(cameraSpeedLeft);
    });
    m_keys[m_key_space] = m_input.loopKey(NYREM_KEY_SPACE).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveUp(cameraSpeedUp);
    });
    m_keys[m_key_shift] = m_input.loopKey(NYREM_KEY_LEFT_SHIFT).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->moveDown(cameraSpeedUp);
    });
    m_keys[m_key_g] = m_input.callbackKey(NYREM_KEY_G).listen(true, [this, &nav](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            nav.pushReplacementNamed("canvas");
    });
    m_keys[m_key_r] = m_input.callbackKey(NYREM_KEY_R).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED) {
            m_start = m_camera->translation().translation();
            m_hasStart = true;
        }
    });
    m_keys[m_key_t] = m_input.callbackKey(NYREM_KEY_T).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED) {
            m_end = m_camera->translation().translation();
            m_hasEnd = true;
        }
    });
    m_keys[m_key_enter] = m_input.callbackKey(NYREM_KEY_ENTER).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED && m_hasStart && m_hasEnd) {
            spdlog::info("Finding way from {},{} to {},{}",
                m_start.x, m_start.z, m_end.x, m_end.z);
			auto &traffic = m_world->getTrafficGraph();
			TrafficGraphNodeIndex idStart = traffic->findClosestNodeIdxPlane({m_start.x, m_start.z});
			TrafficGraphNodeIndex idStop = traffic->findClosestNodeIdxPlane({m_end.x, m_end.z});
            spdlog::info("Found Nodes: {} to {}", idStart, idStop);
            IndexRoute route = traffic->findIndexRoute(idStart, idStop);
            
            // finds the positions of all routes on the way
            std::vector<vec2> positions(route.size());
            for (size_t i = 0; i < route.size(); i++) {
                spdlog::info("Point {}", route[i]);
                positions[i] = traffic->findNodeByIndex(route[i]).plane();
            }
            MeshBuilder builder;
            generateWayMesh(builder, positions,
                streetSelectedHeight, streetWidth);
            auto expFile = builder.exporter().addVertex().addNormal().exportData();
            auto model = std::make_shared<GLModel>(expFile);
            auto routeEntity = std::make_shared<TransformableEntity>();
            routeEntity->getColorStorage().addColor({0.0f, 1.0f, 0.0f});
            routeEntity->setModel(model);
            m_entities->add(routeEntity);
        }
    });


    // Arrow keybindings
    m_keys[m_key_up] = m_input.loopKey(NYREM_KEY_UP).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->rotateUp(cameraSpeedRotateUp);
    });
    m_keys[m_key_down] = m_input.loopKey(NYREM_KEY_DOWN).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->rotateDown(cameraSpeedRotateUp);
    });
    m_keys[m_key_left] = m_input.loopKey(NYREM_KEY_LEFT).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->rotateLeft(cameraSpeedRotateDown);
    });
    m_keys[m_key_right] = m_input.loopKey(NYREM_KEY_RIGHT).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_camera->rotateRight(cameraSpeedRotateDown);
    });
    m_keys[m_key_h] = m_input.callbackKey(NYREM_KEY_H).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED && m_hasStart && m_hasEnd) {
            const auto &traffic = m_world->getTrafficGraph();
			TrafficGraphNodeIndex idStart = traffic->findClosestNodeIdxPlane({m_start.x, m_start.z});
			TrafficGraphNodeIndex idStop = traffic->findClosestNodeIdxPlane({m_end.x, m_end.z});
            m_world->createAgent(idStart, idStop);
        }
    });
    m_keys[m_key_z] = m_input.callbackKey(NYREM_KEY_Z).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_run = true; 
    });
    m_keys[m_key_x] = m_input.callbackKey(NYREM_KEY_X).listen(true, [this](KeyEvent e) {
        if (e.action == KEYSTATUS_PRESSED)
            m_run = false;
    });


}

void MapWorld::deactivate(nyrem::Navigator &nav)
{
    for (size_t i = 0; i < m_key_last; i++)
        m_keys[i].remove();
}

bool MapWorld::isRunning() const noexcept {
    return m_run;
}

void MapWorld::loadWorld(const std::shared_ptr<traffic::OSMSegment> &map) noexcept
{
    using std::vector;
    using namespace nyrem;

    OSMViewTransformer &transformer = *m_world->transformer();
    vector<vector<dvec2>> buildings = map->findBuildings();
    // calculates some meta information and the geographical center
    size_t nodeCount = 0;
    for (const auto& building : buildings)
        nodeCount += building.size();
    spdlog::info("Found {} Buildings with {} Nodes",
        buildings.size(), nodeCount);

    // converts the information to natural space
    vector<vector<vec2>> naturalSpace(buildings.size());
    for (size_t i = 0; i < buildings.size(); i++) {
        // the used references for the buildings that are transformed
        const auto& building = buildings[i];
        auto& naturalBuilding = naturalSpace[i];

        naturalBuilding.resize(building.size());
        for (size_t k = 0; k < buildings[i].size(); k++)
            naturalBuilding[k] = vec2(transformer.transform(building[k]));
    }
    // we don't need the original list of buildings anymore
    buildings.clear();

    // creates a list of MeshBuilders from the natural space
    float height = 4.0f;
    vec3 heightVector = {0.0f, height, 0.0f};

    vector<MeshBuilder> objectMeshes(naturalSpace.size());
    for (size_t i = 0; i < naturalSpace.size(); i++) {
        auto &naturalBuilding = naturalSpace[i];
        // creates the top polygon
        MeshBuilder2D top;
        top.addPolygon(naturalBuilding);
        top.invertWinding(false); // inverts the winding
        objectMeshes[i].add(top, 4.0f, false);
        // adds the walls
        for (size_t k = 0; k < naturalBuilding.size() - 1; k++) {
            vec2 x1 = naturalBuilding[k];
            vec2 x2 = naturalBuilding[k + 1];
            objectMeshes[i].addPlane(
                {x1.x, 0.0f, x1.y}, {x1.x, height, x1.y },
                {x2.x, height, x2.y}, {x2.x, 0.0f, x2.y }
            );
        }
    }

    MeshBuilder total;
    for (const auto& childMesh : objectMeshes)
        total.add(childMesh);

    ExportFile file = total.exporter()
        .addVertex().addNormal().exportData();
    models.push_back(std::make_shared<GLModel>(file));

    // creates the model entity
    m_world_entity = std::make_shared<TransformableEntity>();
    m_world_entity->setModel(models.front());

    std::cout << total.info() << std::endl;

    // ==== Generates the plane ==== //
    // finds the min and max coordinates
    vec2 mins = { 10000, 10000 }, maxs = { -10000, -10000};
    const auto& verts = total.getVertices();
    for (size_t i = 0; i < verts.size(); i++) {
        if (verts[i].x < mins.x) mins.x = verts[i].x;
        if (verts[i].z < mins.y) mins.y = verts[i].z;
        if (verts[i].x > maxs.x) maxs.x = verts[i].x;
        if (verts[i].z > maxs.y) maxs.y = verts[i].z;
    }
    float planeHeight = 0.0f;
    MeshBuilder plane;
    plane.addPlane(
        {mins.x, planeHeight, maxs.y},
        {maxs.x, planeHeight, maxs.y},
        {maxs.x, planeHeight, mins.y},
        {mins.x, planeHeight, mins.y}
    );
    ExportFile exportPlane = plane.exporter()
        .addVertex().addNormal().exportData();
    auto planeModel = std::make_shared<GLModel>(exportPlane);
    m_plane_entity = std::make_shared<TransformableEntity>();
    m_plane_entity->setModel(planeModel);
    m_plane_entity->setMaterial(std::make_shared<GLMaterial>(0.3, 0.7, 0.2, 2.0));
}

void MapWorld::generateWayMesh(
    nyrem::MeshBuilder &mesh,
    const std::vector<nyrem::vec2> &points,
    float height, float width)
{
    using namespace nyrem;
    vec2 last;
    for (size_t i = 0; i < points.size(); i++) {
        vec2 vec = points[i];
        if (i != 0) {
            vec2 diff = glm::normalize(last - vec);
            vec2 cross = vec2(diff.y, -diff.x) * width;

            mesh.addPlane(
                {last.x - cross.x, height, last.y - cross.y},
                {last.x + cross.x, height, last.y + cross.y},
                {vec.x + cross.x, height, vec.y + cross.y},
                {vec.x - cross.x, height, vec.y - cross.y}
            );
        }
        last = vec;
    }
}

void MapWorld::loadHighway(const std::shared_ptr<traffic::OSMSegment> &highwayMap) noexcept
{
    using std::vector;
    using namespace nyrem;
    OSMViewTransformer &trans = *m_world->transformer();

    MeshBuilder highwayMesh;
    const auto &ways = *(highwayMap->getWays());
    for (const auto &way : ways) {
        const auto& wayNodes = way.getNodes();
        std::vector<vec2> positions(wayNodes.size());
        for (size_t i = 0; i < wayNodes.size(); i++)
            positions[i] = vec2(trans.transform(
                highwayMap->getNode(wayNodes[i]).asVector()));
        generateWayMesh(highwayMesh, positions,
            streetHeight, streetWidth);
    }

    ExportFile exportHighway = highwayMesh.exporter()
        .addVertex().addNormal().exportData();
    auto streetModel = std::make_shared<GLModel>(exportHighway);
    m_highway_entity = std::make_shared<TransformableEntity>();
    m_highway_entity->setModel(streetModel);
    m_highway_entity->getColorStorage().addColor({1.0f, 0.0f, 0.0f});
    m_highway_entity->setMaterial(std::make_shared<GLMaterial>(0.3, 0.7, 0.2, 2.0));
}

void MapWorld::render(const nyrem::RenderContext &context)
{
    using namespace nyrem;
    auto& renderList = m_shader_stage->stageBuffer().renderList;
    renderList->clear();
    renderList->add(m_highway_entity);
    renderList->add(m_world_entity);
    renderList->add(m_plane_entity);

    for (const auto &fp : *m_entities)
        renderList->add(fp);

    static float t = 0.0f;
    t += 0.01f;

    const auto &agents = m_world->getAgents();
    for (const Agent& agent : agents) {
        auto agentEntity = std::make_shared<TransformableEntity>();
        vec2 physPosition = agent.physical().position();
            //+ nyrem::vec2(4.0f, 4.0f) * cos(t);
        agentEntity->translation().set(
            {physPosition.x, agentHeight, physPosition.y});
        agentEntity->setModel(m_cubeModel);
        agentEntity->getColorStorage().addColor({0.0f, 0.0f, 1.0f});
        renderList->add(agentEntity);
    }

    m_camera->projection().setAspectRatio(context.aspectRatio());
    m_pipeline.render(context);
}