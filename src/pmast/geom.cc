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

#include <pmast/geom.hpp>

#include <math.h>
#include <algorithm>
#include <fmt/core.h>

using namespace std;
using namespace traffic;

Point::Point() : lat(0.0), lon(0.0) { }
Point::Point(coord_t _lat, coord_t _lon)
	: lat(_lat), lon(_lon) { }

Point::Point(glm::vec2 vec)
	: lat(vec.x), lon(vec.y) { }

Point::Point(glm::dvec2 vec)
	: lat(vec.x), lon(vec.y) { }

coord_t Point::getLatitude() const { return lat; }
coord_t Point::getLongitude() const { return lon; }

Point Point::operator+(const Distance& point) const
{ return Point(lat + point.getLatLength(), lon + point.getLonLength()); }
Point Point::operator-(const Distance& point) const
{ return Point(lat - point.getLatLength(), lon - point.getLonLength()); }

Distance Point::distanceTo(const Point& point) const
{ return Distance(lat - point.getLatitude(), lon - point.getLongitude()); }

// TODO fix
glm::vec2 Point::toVec() const { return glm::vec2(lon, lat); }
// TODO fix
glm::dvec2 Point::toDoubleVec() const { return glm::dvec2(lon, lat); }

// ---- Rect ---- //

Rect Rect::fromBorders(coord_t lowerLat, coord_t upperLat, coord_t lowerLon, coord_t upperLon)
{
	coord_t diffLat2 = (upperLat - lowerLat) / 2;
	coord_t diffLon2 = (upperLon - lowerLon) / 2;
	return Rect(
		Point(lowerLat + diffLat2, lowerLon + diffLon2),
		diffLat2, diffLon2
	);
}

Rect Rect::fromLength(coord_t lowerLat, coord_t lowerLon, coord_t latLength, coord_t lonLength)
{
	return Rect(
		Point(lowerLat + latLength * 0.5, lowerLon + lonLength * 0.5),
		latLength * 0.5, lonLength * 0.5
	);
}

Rect Rect::fromCenter(Point center, coord_t latLength, coord_t lonLength)
{ return Rect(center, latLength, lonLength); }

Rect Rect::fromCenter(coord_t centerLat, coord_t centerLon, coord_t latLength, coord_t lonLength)
{ return Rect(Point(centerLat, centerLon), latLength, lonLength); }

Rect::Rect()
{
	latLength = 0.0;
	lonLength = 0.0;
}

Rect Rect::fromCircle(const Circle& circle)
{
	return Rect(circle.getCenter(),
		circle.getLatRadius(),
		circle.getLonRadius());
}

Rect::Rect(Point center, coord_t latLength, coord_t lonLength)
{
	this->center = center;
	this->latLength = latLength;
	this->lonLength = lonLength;
}

coord_t Rect::getLatLength() const { return latLength; }
coord_t Rect::getLonLength() const { return lonLength; }
Point Rect::latHlonH() const { return center + Distance(latLength, lonLength); }
Point Rect::latHlonL() const { return center + Distance(latLength, -lonLength); }
Point Rect::latLlonH() const { return center + Distance(-latLength, lonLength); }
Point Rect::latLlonL() const { return center + Distance(-latLength, -lonLength); }

std::string Rect::summary() const
{
	return fmt::format("Rect {} {} {} {}",
		lowerLonBorder(), upperLonBorder(),
		lowerLatBorder(), upperLatBorder());
}

Rect& Rect::performScaleLat(coord_t scale) {
	latLength *= scale;
	return *this;
}

Rect& Rect::performScaleLon(coord_t scale) {
	lonLength *= scale;
	return *this;
}

Rect& Rect::performScale(coord_t latScale, coord_t lonScale) {
	latLength *= latScale;
	lonLength *= lonScale;
	return *this;
}

Rect& Rect::performScale(coord_t scale) {
	latLength *= scale;
	lonLength *= scale;
	return *this;
}

Rect Rect::scaleLat(coord_t scale) const
{ return Rect(center, latLength * scale, lonLength); }
Rect Rect::scaleLon(coord_t scale) const
{ return Rect(center, latLength, lonLength * scale); }
Rect Rect::scale(coord_t latScale, coord_t lonScale) const
{ return Rect(center, latLength * latScale, lonLength * lonScale); }
Rect Rect::scale(coord_t scale) const
{ return Rect(center, latLength * scale, lonLength * scale); }

bool Rect::containsLon(Point p) const {
	return p.getLongitude() >= lowerLonBorder() &&
		p.getLongitude() <= upperLonBorder();
}

