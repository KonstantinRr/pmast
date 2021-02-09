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

#ifndef CORE_HPP
#define CORE_HPP

#include <pmast/internal.hpp>

#include <string>
#include <glm/glm.hpp>

namespace traffic {

using coord_t = double;

// Forward declarations of types declared in this file //
class Point;	// basic point defined by latitude and longitude
class Distance; // distance between two points defined by d-latitude and d-longitude
class Circle;	// circle definition using a center point and a radius
class Rect;		// rectangle defined using a latitude and longitude

/// Represents a point using the latitude and longitude
/// format. Points can be created using coordinates or
/// by using a distance.
class Point {
public:
	using ThisType = Point;
	using PointType = ThisType;

protected:
	coord_t lat, lon;

public:
	/// (2) Creates a point using the given distance
	/// (3) Creates a point using the given latitude and longitude
	
	/// <summary>
	/// Creates a point at the coordinates (0.0, 0.0).
	/// This is 600km south of the coast of Ghana :)
	/// </summary>
	explicit Point();

	/// <summary>
	/// Creates a Point at the given coordinates.
	/// </summary>
	/// <param name="lat">The latitude coordinate</param>
	/// <param name="lat">The longitude coordinate</param>
	explicit Point(coord_t lat, coord_t lon);

	/// <summary>
	/// Creates a point from a vector of two floating point numbers.
	/// The vector stores the point in the order x: lattitude, y: longitude.
	/// </summary>
	Point(glm::vec2 vec);

	/// <summary>
	/// Creates a point from a vector of two double floating point numbers.
	/// The vector stores the point in the order x: lattitude, y: longitude.
	/// </summary>
	Point(glm::dvec2 vec);

	/// <summary>
	/// Returns the latitude of this point.
	/// </summary>
	coord_t getLatitude() const;

	/// <summary>
	/// Returns the longitude of thuis point.
	/// </summary>
	coord_t getLongitude() const;

	/// <summary>
	/// Adds a distance from this point and returns the new point.
	/// </summary>
	Point operator+(const Distance& point) const;
	
	/// <summary>
	/// Subtracts a distance from this point and returns the new point.
	/// </summary>
	Point operator-(const Distance& point) const;

	/// <summary>
	/// Returns the distance to the given point.
	/// </summary>
	Distance distanceTo(const Point& point) const;

	/// <summary>
	/// Returns the vector representation of this point.
	/// </summary<
	glm::vec2 toVec() const;

	/// <summary>
	/// Returns the double vector representation of this point.
	/// </summary>
	glm::dvec2 toDoubleVec() const;
};

/// The distance class describes a distance using
/// the latitude and longitude coordinates.
class Distance {
public:
	using ThisType = Distance;
	using DistanceType = ThisType;

protected:
	coord_t latLength, lonLength;

public:
	/// (1) Creates a distance that spans (0.0, 0.0)
	/// (2) Creates a distance using the point parameters
	/// (3) Creates a distance between to points
	/// (4) Creates a distance using the latitude and longitude difference
	explicit Distance();
	explicit Distance(Point point);
	explicit Distance(Point p1, Point p2);
	explicit Distance(coord_t latLength, coord_t lonLength);

	/// (1) Returns the difference in latitude
	/// (2) Returns the difference in longitude
	coord_t getLatLength() const;
	coord_t getLonLength() const;
	Distance& performScaleLat(coord_t scale);
	Distance& performScaleLon(coord_t scale);
	Distance& performScale(coord_t latScale, coord_t lonScale);
	Distance& performScale(coord_t scale);

	Distance scaleLat(coord_t scale) const;
	Distance scaleLon(coord_t scale) const;
	Distance scale(coord_t latScale, coord_t lonScale) const;
	Distance scale(coord_t scale) const;

