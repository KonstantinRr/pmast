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

#include <engine/resource.hpp>
#include <engine/util.hpp>

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/norm.hpp>
#include <fmt/format.h>

#include <limits>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include <mapbox/earcut.hpp>

NYREM_USE_NAMESPACE

// Defines the accessor for vectors
namespace mapbox {
namespace util {

template <>
struct nth<0, vec2> {
    inline static auto get(const vec2 &t) {
        return t.x;
    };
};
template <>
struct nth<1, vec2> {
    inline static auto get(const vec2 &t) {
        return t.y;
    };
};

} // namespace util
} // namespace mapbox


std::vector<char> nyrem::readFile(const std::string &file)
{
	spdlog::info("Opening resource {}", file);
	FILE *f = fopen(file.c_str(), "rb");
	if (!f) throw std::runtime_error("Could not open File");
	int rt;
	rt = fseek(f, 0, SEEK_END);
	if (rt != 0) throw std::runtime_error("Could not seek file");
	size_t fsize = static_cast<size_t>(ftell(f));
	rt = fseek(f, 0, SEEK_SET);
	if (rt != 0) throw std::runtime_error("Could not seek file");

	std::vector<char> data;
	data.resize(fsize + 1);
	size_t readBytes = fread(data.data(), 1, fsize, f);
	data[fsize] = 0;

	if (readBytes != fsize)
		throw std::runtime_error("Bytes read did not match file size");

	rt = fclose(f);
	if (rt != 0) throw std::runtime_error("Could not close file");
	return data;
}

std::vector<int> generateIndices(size_t length) {
	std::vector<int> indices(length);
	for (size_t i = 0; i < length; i++)
		indices[i] = i;
	return indices;
}

/////////////////////////////
//// ---- HeightMap ---- ////
HeightMap::HeightMap(size_t size) {
    heightMap.resize(size, std::vector<float>(size));
}
HeightMap::HeightMap(const std::string &) {
	//LTImage image = loadImage(filename);
    ////image = image.scaled(imagePrev.width()/4, imagePrev.height()/4);
    //size_t size = static_cast<size_t>(image.size().width());
    //heightMap.resize(size, std::vector<float>(size));
    //for (size_t x = 0; x < size; x++) {
    //    for (size_t y = 0; y < size; y++) {
    //        QColor pixel = image.pixelColor(x, y); // Alpha component
    //        heightMap[x][y] = pixel.redF();
    //    }
    //}
}
void HeightMap::scaleHeight(float scale) {
    for (auto &line : heightMap) {
        for (auto &val : line) {
            val *= scale;
        }
    }
}

void HeightMap::fillRandom() {
    for (auto &line : heightMap) {
        for (auto &val : line) {
            val = static_cast<float>(rand())
                    / static_cast<float>(RAND_MAX);
        }
    }
}


ExportMacro::ExportMacro(ExportType type)
	: type(type)
{
	switch(type) {
		case EXPORT_VERTEX: size = 2; break;
		case EXPORT_VERTEX_INDEXED: size = 2; break;
		case EXPORT_TEXTURE: size = 2; break;
		case EXPORT_TEXTURE_INDEXED: size = 2; break;
		case EXPORT_COLOR: size = 3; break;
		case EXPORT_COLOR_INDEXED: size = 3; break;
		case EXPORT_VERTEX_3D: size = 3; break;
		case EXPORT_VERTEX_INDEXED_3D: size = 3; break;
		case EXPORT_NORMAL_3D: size = 3; break;
		case EXPORT_NORMAL_INDEXED_3D: size = 3; break;
		default: spdlog::error("Unknown ExportType macro!");
	}
}

size_t ExportFile::strideSize() const noexcept {
	size_t s = 0;
	for (ExportMacro macro : exp)
		s += macro.size;
	return s;
}
std::string ExportFile::info() const {
	return fmt::format("ExportFile[dataSize: {}, exportMacros: {}]",
		data.size(), exp.size());
}
std::string ExportFile::detail(char seperator, bool separateSegments) const {
	FastSStream stream(
		data.size() * 3 + exp.size() * 3 + 2,
		data.size() + exp.size());
	
	size_t rowLengthMacro = 1;
	
	stream.add("==== Detailed Export File Data Report: ====");
	for (size_t i = 0; i < data.size();) {
		stream.add('\n');
		for (const auto &expMacro : exp) {
			for (size_t k = 0; k < expMacro.size; k++) {
				stream.add(data[i]);
				stream.add(seperator);
				i++;
			}
			if (separateSegments) {
				stream.add('|');
				stream.add(seperator);
			}
		}
		stream.add('\t');
	}

	stream.add("\n====Detailed Export Macro Report:====");
	for (size_t i = 0; i < exp.size(); i++) {
		if (i % rowLengthMacro == 0)
			stream.add('\n');
		stream.add(exportNames[exp[i].type].exportName);
		stream.add(seperator);
	}
	return stream.generate();
}

template<typename Type, typename VType>
size_t findClosestIndex(
	Type &&exec, const std::vector<VType> &vertices)
{
	size_t current = 0;
	for (size_t i = 1; i < vertices.size(); i++) {
		if (exec(vertices[i], vertices[current]))
			current = i;
	}
	return current;
}