bool Rect::containsLat(Point p) const {
	return p.getLatitude() >= lowerLatBorder() &&
		p.getLatitude() <= upperLatBorder();
}

bool Rect::contains(Point nd) const {
	return containsLon(nd) && containsLat(nd);
}

// ---- Distance ---- //

Distance::Distance()
	: latLength(0.0), lonLength(0.0) { }
Distance::Distance(Point point) 
	: latLength(point.getLatitude()), lonLength(point.getLongitude()) { }
Distance::Distance(Point p1, Point p2) :
	latLength(p1.getLatitude() - p2.getLatitude()),
	lonLength(p1.getLongitude() - p2.getLongitude()) { }
Distance::Distance(coord_t _latLength, coord_t _lonLength)
	: latLength(_latLength), lonLength(_lonLength) { }

coord_t Distance::getLatLength() const { return latLength; }
coord_t Distance::getLonLength() const { return lonLength; }
Distance& Distance::performScaleLat(coord_t scale) {
	latLength *= scale;
	return *this;
}

Distance& Distance::performScaleLon(coord_t scale) {
	lonLength *= scale;
	return *this;
}

Distance& Distance::performScale(coord_t latScale, coord_t lonScale) {
	latLength *= latScale;
	lonLength *= lonScale;
	return *this;
}

Distance& Distance::performScale(coord_t scale) {
	latLength *= scale;
	lonLength *= scale;
	return *this;
}

Distance Distance::scaleLat(coord_t scale) const { return Distance(latLength * scale, lonLength); }
Distance Distance::scaleLon(coord_t scale) const { return Distance(latLength, lonLength * scale); }
Distance Distance::scale(coord_t latScale, coord_t lonScale) const { return Distance(latLength * latScale, lonLength * lonScale); }
Distance Distance::scale(coord_t scale) const { return Distance(latLength * scale, lonLength * scale); }

coord_t Distance::getLength() const {
	return (coord_t)(sqrt(
		(double)latLength * (double)latLength +
		(double)lonLength * (double)lonLength)
	);
}
coord_t Distance::getLengthSquared() const { return latLength * latLength + lonLength * lonLength; }

Distance Distance::operator+(const Distance& dist) const {
	return Distance(latLength + dist.getLatLength(), lonLength + dist.getLonLength());
}

Distance Distance::operator-(const Distance& dist) const {
	return Distance(latLength - dist.getLatLength(), lonLength - dist.getLonLength());
}

Distance Distance::operator*(coord_t _scale) const {
	return scale(_scale);
}

Circle::Circle(const Point& center, coord_t radius)
{
	this->center = center;
	this->radiusLat = radius;
	this->radiusLon = radius;
}

Circle::Circle(const Point& center, coord_t radiusLat, coord_t radiusLon)
{
	this->center = center;
	this->radiusLat = radiusLat;
	this->radiusLon = radiusLon;
}

Circle& Circle::performLatScale(coord_t scale) {
	radiusLat *= scale;
	return *this;
}

Circle& Circle::performLonScale(coord_t scale) {
	radiusLon *= scale;
	return *this;
}

Circle& Circle::performScale(coord_t latScale, coord_t lonScale) {
	radiusLat *= latScale;
	radiusLon *= lonScale;
	return *this;
}

Circle& Circle::performScale(coord_t scale) {
	radiusLat *= scale;
	radiusLon *= scale;
	return *this;
}

Circle Circle::scaleLat(coord_t scale) const { return Circle(center, radiusLat * scale, radiusLon); }
Circle Circle::scaleLon(coord_t scale) const { return Circle(center, radiusLat, radiusLon * scale); }
Circle Circle::scale(coord_t latScale, coord_t lonScale) const { return Circle(center, radiusLat * latScale, radiusLon * lonScale); }
Circle Circle::scale(coord_t scale) const { return Circle(center, radiusLat * scale, radiusLon * scale); }

bool Circle::contains(const Point& p) const {
	Point dist = p - Distance(center);
	return (dist.getLatitude() * dist.getLatitude()) / radiusLat +
		(dist.getLongitude() * dist.getLongitude()) / radiusLon <= 1;
}

Point Circle::getCenter() const { return center; }
coord_t Circle::getLatRadius() const { return radiusLat; }
coord_t Circle::getLonRadius() const { return radiusLon; }

Circle::Circle()
{
	center = Point();
	radiusLat = 1.0;
	radiusLon = 1.0;
}

