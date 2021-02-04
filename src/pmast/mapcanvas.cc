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

#include <pmast/mapcanvas.hpp>
#include <pmast/agent.hpp>
#include <pmast/osm_mesh.hpp>
#include <pmast/osm_graph.hpp>

#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>

using namespace traffic;
using namespace glm;

//// ---- MapCanvas ---- ////


MapCanvas::MapCanvas(
	const std::shared_ptr<OSMSegment> &world,
	const std::shared_ptr<traffic::OSMSegment> &highway)
	: m_min_zoom(2.0), m_max_zoom(1000.0)
{
	try {
		loadMap(world);
		loadHighwayMap(highway);
		resetView();

		using namespace nyrem; // is used a lot
		MeshBuilder2D rect = loadRect2D();
		m_model = std::make_shared<GLModel>(
			rect.exporter().addVertex().addTexture().exportData());

		// creates the shaders
		l_shader = make_shader<LineMemoryShader>();
		rect_shader = make_shader<MemoryRectShader>();
		// creates the components
		rect_comp = std::make_shared<RectListStage>(rect_shader);
		l_comp = std::make_shared<LineStage>(l_shader);

		l_pipeline.addStage(l_comp);
		l_pipeline.addStage(rect_comp);
	}
	catch (const std::exception &excp) {
		l_shader = nullptr;
		l_comp = nullptr;
		l_pipeline.clear();
		throw;
	}
}


// ---- Apply Changes ---- //

void MapCanvas::applyTranslation(glm::dvec2 rel)
{
	glm::dvec2 mx = glm::rotate(rel / m_zoom, -m_rotation);
	setPosition(position - mx);
}

void MapCanvas::applyZoom(double iterations)
{
	double scale = pow(0.99, iterations);
	double clamped = std::clamp(m_zoom * scale, m_min_zoom, m_max_zoom);
	setZoom(clamped);
}

void MapCanvas::applyRotation(double radians)
{
	setRotation(m_rotation + radians);
}

void MapCanvas::resetView()
{
	auto centerPoint = sphereToPlane(getCenter());
	position = { centerPoint.x, centerPoint.y };
	cursor = { 0.0, 0.0 };
	m_zoom = 25;
	m_rotation = 0.0;

	cb_map_moved().trigger(getPosition());
	cb_cursor_moved().trigger(getCursor());
	cb_rotation_changed().trigger(getRotation());
	cb_zoom_changed().trigger(getZoom());
}

void MapCanvas::setLatitude(double lat)
{
	setPosition(glm::dvec2(
		latitudeToPlane(lat, getCenter()),
		position.y));
}

void MapCanvas::setLongitude(double lon)
{
	setPosition(glm::dvec2(
		position.x,
		longitudeToPlane(lon, getCenter())));
}

void MapCanvas::setLatLon(double lat, double lon)
{
	setPosition(glm::dvec2(
		latitudeToPlane(lat, getCenter()),
		longitudeToPlane(lon, getCenter())));
}

void MapCanvas::setPosition(glm::dvec2 pos)
{
	position = pos;
	cb_map_moved().trigger(getPosition());
	cb_view_changed().trigger(rect());
}

void MapCanvas::setZoom(double zoom)
{
	m_zoom = zoom;
	cb_zoom_changed().trigger(getZoom());
	cb_view_changed().trigger(rect());
}

void MapCanvas::setRotation(double rotation)
{
	m_rotation = rotation;
	cb_rotation_changed().trigger(getRotation());
}

double MapCanvas::getLatitude() const { return planeToLatitude(position.x, getCenter()); }
double MapCanvas::getLongitude() const { return planeToLongitude(position.y, getCenter()); }
double MapCanvas::getCursorLatitude() const { return planeToLatitude(cursor.x, getCenter()); }
double MapCanvas::getCursorLongitude() const { return planeToLongitude(cursor.y, getCenter()); }
glm::dvec2 MapCanvas::getCursor() const { return glm::dvec2(getCursorLatitude(), getCursorLongitude()); }
glm::dvec2 MapCanvas::getPosition() const { return glm::dvec2(getLatitude(), getLongitude()); }
glm::dvec2 MapCanvas::getPositionPlane() const { return position;}
glm::dvec2 MapCanvas::getCursorPlane() const { return cursor; }