template<typename VType>
size_t impl_minExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return glm::length2(v1) < glm::length2(v2); }, vertices);
}
template<typename VType>
size_t impl_maxExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return glm::length2(v1) > glm::length2(v2); }, vertices);
}
// Single Coordinates
template<typename VType>
size_t impl_maxXExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return v1.x > v2.x; }, vertices);
}
template<typename VType>
size_t impl_minXExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return v1.x < v2.x; }, vertices);
}
template<typename VType>
size_t impl_maxYExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return v1.y > v2.y; }, vertices);
}
template<typename VType>
size_t impl_minYExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return v1.y < v2.y; }, vertices);
}
template<typename VType>
size_t impl_maxZExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return v1.z > v2.z; }, vertices);
}
template<typename VType>
size_t impl_minZExtentIndex(const std::vector<VType> &vertices) {
	return findClosestIndex([](const VType &v1, const VType &v2) {
		return v1.z < v2.z; }, vertices);
}

// ---- Helper functions ---- //
void addVec2(std::vector<float>& data, glm::vec2 vec)
{
	data.push_back(vec.x);
	data.push_back(vec.y);
}

void addVec3(std::vector<float>& data, glm::vec3 vec)
{
	data.push_back(vec.x);
	data.push_back(vec.y);
	data.push_back(vec.z);
}

/////////////////////////////////
//// ---- MeshBuilder2D ---- ////
MeshBuilder2D::MeshBuilder2D() {}
MeshBuilder2D::MeshBuilder2D(
    const std::vector<glm::vec2> &pVertices,
    const std::vector<glm::vec2> &pTexCoords,
	const std::vector<glm::vec3> &colors,
    const std::vector<int> &pV_indices,
    const std::vector<int> &pVt_indices)
	: vertices(pVertices), texCoords(pTexCoords), colors(colors),
	v_indices(pV_indices), vt_indices(pVt_indices) { }

void MeshBuilder2D::clear() {
	vertices.clear();
	texCoords.clear();
	colors.clear();
	v_indices.clear();
	vt_indices.clear();
}

MeshBuilder2D::Exporter2D MeshBuilder2D::exporter() const { return Exporter2D(this); }

size_t MeshBuilder2D::maxExtentIndex() const { return impl_maxExtentIndex(vertices); }
size_t MeshBuilder2D::minExtentIndex() const { return impl_minExtentIndex(vertices); }
size_t MeshBuilder2D::maxXExtentIndex() const { return impl_maxXExtentIndex(vertices); }
size_t MeshBuilder2D::minXExtentIndex() const { return impl_minXExtentIndex(vertices); }
size_t MeshBuilder2D::maxYExtentIndex() const { return impl_maxYExtentIndex(vertices); }
size_t MeshBuilder2D::minYExtentIndex() const { return impl_minYExtentIndex(vertices); }

float MeshBuilder2D::maxExtent() const { return vertices.empty() ? 0.0f : glm::length(vertices[maxExtentIndex()]); }
float MeshBuilder2D::minExtent() const { return vertices.empty() ? 0.0f : glm::length(vertices[minExtentIndex()]); }
float MeshBuilder2D::maxXExtent() const { return vertices.empty() ? 0.0f : vertices[maxXExtentIndex()].x; }
float MeshBuilder2D::minXExtent() const { return vertices.empty() ? 0.0f : vertices[minXExtentIndex()].x; }
float MeshBuilder2D::maxYExtent() const { return vertices.empty() ? 0.0f : vertices[maxYExtentIndex()].y; }
float MeshBuilder2D::minYExtent() const { return vertices.empty() ? 0.0f : vertices[minYExtentIndex()].y; }

glm::vec2 MeshBuilder2D::center() const {
	return glm::vec2(
		(maxXExtent() + minXExtent()) / 2.0,
		(maxYExtent() + minYExtent()) / 2.0
	);
}

glm::vec2 MeshBuilder2D::massCenter() const {
	glm::vec2 average = { 0.0f, 0.0f };
	for (const glm::vec2 vec : vertices)
		average += vec;
	return average / static_cast<float>(vertices.size());
}

MeshBuilder2D& MeshBuilder2D::scale(float scale) {
	for (size_t i = 0; i < vertices.size(); i++)
		vertices[i] *= scale;
	return *this;
}

MeshBuilder2D& MeshBuilder2D::scale(float scaleX, float scaleY) {
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].x *= scaleX;
		vertices[i].y *= scaleY;
	}
	return *this;
}

MeshBuilder2D& MeshBuilder2D::translate(glm::vec2 translation) {
	for (size_t i = 0; i < vertices.size(); i++)
		vertices[i] += translation;
	return *this;
}

MeshBuilder2D& MeshBuilder2D::rotate(float angle) {
	//glm::rotate() // TODO
	return *this;
}

MeshBuilder2D& MeshBuilder2D::transform(const glm::mat2x2 &mat) {
	for (size_t i = 0; i < vertices.size(); i++)
		vertices[i] = mat * vertices[i];
	return *this;
}
MeshBuilder2D& MeshBuilder2D::transform(const glm::mat3x3 &mat) {
	for (size_t i = 0; i < vertices.size(); i++)
		vertices[i] = glm::vec2(mat * glm::vec3(vertices[i], 1.0));
	return *this;
}
MeshBuilder2D& MeshBuilder2D::transform(const glm::mat4x4 &mat) {
	for (size_t i = 0; i < vertices.size(); i++)
		vertices[i] = glm::vec2(mat * glm::vec4(vertices[i], 1.0, 1.0));
	return *this;
}

