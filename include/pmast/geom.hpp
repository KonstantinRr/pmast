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
	/// The vector stores the point in the order x: latitude, y: longitude.
	/// </summary>
	Point(glm::vec2 vec);

	/// <summary>
	/// Creates a point from a vector of two double floating point numbers.
	/// The vector stores the point in the order x: latitude, y: longitude.
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

class Rect {
public:
	using ThisType = Rect;
	using RectType = ThisType;

protected:
	/// <summary>
	/// The center of this circle
	/// </summary>
	Point center;

	/// <summary>
	/// The latitude and longitude length.
	/// </summary>
	coord_t latLength, lonLength;

public:
	/// <summary>
	/// Creates that most tightly encloses the given values.
	/// </summary>
	/// <param name="lowerLat">Lower latitude boundary</param>
	/// <param name="upperLat">Upper latitude boundary</param>
	/// <param name="lowerLon">Lower longitude boundary</param>
	/// <param name="upperLon">Upper longitude boundary</param>
	static Rect fromBorders(coord_t lowerLat, coord_t upperLat, coord_t lowerLon, coord_t upperLon);
	
	/// <summary>
	/// Creates a rect from two boundaries and the length in latitude 
	/// and longitude direction.
	/// </summary>
	/// <param name="lowerLat">Lower latitude boundary</param>
	/// <param name="lowerLon">Lower longitude boundary</param>
	/// <param name="latLength">Latitude length</param>
	/// <param name="lonLength">Longitude length</param>
	static Rect fromLength(coord_t lowerLat, coord_t lowerLon, coord_t latLength, coord_t lonLength);
	
	/// <summary>
	/// Creates a rect from a center point and the distances from
	/// the center to the sides.
	/// </summary>
	/// <param name="center">Lower longitude boundary</param>
	/// <param name="latLength">Distance from the center to the latitude border</param>
	/// <param name="lonLength">Distance from the center to the longitude border</param>
	static Rect fromCenter(Point center, coord_t latLength, coord_t lonLength);
	
	/// <summary>
	/// Creates a rect from a center point and the distances from
	/// the center to the sides. This function has the same logic as
	/// </summary>
	/// <param name="centerLat">Center latitude</param>
	/// <param name="centerLon">Center longitude</param> 
	static Rect fromCenter(coord_t centerLat, coord_t centerLon, coord_t latLength, coord_t lonLength);
	
	/// <summary>
	/// Creates a rect that most tightly encloses a circle.
	/// </summary>
	/// <param name="circle">The circle</param> 
	static Rect fromCircle(const Circle& circle);


	/// (1) Creates a rect that is equal to fromBorders(0, 0, 0, 0)
	explicit Rect();
	explicit Rect(Point center, coord_t latLength, coord_t lonLength);
	
	/// <summary>
	/// Returns the latitude length of this rect.
	/// </summary>
	coord_t getLatLength() const;
	/// <summary>
	/// Returns the longitude length of this rect.
	/// </summary>
	coord_t getLonLength() const;

	/// <summary>
	/// Returns upper latitude longitude coordinate.
	/// </summary>
	Point latHlonH() const;

	/// <summary>
	/// Returns the upper latitude and lower longitude coordinate.
	/// </summary>
	Point latHlonL() const;

	/// <summary>
	/// Returns the lower latitude and upper latitude coordinate.
	/// </summary>
	Point latLlonH() const;

	/// <summary>
	/// Returns the lower latitude longitude coordinate.
	/// </summary>
	Point latLlonL() const;

	/// <summary>
	/// Returns the upper latitude coordinate with the center longitude coordinate.
	/// </summary>
	inline Point latHCenter() const { return center + Distance(latLength, 0); }
	
	/// <summary>
	/// Returns the lower latitude coordinate with the center longitude coordinate.
	/// </summary>
	inline Point latLCenter() const { return center + Distance(-latLength, 0); }
	
