/*
 * header.h
 *
 *  Created on: Aug 18, 2011
 *      Author: bmurphy
 */

#ifndef HEADER_H_
#define HEADER_H_


#ifdef __APPLE__

#include <gl.h>
#include <glu.h>
#include <SDL.h>

#else

#include <SDL/SDL.h>
#include <gl/gl.h>
#include <gl/glu.h>


#endif

//#define __USE_QUADTREE__ 1
#define __USE_GEOMETRY__ 1

#include "quadtree.h"

#include "geometry_array.h"

#ifdef __USE_QUADTREE__

#define geometry_init(a, b, c, d) geometry_quadtree_init(a, b, c, d)
#define geometry_addLine(a) geometry_quadtree_addLine(a)
#define geometry_destroy() geometry_quadtree_destroy()
#define geometry_CheckCollision(a, b, c) geometry_quadtree_CheckCollision(a, b, c)
#define geometry_ResetLineIterator() geometry_quadtree_ResetLineIterator()
#define geometry_GetNextLine() geometry_quadtree_GetNextLine()

#endif
#ifdef __USE_GEOMETRY__

#define geometry_init(a, b, c, d) geometry_array_init(a, b, c, d)
#define geometry_addLine(a) geometry_array_addLine(a)
#define geometry_destroy() geometry_array_destroy()
#define geometry_CheckCollision(a, b, c) geometry_array_CheckCollision(a, b, c)
#define geometry_ResetLineIterator() geometry_array_ResetLineIterator()
#define geometry_GetNextLine() geometry_array_GetNextLine()

#endif
#endif /* HEADER_H_ */