glm::vec2 MeshBuilder2D::unitize(float unitScale) {
	if (vertices.empty())
		return glm::vec2(1.0f, 1.0f);
	float sc = unitScale / maxExtent();
	scale(sc);
	return glm::vec2(sc, sc);
}

glm::vec2 MeshBuilder2D::unitizeAxis(float unitScale, bool keepProportion) {
	if (vertices.empty())
		return glm::vec2(1.0f, 1.0f);
	
	if (keepProportion) {
		float sc = unitScale /
			std::max(maxXExtent(), maxYExtent());
		scale(sc);
		return glm::vec2(sc, sc);
	} else {
		glm::vec2 sc = {
			unitScale / maxXExtent(),
			unitScale / maxYExtent()};
		scale(sc.x, sc.y);
		return sc;
	}
}

glm::vec2 MeshBuilder2D::centerModel() {
	glm::vec2 trans = -center();
	translate(trans);
	return trans;
}

glm::mat3x3 MeshBuilder2D::unitizeMatrix(float unitScale, bool keepProportion) const {
	if (keepProportion) {
		float sc = unitScale / maxExtent();
		return glm::scale(glm::mat3(1.0f), glm::vec2(sc, sc));
	} else {
		return glm::scale(glm::mat3(1.0f), glm::vec2(
			unitScale / maxXExtent(),
			unitScale / maxYExtent()
		));
	}	
}

glm::mat3x3 MeshBuilder2D::centerMatrix() const {
	return glm::translate(glm::mat3(1.0f), -center());
}


void MeshBuilder2D::addVertex(glm::vec2 vertex) { vertices.push_back(vertex); }
void MeshBuilder2D::addTextureCoord(glm::vec2 vertex) { texCoords.push_back(vertex); }
void MeshBuilder2D::addColor(glm::vec3 color) { colors.push_back(color); }

MeshBuilder2D& MeshBuilder2D::addMesh(const MeshBuilder2D &mesh) noexcept
{
	// appends the mesh data
	addVertices(mesh.getVertices().begin(), mesh.getVertices().end());
	addTextureCoords(mesh.getTextureCoords().begin(), mesh.getTextureCoords().end());
	addColors(mesh.getColors().begin(), mesh.getColors().end());

	size_t vSize = v_indices.size();
	size_t vcSize = vc_indices.size();
	size_t vtSize = vt_indices.size();
	// reserves the necessary space for the indices
	v_indices.reserve(v_indices.size() + mesh.getV_indices().size());
	vc_indices.reserve(vc_indices.size() + mesh.getVc_indices().size());
	vt_indices.reserve(vt_indices.size() + mesh.getVt_indices().size());

	for (IndexType idx : mesh.getV_indices())
		v_indices.push_back(vSize + idx);
	for (IndexType idx : mesh.getVc_indices())
		vc_indices.push_back(vcSize + idx);
	for (IndexType idx : mesh.getVt_indices())
		vt_indices.push_back(vtSize + idx);
	return *this;
}

//MeshBuilder2D& MeshBuilder2D::addCircle(size_t pcount, float radius, bool strip=false)
//{
//	float angleDiff = 2 * 3.141592653589793238f / static_cast<float>(pcount);
//	if (strip) {
//		vertices.push_back()
//		for (size_t i = 0; i < pcount; i++) {
//			float angle = angleDiff * static_cast<float>(i);
//			vertices.push_back({
//				radius * cosf(angle),
//				radius * sinf(angle) 
//			});
//		}
//		return *this;
//	} else {
//	}
//}
MeshBuilder2D& MeshBuilder2D::addRect(float x, float y, float width, float height)
{
	return addRect({x, y}, {width, height});
}

MeshBuilder2D& MeshBuilder2D::addRect(vec2 center, vec2 offset) {
	vec2 newVertices[6] = {
		center + vec2(-offset.x, -offset.y),
		center + vec2( offset.x,  offset.y),
		center + vec2(-offset.x,  offset.y),
		center + vec2(-offset.x, -offset.y),
		center + vec2( offset.x, -offset.y),
		center + vec2( offset.x,  offset.y)
	};
	vec2 newTexCoords[] = {
		{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}
	};
	addVertices(std::begin(newVertices), std::end(newVertices));
	addTextureCoords(std::begin(newTexCoords), std::end(newTexCoords));
	return *this;
}

struct PolygonStorage {
protected:
	std::vector<const std::vector<vec2>*> m_data;
public:
	PolygonStorage(const std::vector<vec2> &begin,
		const std::vector<std::vector<vec2>> &holes = {}) noexcept {
		add(begin);
		std::tuple
		addAll(holes);
	}

	void addAll(const std::vector<std::vector<vec2>> &dataList) {
		m_data.reserve(m_data.size() + dataList.size());
		for (const auto &data : dataList)
			m_data.push_back(&data);
	}

	void add(const std::vector<vec2> &data) noexcept {
		m_data.push_back(&data);
	}

	vec2 findByIdx(size_t idx) {
		size_t dataIdx = 0;
		while (idx >= m_data[dataIdx]->size()) {
			idx -= m_data[dataIdx]->size();
			dataIdx++;
		}
		return (*m_data[dataIdx])[idx];
	}

