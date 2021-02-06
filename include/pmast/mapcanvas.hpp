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

#ifndef MAPCANVAS_HPP
#define MAPCANVAS_HPP

#include <pmast/internal.hpp>
#include <pmast/agent.hpp>
#include <pmast/osm.hpp>

#include <engine/listener.hpp>
#include <engine/shader.hpp>
#include <engine/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace traffic {
	
class Agent; // externally defined Agent class
class Route; // externally defined Route class

/// <summary>
/// A canvas that is used to render a map to the screen. This canvas uses its own
/// OpenGL shaders to render a mesh of the map dynamically on the screen. It offers
/// some functions to manipulate the view matrix (zoom, rotation, translation).
/// </summary>
class MapCanvas : public nyrem::EngineStage {
public:
	MapCanvas(
		const std::shared_ptr<nyrem::Engine> &engine,
		const std::shared_ptr<traffic::World> &world);

	virtual ~MapCanvas() = default;

	inline nyrem::RenderPipeline& getPipeline() {
		return l_pipeline;
	}

	virtual void render(const nyrem::RenderContext &context) override;

	// ---- Position updates ---- //
	void setLatitude(double lat);
	void setLongitude(double lon);
	void setLatLon(double lat, double lon);
	void setPosition(glm::dvec2 pos);
	void setZoom(double zoom);
	void setRotation(double rotation);

	// ---- Positional reads ---- //
	double getLatitude() const;
	double getLongitude() const;
	double getCursorLatitude() const;
	double getCursorLongitude() const;
	glm::dvec2 getPosition() const;
	glm::dvec2 getCursor() const;
	glm::dvec2 getPositionPlane() const;
	glm::dvec2 getCursorPlane() const;

	double getDistance(glm::dvec2 p1, glm::dvec2 p2) const;

	double getZoom() const;
	double getRotation() const;
	double getMinZoom() const;
	double getMaxZoom() const;

	glm::dvec2 getCenter() const;

	void loadMap(std::shared_ptr<traffic::OSMSegment> map);
	void loadHighwayMap(std::shared_ptr<traffic::OSMSegment> map);
	void loadRoute(const traffic::Route &route, std::shared_ptr<traffic::OSMSegment> map);
	void clearRoutes();

	bool hasMap() const;

	// ---- Events ---- //

	glm::dvec2 scaleWindowDistance(glm::ivec2 vec);

	void applyTranslation(glm::dvec2 rel);
	void applyZoom(double iterations);
	void applyRotation(double radians);
	void resetView();

	// ---- Transformations ---- //
	// -- Window <-> View -- //
	glm::dvec2 windowToView(glm::ivec2 vec) const;
	glm::ivec2 viewToWindow(glm::dvec2 vec) const;
	// -- View <-> Plane -- //
	glm::dvec2 viewToPlane(const glm::dvec2 &pos) const;
	glm::dvec2 planeToView(const glm::dvec2 &pos) const;
	// -- Plane <-> Position -- //
	glm::dvec2 planeToPosition(const glm::dvec2 &pos) const;
	glm::dvec2 positionToPlane(const glm::dvec2 &pos) const;

	glm::dvec2 windowToPosition(glm::ivec2 vec) const;
	glm::ivec2 positionToWindow(glm::dvec2 vec) const;

	//Matrix3f transformPlaneToView3D() const;
	//Matrix4f transformPlaneToView4D() const;

	glm::mat3 transformPlaneToView3DGLM() const;
	glm::mat4 transformPlaneToView4DGLM() const;
	
	Rect rect() const; 

	std::shared_ptr<nyrem::ViewTransformer> asCamera() const noexcept;