	coord_t getLength() const;
	coord_t getLengthSquared() const;
	Distance operator+(const Distance& dist) const;
	Distance operator-(const Distance& dist) const;
	Distance operator*(coord_t scale) const;
};

class Rect
{
protected:
	Point center;
	coord_t latLength, lonLength;

public:
	/// Creates that most tightly encloses the given values
	/// coord_t lowerLat: Lower latitude boundary
	/// coord_t upperLat: Upper latitude boundary
	/// coord_t lowerLon: Lower longitude boundary
	/// coord_t upperLon: Upper longitude boundary
	static Rect fromBorders(coord_t lowerLat, coord_t upperLat, coord_t lowerLon, coord_t upperLon);
	static Rect fromLength(coord_t lowerLat, coord_t lowerLon, coord_t latLength, coord_t lonLength);
	
	/// Creates a rect from a center point and the distances from
	/// the center to the sides.
	/// Point center: Center point of this rect
	/// coord_t latLength: Distance from the center to the latitude border
	/// coord_t lonLength: Distance from the center to the longitude border
	static Rect fromCenter(Point center, coord_t latLength, coord_t lonLength);
	/// Creates a rect from a center point and the distances from
	/// the center to the sides. This function has the same logic as
	/// fromCenter(Point, coord_t, coord_t)
	static Rect fromCenter(coord_t centerLat, coord_t centerLon, coord_t latLength, coord_t lonLength);
	/// Creates a rect that most tightly encloses a circle.
	static Rect fromCircle(const Circle& circle);


	/// (1) Creates a rect that is equal to fromBorders(0, 0, 0, 0)
	explicit Rect();
	explicit Rect(Point center, coord_t latLength, coord_t lonLength);
	coord_t getLatLength() const;
	coord_t getLonLength() const;
	Point latHlonH() const;
	Point latHlonL() const;
	Point latLlonH() const;
	Point latLlonL() const;

	inline Point latHCenter() const { return center + Distance(latLength, 0); }
	inline Point latLCenter() const { return center + Distance(-latLength, 0); }
	inline Point lonHCenter() const { return center + Distance(0, lonLength); }
	inline Point lonLCenter() const { return center + Distance(0, -lonLength); }

	inline coord_t lowerLatBorder() const { return center.getLatitude() - latLength; }
	inline coord_t upperLatBorder() const { return center.getLatitude() + latLength; }
	inline coord_t lowerLonBorder() const { return center.getLongitude() - lonLength; }
	inline coord_t upperLonBorder() const { return center.getLongitude() + lonLength; }
	inline coord_t latCenter() const { return center.getLatitude(); }
	inline coord_t lonCenter() const { return center.getLongitude(); }
	inline coord_t latDistance() const { return 2 * latLength; }
	inline coord_t lonDistance() const { return 2 * lonLength;}

	std::string summary() const;

	Rect& performScaleLat(coord_t scale);
	Rect& performScaleLon(coord_t scale);
	Rect& performScale(coord_t latScale, coord_t lonScale);
	Rect& performScale(coord_t scale);

	Rect scaleLat(coord_t scale) const;
	Rect scaleLon(coord_t scale) const;
	Rect scale(coord_t latScale, coord_t lonScale) const;
	Rect scale(coord_t scale) const;

	bool containsLon(Point p) const;
	bool containsLat(Point p) const;
	bool contains(Point p) const;
	inline Point getCenter() const { return center; }
};

class Circle
{
protected:
	Point center;
	coord_t radiusLat, radiusLon;

public:
	explicit Circle();
	explicit Circle(const Point& center, coord_t radius);
	explicit Circle(const Point& center, coord_t radiusLat, coord_t radiusLon);

	Circle& performLatScale(coord_t scale);
	Circle& performLonScale(coord_t scale);
	Circle& performScale(coord_t latScale, coord_t lonScale);
	Circle& performScale(coord_t scale);

	Circle scaleLat(coord_t scale) const;
	Circle scaleLon(coord_t scale) const;
	Circle scale(coord_t latScale, coord_t lonScale) const;
	Circle scale(coord_t scale) const;

	bool contains(const Point& p) const;
	Point getCenter() const;
	coord_t getLatRadius() const;
	coord_t getLonRadius() const;
};

} // namespace traffic

#endif