	/// <summary>
	/// Returns the upper longitude coordiante with the center latitude coordinate.
	/// </summary>
	inline Point lonHCenter() const { return center + Distance(0, lonLength); }
	
	/// <summary>
	/// Returns the lower longitude coordinate with the center longitude coordinate.
	/// </summary>
	inline Point lonLCenter() const { return center + Distance(0, -lonLength); }


	/// <summary>
	/// Returns the lower latitude boundary of this rect.
	/// </summary>
	inline coord_t lowerLatBorder() const { return center.getLatitude() - latLength; }
	
	/// <summary>
	/// Returns the upper latitude boundary of this rect.
	/// </summary>
	inline coord_t upperLatBorder() const { return center.getLatitude() + latLength; }
	
	/// <summary>
	/// Returns the lower longitude boundary of this rect.
	/// </summary>
	inline coord_t lowerLonBorder() const { return center.getLongitude() - lonLength; }
	
	/// <summary>
	/// Returns the upper longitude boundary of this rect.
	/// </summary>
	inline coord_t upperLonBorder() const { return center.getLongitude() + lonLength; }
	
	/// <summary>
	/// Returns the center latitude coordinate.
	/// </summary>
	inline coord_t latCenter() const { return center.getLatitude(); }
	
	/// <summary>
	/// Returns the center longitude coordinate.
	/// </summary>
	inline coord_t lonCenter() const { return center.getLongitude(); }
	
	/// <summary>
	/// Returns the latitude dimensions of this rect.
	/// </summary> 
	inline coord_t latDistance() const { return 2 * latLength; }
	
	/// <summary>
	/// Returns the longitude dimensions of this rect.
	/// </summary>
	inline coord_t lonDistance() const { return 2 * lonLength;}

	std::string summary() const;

	/// <summary>
	/// Scales the rect with a latitude scale factor.
	/// </summary>
	/// <param name="scale">The latitude scale factor</param>
	/// <returns>A reference to *this</returns>
	Rect& performScaleLat(coord_t scale);

	/// <summary>
	/// Scales the rect with a longitude scale factor.
	/// </summary>
	/// <param name="scale">The latitude scale factor</param>
	/// <returns>A reference to *this</returns>
	Rect& performScaleLon(coord_t scale);

	/// <summary>
	/// Scales the rect with a latitude and longitude factor.
	/// </summary>
	/// <param name="latScale">The latitude scale factor</param>  
	/// <param name="lonScale">The longitude scale factor</param> 
	/// <returns>A reference to *this</returns>
	Rect& performScale(coord_t latScale, coord_t lonScale);

	/// <summary>
	/// Scales the the rect with a uniform scale factor.
	/// </summary>
	/// <param name="scale">The uniform scale factor</param> 
	/// <returns>A reference to *this</returns>
	Rect& performScale(coord_t scale);

	/// <summary>
	/// Returns a latitude scaled copy of this object.
	/// </summary>
	/// <param name="scale">The latitude scale factor</param> 
	/// <returns>The scaled copy</returns> 
	Rect scaleLat(coord_t scale) const;
	
	/// <summary>
	/// Returns a longitude scaled copy of this object.
	/// </summary>
	/// <param name="scale">The longitude scale factor</param> 
	/// <returns>The scaled copy</returns> 
	Rect scaleLon(coord_t scale) const;

	/// <summary>
	/// Returns a scaled copy of this object.
	/// </summary>
	/// <param name="latScale">The latitude scale factor</param> 
	/// <param name="lonScale">The longitude scale factor</param> 
	/// <returns>The scaled copy</returns> 
	Rect scale(coord_t latScale, coord_t lonScale) const;

	/// <summary>
	/// Returns a scaled copy of this object.
	/// </summary>
	/// <param name="scale">The scale factor</param>
	/// <returns>The scaled copy</returns> 
	Rect scale(coord_t scale) const;