double MapCanvas::getZoom() const { return m_zoom; }
double MapCanvas::getRotation() const { return m_rotation; }
double MapCanvas::getMinZoom() const { return m_min_zoom; }
double MapCanvas::getMaxZoom() const { return m_max_zoom; }

glm::dvec2 MapCanvas::scaleWindowDistance(glm::ivec2 vec) {
	return glm::dvec2(
		vec.x * 2.0 / context.width(), // TODO
		-vec.y * 2.0 / context.width()); // TODO
}

double MapCanvas::getDistance(glm::dvec2 p1, glm::dvec2 p2) const {
	return distance(p1, p2);
}

glm::dvec2 MapCanvas::getCenter() const
{
	return m_map ?
		glm::dvec2(m_map->getBoundingBox().getCenter().toVec()) :
		glm::dvec2(0.0, 0.0);
}

// ---- Loading Routes ---- //

void MapCanvas::loadMap(std::shared_ptr<traffic::OSMSegment> map)
{
	if (map) {
		m_map = map;
		l_mesh_map = genMeshFromMap(*map, { 1.0f, 1.0f, 1.0f});
		resetView();
	}
}

void MapCanvas::loadHighwayMap(std::shared_ptr<traffic::OSMSegment> map)
{
	if (map) {
		m_highway_map = map;
		l_mesh_highway = genMeshFromMap(*map, { 1.0f, 0.0f, 0.0f });
		resetView();
	}
}

void MapCanvas::loadRoute(const Route& route, std::shared_ptr<traffic::OSMSegment> map)
{
	std::vector<vec2> points = generateRouteMesh(route, *map);
	std::vector<vec3> colors(points.size(), glm::vec3(0.0f, 0.0f, 1.0f));
	l_mesh_routes.push_back(
		genMesh(std::move(points), std::move(colors)));
}

void MapCanvas::clearRoutes()
{
	l_mesh_routes.clear();
}



bool MapCanvas::hasMap() const { return m_map.get(); }

// ---- Mesh ---- //

std::shared_ptr<nyrem::TransformedEntity2D> MapCanvas::genMeshFromMap(
	const OSMSegment& seg, glm::vec3 color)
{
	std::vector<vec2> points = generateMesh(seg);
	std::vector<vec3> colors(points.size(), color);
	return genMesh(std::move(points), std::move(colors));
}

std::shared_ptr<nyrem::TransformedEntity2D> MapCanvas::genMesh(
	std::vector<glm::vec2>&& points, std::vector<glm::vec3>&& colors)
{
	nyrem::MeshBuilder2D builder;
	builder.setVertices(std::move(points));
	builder.setColors(std::move(colors));

	spdlog::info(builder.info());

	auto exp = builder.exporter()
		.addVertex().addColor()
		.exportData();

	auto model = std::make_shared<nyrem::GLModel>(exp);
	return std::make_shared<nyrem::TransformedEntity2D>(0, model);
}

void MapCanvas::clearMesh() {
	l_mesh_highway = nullptr;
	l_mesh_map = nullptr;
	l_mesh_routes.clear();
}

glm::dvec2 MapCanvas::windowToView(glm::ivec2 vec) const {
	return glm::dvec2(
		vec.x * 2.0 / context.width() - 1.0,
		(context.height() - vec.y) * 2.0 / context.height() - 1.0);
}

glm::ivec2 MapCanvas::viewToWindow(glm::dvec2 vec) const {
	return glm::ivec2(
		(int32_t)((vec.x + 1.0) / 2.0 * context.width()),
		-((int32_t)((vec.y + 1.0) / 2.0 * context.height()) - context.height())
	);
}

glm::dvec2 MapCanvas::planeToView(const glm::dvec2& pos) const
{
	glm::dvec2 f = pos - position;
	f = glm::rotate(f, m_rotation);
	f = f * dvec2(m_zoom, m_zoom * context.width() / context.height());
	return f;
}

glm::dvec2 MapCanvas::viewToPlane(const glm::dvec2& pos) const
{
	glm::dvec2 f = pos / dvec2(m_zoom, m_zoom * context.width() / context.height());
	f = glm::rotate(f, -m_rotation);
	f = f + position;
	return f;
}