	bool empty() const noexcept { return m_data.empty(); }
	size_t size() const noexcept { return m_data.size(); }
	const std::vector<vec2>& operator[](size_t idx) const { return *(m_data[idx]); }

};

MeshBuilder2D& MeshBuilder2D::addPolygon(
	const std::vector<vec2> &polygon, const std::vector<std::vector<vec2>> &holes, bool indexed) {
	
	using N = uint32_t;

	PolygonStorage storage(polygon, holes);
	storage.addAll(holes);
	std::vector<N> indices = mapbox::earcut<N>(storage);

	vertices.reserve(vertices.size() + indices.size());
	for (auto idx : indices)
		vertices.push_back(storage.findByIdx(idx));
	return *this;
}

MeshBuilder2D& MeshBuilder2D::addVertices(std::initializer_list<glm::vec2> init) {
	return addVertices(init.begin(), init.end());
}
MeshBuilder2D& MeshBuilder2D::addTextureCoords(std::initializer_list<glm::vec2> init) {
	return addTextureCoords(init.begin(), init.end());
}
MeshBuilder2D& MeshBuilder2D::addColors(std::initializer_list<glm::vec3> init) {
	return addColors(init.begin(), init.end());
}

void MeshBuilder2D::setVertices(const std::vector<glm::vec2> &vertices) { this->vertices = vertices; }
void MeshBuilder2D::setTextureCoords(const std::vector<glm::vec2> &texCoords) { this->texCoords = texCoords; }
void MeshBuilder2D::setColors(const std::vector<glm::vec3>& colors) { this->colors = colors;}

void MeshBuilder2D::setVertices(std::vector<glm::vec2>&& vertices) { this->vertices = std::move(vertices); }
void MeshBuilder2D::setTextureCoords(std::vector<glm::vec2>&& textureCoords) { this->texCoords = std::move(textureCoords); }
void MeshBuilder2D::setColors(std::vector<glm::vec3>&& colors) { this->colors = std::move(colors); }

void MeshBuilder2D::generateDefaultIndices()
{
	v_indices = generateIndices(vertices.size());
	vt_indices = generateIndices(texCoords.size());
	vc_indices = generateIndices(colors.size());
}

void MeshBuilder2D::generateNormals(bool indexed) {
	if (indexed) {

	} else {
		// TODO
	}
}

void MeshBuilder2D::setVIndices(const std::vector<int> &pv_indices) { this->v_indices = pv_indices; }
void MeshBuilder2D::setVTIndices(const std::vector<int> &pvt_indices) { this->vt_indices = pvt_indices; }

void MeshBuilder2D::setVIndices(std::vector<int>&& v_indices) { this->vt_indices = std::move(v_indices); }
void MeshBuilder2D::setVTIndices(std::vector<int>&& vt_indices) { this->vt_indices = std::move(vt_indices); }

const std::vector<glm::vec2>& MeshBuilder2D::getVertices() const { return vertices; }
const std::vector<glm::vec2>& MeshBuilder2D::getTextureCoords() const { return texCoords; }
const std::vector<glm::vec3>& MeshBuilder2D::getColors() const { return colors; }
const std::vector<int>& MeshBuilder2D::getV_indices() const { return v_indices; }
const std::vector<int>& MeshBuilder2D::getVc_indices() const { return vc_indices; }
const std::vector<int>& MeshBuilder2D::getVt_indices() const { return vt_indices; }


std::string MeshBuilder2D::info() {
	return fmt::format("MeshBuilder2D Object\n"
		"\tVertices  {} indices {}\n"
		"\tTexCoords {} indices {}\n"
		"\tColors    {} indices {}\n"
		"\tminX {}, maxX {}\n"
		"\tminY {}, maxY {}\n",
		vertices.size(), v_indices.size(),
		texCoords.size(), vt_indices.size(),
		colors.size(), vc_indices.size(),
		minXExtent(), maxXExtent(),
		minYExtent(), maxYExtent()
	);
}

///////////////////////////////
//// ---- MeshBuilder ---- ////

MeshBuilder::Exporter3D::Exporter3D(const MeshBuilder *builder)
	: builder(builder) { }

MeshBuilder::Exporter3D& MeshBuilder::Exporter3D::addVertex(bool indexed) {
	exp.push_back(indexed ? EXPORT_VERTEX_INDEXED_3D : EXPORT_VERTEX_3D);
	return *this;
}
MeshBuilder::Exporter3D& MeshBuilder::Exporter3D::addTexture(bool indexed) {
	exp.push_back(indexed ? EXPORT_TEXTURE_INDEXED : EXPORT_TEXTURE);
	return *this;
}
MeshBuilder::Exporter3D& MeshBuilder::Exporter3D::addColor(bool indexed) {
	exp.push_back(indexed ? EXPORT_COLOR_INDEXED : EXPORT_COLOR);
	return *this;
}
MeshBuilder::Exporter3D& MeshBuilder::Exporter3D::addNormal(bool indexed) {
	exp.push_back(indexed ? EXPORT_NORMAL_INDEXED_3D : EXPORT_NORMAL_3D);
	return *this;
}