	bool containsLon(Point p) const;
	bool containsLat(Point p) const;
	bool contains(Point p) const;
	inline Point getCenter() const { return center; }
};

class Circle {
public:
	using ThisType = Circle;
	using CircleType = ThisType;

protected:
	Point center;
	coord_t radiusLat, radiusLon;

public:
	/// <summary>
	/// Creates a new circle/elipse with with the center (0, 0) and a radius
	/// latitude and longitude.
	/// </summary>
	explicit Circle();

	/// <summary>
	/// Creates a new circle with the same latitude and longitude radius.
	/// </summary>
	/// <param name="center">The center of the circle</param>
	/// <param name="radius">The circle radius</param>
	explicit Circle(const Point& center, coord_t radius);
	
	/// <summary>
	/// Creates a new circle at a point with the given latitude and longitude radius. 
	/// </summary>
	/// <param name="center">The center of the circle</param>
	/// <param name="radiusLat">The latitude radius</param>
	/// <param name="radiusLon">The longitude radius</param>
	explicit Circle(const Point& center, coord_t radiusLat, coord_t radiusLon);


	/// <summary>
	/// Scales the circle with a latitude scale factor.
	/// </summary>
	/// <param name="scale">The latitude scale factor</param>
	/// <returns>A reference to *this</returns>
	Circle& performLatScale(coord_t scale);

	/// <summary>
	/// Scales the circle with a longitude scale factor.
	/// </summary>
	/// <param name="scale">The longitude scale factor</param> 
	/// <returns>A reference to *this</returns>
	Circle& performLonScale(coord_t scale);

	/// <summary>
	/// Scales the circle with a latitude and longitude factor.
	/// </summary>
	/// <param name="latScale">The latitude scale factor</param>  
	/// <param name="lonScale">The longitude scale factor</param> 
	/// <returns>A reference to *this</returns>
	Circle& performScale(coord_t latScale, coord_t lonScale);
	
	/// <summary>
	/// Scales the the circle with a uniform scale factor.
	/// </summary>
	/// <param name="scale">The uniform scale factor</param> 
	/// <returns>A reference to *this</returns>
	Circle& performScale(coord_t scale);


	/// <summary>
	/// Returns a latitude scaled copy of this object.
	/// </summary>
	/// <param name="scale">The latitude scale factor</param> 
	/// <returns>The scaled copy</returns> 
	Circle scaleLat(coord_t scale) const;
	
	/// <summary>
	/// Returns a longitude scaled copy of this object.
	/// </summary>
	/// <param name="scale">The longitude scale factor</param> 
	/// <returns>The scaled copy</returns> 
	Circle scaleLon(coord_t scale) const;
	
	/// <summary>
	/// Returns a scaled copy of this object.
	/// </summary>
	/// <param name="latScale">The latitude scale factor</param> 
	/// <param name="lonScale">The longitude scale factor</param> 
	/// <returns>The scaled copy</returns> 
	Circle scale(coord_t latScale, coord_t lonScale) const;

	/// <summary>
	/// Returns a scaled copy of this object.
	/// </summary>
	/// <param name="scale">The scale factor</param>
	/// <returns>The scaled copy</returns> 
	Circle scale(coord_t scale) const;

	/// <summary>
	/// Checks whether this object contains a given circle.
	/// </summary>
	/// <param name="p">The point that is checked</param> 
	/// <returns>True iff the point is in the object</returns>
	bool contains(const Point& p) const;

	/// <summary>
	/// Returns the center of this object.
	/// </summary>
	/// <returns>The center of this o</returns>
	Point getCenter() const;

	/// <summary>
	/// Returns the latitude radius.
	/// </summary>
	/// <returns>The latitude radius</returns>
	coord_t getLatRadius() const;

	/// <summary>
	/// Returns the longitude radius.
	/// </summary>
	/// <returns>The latitude radius</returns>
	coord_t getLonRadius() const;
};

} // namespace traffic

#endif