glm::dvec2 MapCanvas::planeToPosition(const glm::dvec2& pos) const
{
	return planeToSphere(pos, getCenter());
}

glm::dvec2 MapCanvas::positionToPlane(const glm::dvec2& pos) const
{
	return sphereToPlane(pos, getCenter());
}

glm::dvec2 MapCanvas::windowToPosition(glm::ivec2 vec) const
{
	dvec2 view = windowToView(vec);
	dvec2 plane = viewToPlane(view);
	dvec2 pos = planeToPosition(plane);
	return pos;
}

glm::ivec2 MapCanvas::positionToWindow(glm::dvec2 vec) const
{
	dvec2 plane = positionToPlane(vec);
	dvec2 view = planeToView(plane);
	ivec2 window = viewToWindow(view);
	return window;
}

glm::mat3 MapCanvas::transformPlaneToView3DGLM() const {
	glm::mat3 mat(1.0f);
	mat = glm::scale(mat, glm::vec2(m_zoom, m_zoom * context.width() / context.height()));
	mat = glm::rotate(mat, (float)m_rotation);
	mat = glm::translate(mat, glm::vec2(-position.x, -position.y));
	return mat;
}

glm::mat4 MapCanvas::transformPlaneToView4DGLM() const {
	glm::mat4 mat(1.0f);
	mat = glm::scale(mat, glm::vec3(m_zoom, m_zoom * context.width() / context.height(), 1.0f));
	mat = glm::rotate(mat, (float)m_rotation, {0.0F, 0.0F, 1.0F});
	mat = glm::translate(mat, glm::vec3(-position.x, -position.y, 0.0f));
	return mat;
}

std::shared_ptr<nyrem::ViewTransformer> MapCanvas::asCamera() const noexcept
{
	return std::make_shared<nyrem::ViewTransformer>(
		nyrem::TransformedCamera<>(transformPlaneToView4DGLM()));
}

void MapCanvas::render(const nyrem::RenderContext &context)
{
	// calls the parent render functions
	storeContext(context);
	if (hasMap()) {
		auto transform = transformPlaneToView4DGLM();
		l_mesh_map->setTransformationMatrix(transform);
		l_mesh_highway->setTransformationMatrix(transform);
		
		nyrem::RenderList<nyrem::Entity2D> &renderList =
			l_comp->stageBuffer().renderList;
		renderList.clear();
		// adds the two basic meshes
		renderList.add(l_mesh_highway);
		renderList.add(l_mesh_map);
		// adds any additional route meshes
		for (const auto& t : l_mesh_routes) {
			t->setTransformationMatrix(transform);
			renderList.add(t);
		}

		// adds all agents
		if (m_agentList != nullptr) {
			rect_comp->stageBuffer().renderList.clear();
			rect_comp->stageBuffer().camera = std::make_shared<
				nyrem::TransformedCamera<>>(transform);
			for (const Agent &agent : *m_agentList) {
				//auto a = std::make_shared<nyrem::TransformableEntity2D>(nyrem::TransformableEntity2D(1,
				//	nullptr, nullptr, { nyrem::vec3{0.0f, 1.0f, 0.0f} },
				//	agent.physical().position(), { 0.01f, 0.01f }, 0.0f
				//));
				rect_comp->stageBuffer().renderList.add(
					std::make_shared<nyrem::TransformableEntity2D>(nyrem::TransformableEntity2D(1,
						m_model, nullptr, { nyrem::vec3{0.0f, 1.0f, 0.0f} },
						agent.physical().position(),
						{ 0.00008f, 0.00008f }, 0.0f
					))
				);
			}
		}

		l_pipeline.render(context);
	}
}

Rect MapCanvas::rect() const {
	return Rect::fromCenter(
		position.x, position.y, m_zoom, m_zoom);
}

std::string MapCanvas::info() {
	return fmt::format("MapCanvas Object\n"
		"\tposition: {} {}\n"
		"\tcursor:   {} {}\n"
		"\trotation: {}\n"
		"\tzoom:     {}\n"
		"\trect:     {}\n",
		position.x, position.y,
		cursor.x, cursor.y,
		m_rotation, m_zoom,
		rect().summary());
}

void traffic::MapCanvas::setAgentList(const std::vector<Agent>& agentList)
{
	m_agentList = &agentList;
}