bool MeshBuilder::Exporter3D::resolveTypes(std::vector<float> &data, size_t i) const
{
	for (ExportType type : exp) {
		switch (type) {
			case EXPORT_VERTEX_3D:
				if (i >= builder->getVertices().size()) return false;
				addVec3(data, builder->vertices[i]); break;
			case EXPORT_VERTEX_INDEXED_3D:
				if (i >= builder->getVIndices().size()) return false;
				addVec3(data, builder->vertices[builder->v_indices[i]]); break;
			case EXPORT_TEXTURE:
				if (i >= builder->getTextureCoords().size()) return false;
				addVec2(data, builder->getTextureCoords()[i]); break;
			case EXPORT_TEXTURE_INDEXED:
				if (i >= builder->getVTIndices().size()) return false;
				addVec2(data, builder->texcoords[builder->vt_indices[i]]); break;
			case EXPORT_COLOR:
				if (i >= builder->getColors().size()) return false;
				addVec3(data, builder->getColors()[i]); break;
			case EXPORT_COLOR_INDEXED:
				if (i >= builder->getVCIndices().size()) return false;
				addVec3(data, builder->colors[builder->vc_indices[i]]); break;				
			case EXPORT_NORMAL_3D:
				if (i >= builder->getNormals().size()) return false;
				addVec3(data, builder->getNormals()[i]); break;
			case EXPORT_NORMAL_INDEXED_3D:
				if (i >= builder->getVNIndices().size()) return false;
				addVec3(data, builder->normals[builder->vn_indices[i]]); break;
			default: break;
		}
	}
	return true;
}

ExportFile MeshBuilderBase::exportData() const
{
	ExportFile expFile;
	for (ExportType type : exp)
		expFile.exp.push_back(ExportMacro(type));
	
	size_t strideSize = expFile.strideSize();
	std::vector<float> tempData;
	tempData.reserve(strideSize);
	
	bool success = true;
	for (size_t i = 0; success; i++) {
		tempData.clear();
		success = resolveTypes(tempData, i);
		if (success) {
			expFile.data.insert(expFile.data.end(),
				tempData.begin(), tempData.end());
		}
	}
	return expFile;
}

size_t MeshBuilderBase::size() const noexcept {
	return exp.size();
}

MeshBuilder::MeshBuilder() { }
MeshBuilder::MeshBuilder(
    const std::vector<glm::vec3> &pVertices,
    const std::vector<glm::vec3> &pNormals,
    const std::vector<glm::vec2> &pTexcoords,
    const std::vector<int>& pV_indices,
    const std::vector<int>& pVn_indices,
    const std::vector<int>& pVt_indices)
	: vertices(pVertices), normals(pNormals), texcoords(pTexcoords),
	v_indices(pV_indices), vn_indices(pVn_indices), vt_indices(pVt_indices) { }

void MeshBuilder::clear() {
	vertices.clear();
	normals.clear();
	texcoords.clear();
	colors.clear();
	v_indices.clear();
	vn_indices.clear();
	vt_indices.clear();
}

MeshBuilder::Exporter3D MeshBuilder::exporter() const { return Exporter3D(this); }

float MeshBuilder::maxExtent() const { return glm::length(vertices[maxExtentIndex()]); }
float MeshBuilder::minExtent() const { return glm::length(vertices[minExtentIndex()]); }

// Values calculated as ABS
float MeshBuilder::maxXExtent() const { return vertices[maxXExtentIndex()].x; }
float MeshBuilder::minXExtent() const { return vertices[minXExtentIndex()].x; }
float MeshBuilder::maxYExtent() const { return vertices[maxYExtentIndex()].y; }
float MeshBuilder::minYExtent() const { return vertices[minYExtentIndex()].y; }
float MeshBuilder::maxZExtent() const { return vertices[maxZExtentIndex()].z; }
float MeshBuilder::minZExtent() const { return vertices[minZExtentIndex()].z; }

size_t MeshBuilder::maxExtentIndex() const { return impl_maxExtentIndex(vertices); }
size_t MeshBuilder::minExtentIndex() const { return impl_minExtentIndex(vertices); }

size_t MeshBuilder::maxXExtentIndex() const { return impl_maxXExtentIndex(vertices); }
size_t MeshBuilder::minXExtentIndex() const { return impl_minXExtentIndex(vertices); }
size_t MeshBuilder::maxYExtentIndex() const { return impl_maxYExtentIndex(vertices); }
size_t MeshBuilder::minYExtentIndex() const { return impl_minYExtentIndex(vertices); }
size_t MeshBuilder::maxZExtentIndex() const { return impl_maxZExtentIndex(vertices); }
size_t MeshBuilder::minZExtentIndex() const { return impl_minZExtentIndex(vertices); }

void MeshBuilder::add(
	const MeshBuilder2D &mesh, float height,
	bool srcIndex, bool dstIndex) noexcept
{
	const auto& verts2D = mesh.getVertices();
	vertices.reserve(vertices.size() + mesh.getVertices().size());	
	for (size_t i = 0; i < verts2D.size(); i++) {
		normals.push_back(vec3(0.0f, 1.0f, 0.0f));
		vertices.push_back(vec3(verts2D[i], height));
	}
	
	addTextureCoords(
		mesh.getTextureCoords().begin(),
		mesh.getTextureCoords().end());
	addColors(
		mesh.getColors().begin(),
		mesh.getColors().end());

	addVerticeIndices(mesh.getV_indices().begin(), mesh.getV_indices().end());
	addTextureIndices(mesh.getVt_indices().begin(), mesh.getVt_indices().end());
	addColorIndices(mesh.getVc_indices().begin(), mesh.getVc_indices().end());
}

