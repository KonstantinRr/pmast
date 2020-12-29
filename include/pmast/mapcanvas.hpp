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

#include "engine.hpp"
#include "agent.hpp"
#include "osm.hpp"

#include "engine/listener.hpp"
#include "engine/shader.hpp"
#include "engine/window.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace traffic
{
/// <summary>
/// A canvas that is used to render a map to the screen. This canvas uses its own
/// OpenGL shaders to render a mesh of the map dynamically on the screen. It offers
/// some functions to manipulate the view matrix (zoom, rotation, translation).
/// </summary>
class MapCanvas : public nyrem::Renderable {
public:
	MapCanvas(
		std::shared_ptr<traffic::OSMSegment> map);

	nyrem::RenderPipeline& getPipeline();

	virtual void render(const nyrem::RenderContext &context);

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

	// ---- Callbacks ---- //
	inline Listener<void(glm::dvec2)>& cb_leftclick() { return m_cb_leftclick; }
	inline Listener<void(glm::dvec2)>& cb_rightclick() { return m_cb_rightclick; }
	inline Listener<void(glm::dvec2)>& cb_map_moved() { return m_cb_map_moved; }
	inline Listener<void(glm::dvec2)>& cb_cursor_moved() { return m_cb_cursor_moved; }
	inline Listener<void(traffic::Rect)>& cb_view_changed() { return m_cb_view_changed; }
	inline Listener<void(double)>& cb_zoom_changed() { return m_cb_zoom_changed; }
	inline Listener<void(double)>& cb_rotation_changed() { return m_cb_rotation_changed; }

	std::string info();

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

	Listener<void(glm::dvec2)> m_cb_leftclick;		// triggered on left click
	Listener<void(glm::dvec2)> m_cb_rightclick;		// triggered on right clcik
	Listener<void(glm::dvec2)> m_cb_map_moved;		// triggered if map moves
	Listener<void(glm::dvec2)> m_cb_cursor_moved;	// triggered if cursor moves
	Listener<void(traffic::Rect)> m_cb_view_changed;// triggered if view changes
	Listener<void(double)> m_cb_zoom_changed;		// triggered if zoom changes
	Listener<void(double)> m_cb_rotation_changed;	// triggered if rotation changes

	// contains the overall map mesh as well as the highway mesh
	std::shared_ptr<nyrem::TransformedEntity2D>
		l_mesh_map, l_mesh_highway;
	// contains a list of routes that are rendered on the screen
	std::vector<std::shared_ptr<nyrem::TransformedEntity2D>> l_mesh_routes;

	// ---- RenderPipeline ---- //
	nyrem::RenderPipeline l_pipeline;
	std::shared_ptr<nyrem::LineMemoryShader> l_shader;
	std::shared_ptr<nyrem::RenderComponent<
		nyrem::LineStageBuffer,
		nyrem::LineMemoryShader>> l_comp;


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
};

}
#endif