	// ---- Callbacks ---- //
	inline nyrem::Listener<void(glm::dvec2)>& cb_leftclick() { return m_cb_leftclick; }
	inline nyrem::Listener<void(glm::dvec2)>& cb_rightclick() { return m_cb_rightclick; }
	inline nyrem::Listener<void(glm::dvec2)>& cb_map_moved() { return m_cb_map_moved; }
	inline nyrem::Listener<void(glm::dvec2)>& cb_cursor_moved() { return m_cb_cursor_moved; }
	inline nyrem::Listener<void(traffic::Rect)>& cb_view_changed() { return m_cb_view_changed; }
	inline nyrem::Listener<void(double)>& cb_zoom_changed() { return m_cb_zoom_changed; }
	inline nyrem::Listener<void(double)>& cb_rotation_changed() { return m_cb_rotation_changed; }

	std::string info();

	void setAgentList(const std::vector<Agent> &agentList);

	virtual void activate(nyrem::Navigator &nav) override;
	virtual void deactivate(nyrem::Navigator &nav) override;
protected:

	// ---- Mesh access ---- //
	/// <summary>Clears the currently used mesh</summary>
	void clearMesh();

	/// <summary>Generates a mesh with the given color from the segment</summary>
	std::shared_ptr<nyrem::TransformedEntity2D> genMeshFromMap(
		const traffic::OSMSegment &seg, glm::vec3 color);
	std::shared_ptr<nyrem::TransformedEntity2D> genMesh(
		std::vector<glm::vec2> &&points, std::vector<glm::vec3> &&colors);

	void setChunkMesh(
		const std::vector<glm::vec2>& points);

	// ---- Member variables ---- //
	std::shared_ptr<World> m_world;
	std::shared_ptr<nyrem::Engine> m_engine;

	nyrem::Listener<void(glm::dvec2)> m_cb_leftclick;		// triggered on left click
	nyrem::Listener<void(glm::dvec2)> m_cb_rightclick;		// triggered on right clcik
	nyrem::Listener<void(glm::dvec2)> m_cb_map_moved;		// triggered if map moves
	nyrem::Listener<void(glm::dvec2)> m_cb_cursor_moved;	// triggered if cursor moves
	nyrem::Listener<void(traffic::Rect)> m_cb_view_changed; // triggered if view changes
	nyrem::Listener<void(double)> m_cb_zoom_changed;		// triggered if zoom changes
	nyrem::Listener<void(double)> m_cb_rotation_changed;	// triggered if rotation changes

	nyrem::CallbackReturn<void(nyrem::KeyEvent)> m_key_p, m_key_o,
		m_key_k, m_key_l, m_key_left, m_key_right,
		m_key_up, m_key_down, m_key_g, m_key_r, m_key_t,
		m_key_enter, m_key_h; 

	// contains the overall map mesh as well as the highway mesh
	std::shared_ptr<nyrem::TransformedEntity2D> l_mesh_map, l_mesh_highway;
	// contains a list of routes that are rendered on the screen
	std::vector<std::shared_ptr<nyrem::TransformedEntity2D>> l_mesh_routes;

	std::vector<std::shared_ptr<nyrem::TransformableEntity2D>> m_entities;

	const std::vector<Agent> *m_agentList;

	// ---- RenderPipeline ---- //
	nyrem::RenderPipeline l_pipeline;
	std::shared_ptr<nyrem::LineMemoryShader> l_shader;
	std::shared_ptr<nyrem::RectShader> rect_shader;
	std::shared_ptr<nyrem::RectListStage> rect_comp;
	std::shared_ptr<nyrem::LineStage> l_comp;

	std::shared_ptr<nyrem::GLModel> m_model;

	nyrem::SizedObject *l_size;

	std::shared_ptr<traffic::OSMSegment> m_map;
	std::shared_ptr<traffic::OSMSegment> m_highway_map;

	// stores the plane coordinates
	glm::dvec2 position;
	// stores the last cursor plane coordinates
	glm::dvec2 cursor;
	double m_zoom;
	double m_rotation;

	double m_max_zoom;
	double m_min_zoom;

    static constexpr double rotateSpeed = 0.01;
	static constexpr double translateSpeed = 0.015;
	static constexpr double zoomSpeed = 1.5;

    bool hasStart = false;
	bool hasEnd = false;
    glm::dvec2 start{ 0.0 };
	glm::dvec2 end{ 0.0 };
};
}

#endif