void MeshBuilder::scale(float scale) {
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i] *= scale;
	}
}
void MeshBuilder::unitize(float unitScale) {
	scale(unitScale / maxExtent());
}

Indice::Indice(int pv, int pt, int pn) {
	this->v = pv;
	this->n = pn;
	this->t = pt;
}

void vertex_cb(void *user_data, float x, float y, float z, float w) {
	MeshBuilder *mesh = reinterpret_cast<MeshBuilder *>(user_data);
	mesh->vertices.push_back(glm::vec3(x, y, z));
}

void normal_cb(void *user_data, float x, float y, float z) {
	MeshBuilder *mesh = reinterpret_cast<MeshBuilder *>(user_data);
	mesh->normals.push_back(glm::vec3(x, y, z));
}

void texcoord_cb(void *user_data, float x, float y, float z) {
	MeshBuilder *mesh = reinterpret_cast<MeshBuilder *>(user_data);
	mesh->texcoords.push_back(glm::vec2(x, y));
}

/*
void index_cb(void *user_data, tinyobj::index_t *indices, int num_indices) {
	// NOTE: the value of each index is raw value.
	// For example, the application must manually adjust the index with offset
	// (e.g. v_indices.size()) when the value is negative(whic means relative
	// index).
	// Also, the first index starts with 1, not 0.
	// See fixIndex() function in tiny_obj_loader.h for details.
	// Also, 0 is set for the index value which
	// does not exist in .obj
	MeshBuilder *mesh = reinterpret_cast<MeshBuilder *>(user_data);
  	for (int i = 0; i < num_indices; i++) {
		tinyobj::index_t idx = indices[i];
		if (idx.vertex_index != 0) {
		  mesh->v_indices.push_back(idx.vertex_index-1);
		}
		if (idx.normal_index != 0) {
		  mesh->vn_indices.push_back(idx.normal_index-1);
		}
		if (idx.texcoord_index != 0) {
		  mesh->vt_indices.push_back(idx.texcoord_index-1);
		}
	}
}

void usemtl_cb(void *user_data, const char *name, int material_idx) {
  	MeshBuilder *mesh = reinterpret_cast<MeshBuilder*>(user_data);
  	if ((material_idx > -1) && (
		  static_cast<size_t>(material_idx) < mesh->materials.size())) {
  		spdlog::info("usemtl. material id = {}(name = {})", material_idx,
			mesh->materials[static_cast<size_t>(material_idx)].name.c_str());
  	} else {
		spdlog::info("usemtl. name = %s\n", name);
  	}
}

void mtllib_cb(
	void *user_data,
	const tinyobj::material_t *materials, 
	int num_materials)
{
	MeshBuilder *mesh = reinterpret_cast<MeshBuilder *>(user_data);
	spdlog::info("mtllib. # of materials = {}", num_materials);

	for (size_t i = 0; i < static_cast<size_t>(num_materials); i++) {
	  mesh->materials.push_back(materials[i]);
	}
}

void group_cb(void *user_data, const char **names, int num_names) {
	(void)user_data;
	// MyMesh *mesh = reinterpret_cast<MyMesh*>(user_data);
	spdlog::info("group : name = ");
	for (int i = 0; i < num_names; i++) {
	  spdlog::info("  {}\n", names[i]);
	}
}

void object_cb(void *user_data, const char *name) {
	(void)user_data;
	// MyMesh *mesh = reinterpret_cast<MyMesh*>(user_data);
	spdlog::info("object : name = {}", name);
}
*/

void MeshBuilder::setVertices(const std::vector<glm::vec3> &pVertices) { this->vertices = pVertices; }
void MeshBuilder::setNormals(const std::vector<glm::vec3> &pNormals) { this->normals = pNormals; }
void MeshBuilder::setTexCoords(const std::vector<glm::vec2> &pTexCoords) { this->texcoords = pTexCoords; }
void MeshBuilder::setColors(const std::vector<glm::vec3> &colors) { this->colors = colors; }

MeshBuilder& MeshBuilder::addCube(vec3 center, float dimension) noexcept {
	return addCube(center, vec3(dimension));
}
MeshBuilder& MeshBuilder::addCube(vec3 c, vec3 d) noexcept {
	vec3 newVertices[36] = {
		c + vec3(-d.x, -d.y, -d.z), c + vec3(-d.x, -d.y,  d.z), c + vec3(-d.x,  d.y,  d.z),
		c + vec3( d.x,  d.y, -d.z), c + vec3(-d.x, -d.y, -d.z), c + vec3(-d.x,  d.y, -d.z),
		c + vec3( d.x, -d.y,  d.z), c + vec3(-d.x, -d.y, -d.z), c + vec3( d.x, -d.y, -d.z),
		c + vec3( d.x,  d.y, -d.z), c + vec3( d.x, -d.y, -d.z), c + vec3(-d.x, -d.y, -d.z),
		c + vec3(-d.x, -d.y, -d.z), c + vec3(-d.x,  d.y,  d.z), c + vec3(-d.x,  d.y, -d.z),
		c + vec3( d.x, -d.y,  d.z), c + vec3(-d.x, -d.y,  d.z), c + vec3(-d.x, -d.y, -d.z), 
		c + vec3(-d.x,  d.y,  d.z), c + vec3(-d.x, -d.y,  d.z), c + vec3( d.x, -d.y,  d.z),
		c + vec3( d.x,  d.y,  d.z), c + vec3( d.x, -d.y, -d.z), c + vec3( d.x,  d.y, -d.z),
		c + vec3( d.x, -d.y, -d.z), c + vec3( d.x,  d.y,  d.z), c + vec3( d.x, -d.y,  d.z),
		c + vec3( d.x,  d.y,  d.z), c + vec3( d.x,  d.y, -d.z), c + vec3(-d.x,  d.y, -d.z),
		c + vec3( d.x,  d.y,  d.z), c + vec3(-d.x,  d.y, -d.z), c + vec3(-d.x,  d.y,  d.z),
		c + vec3( d.x,  d.y,  d.z), c + vec3(-d.x,  d.y,  d.z), c + vec3( d.x, -d.y,  d.z),
	};
	vec2 newTextureCoords[36] = {
		{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
		{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
		{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
		{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
		{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
		{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
	};
	addVertices(std::begin(newVertices), std::end(newVertices));
	addTextureCoords(std::begin(newTextureCoords), std::end(newTextureCoords));
	generateNormals(false);
	return *this;
}

void MeshBuilder::setVIndices(const std::vector<int> &pIndices) { this->v_indices = pIndices; }
void MeshBuilder::setVNIndices(const std::vector<int> &pIndices) { this->vn_indices = pIndices; }
void MeshBuilder::setVTIndices(const std::vector<int> &pIndices) { this->vt_indices = pIndices; }
void MeshBuilder::setVCIndices(const std::vector<int> &indices) { this->vc_indices = indices; }

void MeshBuilder::generateNormals(bool indexed, bool side) noexcept
{
	normals.clear();
	vn_indices.clear();
	if (indexed) {
		normals.reserve(v_indices.size());
		for (size_t i = 0; i + 3 <= v_indices.size();) {
			const vec3 ab = vertices[v_indices[i + 2]] - vertices[v_indices[i + 1]];
	        const vec3 ac = vertices[v_indices[i + 0]] - vertices[v_indices[i + 1]];
			vec3 normal = glm::normalize(glm::cross(ab, ac));
			normals.push_back(normal);
			vn_indices.push_back(static_cast<IndexType>(i));
			vn_indices.push_back(static_cast<IndexType>(i));
			vn_indices.push_back(static_cast<IndexType>(i));
		}
	} else {
		normals.clear();
		normals.reserve(vertices.size());
		for (size_t i = 0; i + 3 <= vertices.size(); i += 3) {
			const vec3 ab = vertices[i + 2] - vertices[i + 1];
	        const vec3 ac = vertices[i + 0] - vertices[i + 1];
			vec3 normal = glm::normalize(glm::cross(ab, ac));

			normals.push_back(normal);
			normals.push_back(normal);
			normals.push_back(normal);
			vn_indices.push_back(i);
			vn_indices.push_back(i + 1);
			vn_indices.push_back(i + 2);
		}
		// generates the default indices
		vn_indices = generateIndices(normals.size());
	}
}

void MeshBuilder::generateDefaultIndices()
{
	v_indices = generateIndices(vertices.size());
	vn_indices = generateIndices(normals.size());
	vt_indices = generateIndices(texcoords.size());
	vc_indices = generateIndices(colors.size());
}

const std::vector<glm::vec3>& MeshBuilder::getVertices() const { return vertices; }
const std::vector<glm::vec3>& MeshBuilder::getNormals() const { return normals; }
const std::vector<glm::vec2>& MeshBuilder::getTextureCoords() const { return texcoords; }
const std::vector<glm::vec3>& MeshBuilder::getColors() const { return colors; }
const std::vector<int>& MeshBuilder::getVIndices() const { return v_indices; }
const std::vector<int>& MeshBuilder::getVNIndices() const { return vn_indices; }
const std::vector<int>& MeshBuilder::getVTIndices() const { return vt_indices; }
const std::vector<int>& MeshBuilder::getVCIndices() const { return vc_indices; }

/*
MeshBuilder MeshBuilder::fromOBJ(const std::string &filename, const std::string &) {
	tinyobj::callback_t cb;
	cb.vertex_cb = vertex_cb;
	cb.normal_cb = normal_cb;
	cb.texcoord_cb = texcoord_cb;
	cb.index_cb = index_cb;
	cb.usemtl_cb = usemtl_cb;
	cb.mtllib_cb = mtllib_cb;
	cb.group_cb = group_cb;
	cb.object_cb = object_cb;

	MeshBuilder mesh;
	std::string warn;
	std::string err;
	std::ifstream ifs(filename.c_str());

	if (ifs.fail())
		throw std::runtime_error("File not found '" + filename + "'");

	tinyobj::MaterialFileReader mtlReader("../../models/");
	bool ret = tinyobj::LoadObjWithCallback(ifs, cb, &mesh, &mtlReader, &warn, &err);

	if (!warn.empty())
		std::cout << "WARN: " << warn << std::endl;
	if (!err.empty())
		std::cerr << err << std::endl;

	if (!ret)
		throw std::runtime_error("Failed to parse .obj");

	spdlog::info("# of vertices         = {}", mesh.vertices.size() / 3);
	spdlog::info("# of normals          = {}", mesh.normals.size() / 3);
	spdlog::info("# of texcoords        = {}", mesh.texcoords.size() / 2);
	spdlog::info("# of vertex indices   = {}", mesh.v_indices.size());
	spdlog::info("# of normal indices   = {}", mesh.vn_indices.size());
	spdlog::info("# of texcoord indices = {}", mesh.vt_indices.size());
	spdlog::info("# of materials        = {}", mesh.materials.size());

	return mesh;
}
*/

MeshBuilder nyrem::loadCube() {
	MeshBuilder mesh;
	float x = 1.0f, y = 1.0f, z = 1.0f;
	mesh.vertices = {
		{-1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f,-1.0f}, {-1.0f,-1.0f,-1.0f}, {-1.0f, 1.0f,-1.0f},
		{ 1.0f,-1.0f, 1.0f}, {-1.0f,-1.0f,-1.0f}, { 1.0f,-1.0f,-1.0f},
		{ 1.0f, 1.0f,-1.0f}, { 1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f,-1.0f},
		{-1.0f,-1.0f,-1.0f}, {-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f,-1.0f},
		{ 1.0f,-1.0f, 1.0f}, {-1.0f,-1.0f, 1.0f}, {-1.0f,-1.0f,-1.0f}, 
		{-1.0f, 1.0f, 1.0f}, {-1.0f,-1.0f, 1.0f}, { 1.0f,-1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f}, { 1.0f,-1.0f,-1.0f}, { 1.0f, 1.0f,-1.0f},
		{ 1.0f,-1.0f,-1.0f}, { 1.0f, 1.0f, 1.0f}, { 1.0f,-1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f,-1.0f}, {-1.0f, 1.0f,-1.0f},
		{ 1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f,-1.0f}, {-1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, { 1.0f,-1.0f, 1.0f},
	};
	mesh.normals.resize(mesh.vertices.size());
	mesh.texcoords.resize(mesh.vertices.size());
	return mesh;
}

MeshBuilder nyrem::loadTriangle() {
	MeshBuilder mesh;
	mesh.vertices = { { -1.0f, -1.0f, 0.0f }, { 1.0f, -1.0f, 0.0f }, { 0.0f,  1.0f, 0.0f } };
	mesh.normals = { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };
	mesh.texcoords = {{0.0, 0.0}, {1.0, 0.0}, {0.5, 1.0}};
	mesh.v_indices = { 0, 1, 2 };
	mesh.vn_indices = { 0, 1, 2 };
	mesh.vt_indices = { 0, 1, 2 };
	return mesh;
}

MeshBuilder2D nyrem::loadTriangle2D() {
	MeshBuilder2D mesh;
	mesh.vertices = {{-1.0f, -1.0f}, {1.0f, -1.0f}, {0.0f, 1.0f}};
	mesh.texCoords = {{0.0, 0.0}, {1.0f, 0.0f}, {0.5, 1.0}};
	mesh.v_indices = { 0, 1, 2 };
	mesh.vt_indices = { 0, 1, 2 };
	return mesh;
}

MeshBuilder2D nyrem::loadRect2D() {
	MeshBuilder2D mesh;
	mesh.vertices = {
		{-1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 1.0f},
		{-1.0f, -1.0f}, {1.0f, -1.0f}, {1.0f, 1.0f}
	};
	mesh.texCoords = {
		{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}
	};
	mesh.generateDefaultIndices();
	return mesh;
}

MeshBuilder2D::Exporter2D::Exporter2D(const MeshBuilder2D* builder)
	: builder(builder) { }

bool MeshBuilder2D::Exporter2D::resolveTypes(std::vector<float> &data, size_t i) const
{
	for (ExportType type : exp) {
		switch (type) {
			case EXPORT_VERTEX:
				if (i >= builder->vertices.size()) return false;
				addVec2(data, builder->vertices[i]); break;
			case EXPORT_VERTEX_INDEXED:
				if (i >= builder->v_indices.size()) return false;
				addVec2(data, builder->vertices[builder->v_indices[i]]); break;
			case EXPORT_TEXTURE:
				if (i >= builder->texCoords.size()) return false;
				addVec2(data, builder->texCoords[i]); break;
			case EXPORT_TEXTURE_INDEXED:
				if (i >= builder->vt_indices.size()) return false;
				addVec2(data, builder->texCoords[builder->vt_indices[i]]); break;
			case EXPORT_COLOR:
				if (i >= builder->colors.size()) return false;
				addVec3(data, builder->colors[i]); break;
			case EXPORT_COLOR_INDEXED:
				if (i >= builder->vc_indices.size()) return false;
				addVec3(data, builder->colors[builder->vc_indices[i]]); break;
			default: break;
		}
	}
	return true;
}


DataBlob::DataBlob() : k_data(nullptr), k_size(0) { }
DataBlob::DataBlob(void *data, size_t size)
	: k_data(data), k_size(size) { }

DataBlob::~DataBlob() { delete[] (char*) k_data; }
DataBlob::DataBlob(DataBlob &&blob)
	: k_data(std::exchange(blob.k_data, nullptr))
	, k_size(std::exchange(blob.k_size, 0)) { }

DataBlob& DataBlob::operator=(DataBlob &&blob)
{
	k_data = std::exchange(blob.k_data, nullptr);
	k_size = std::exchange(blob.k_size, 0);
	return *this;
}
