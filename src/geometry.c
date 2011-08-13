#include <SDL.h>
#include <gl.h>
#include <glu.h>
#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"


struct geometry_TreeNode* geometry_Root = 0;

void geometry_addShape(int type, float * vertices)
{
	struct geometry_Shape *newShape;
	int numVertices;

	char errorText[BUFFER_SIZE];
	switch (type)
	{
		case GL_LINES :
			numVertices = 2;
			break;
		case GL_TRIANGLES :
			numVertices = 3;
			break;
		default :
			snprintf(errorText, BUFFER_SIZE, "Error: Unknown geometry definition %d", type);

			fputs(stderr, errorText);
	}
	newShape = (struct geometry_Shape *) malloc(sizeof(struct geometry_Shape));
	struct geometry_Vertex ** verticesArray;
	verticesArray = (struct geometry_Vertex **) malloc (GEOMETRY_BUCKET_SIZE * sizeof(struct geometry_Vertex *));

	int i;
	for (i = 0; i < numVertices; i++)
	{
		verticesArray[i] = (struct geometry_Vertex *) malloc (sizeof (struct geometry_Vertex));
		verticesArray[i]->shape = *newShape;
		verticesArray[i]->x = vertices[i * 2];
		verticesArray[i]->y = vertices[i * 2 + 1];

		geometry_addVertex(geometry_Root, verticesArray[i], newShape);
	}

	newShape->vertices = * verticesArray;
	newShape->type = type;
}

void geometry_addVertex(struct geometry_TreeNode* node, struct geometry_Vertex * vertex, struct geometry_Shape* shape)
{
	if (node->numVertices < GEOMETRY_BUCKET_SIZE)
	{
		node->vertices[node->numVertices] = *vertex;
		node->numVertices++;
	}
	else
	{
		// Split

		float midX = node->x1 + node->x2 / 2;
		float midY = node->y1 + node->y2 / 2;

		node->children[0] = geometry_TreeNode_Construct(node->x1, node->y1, midX, midY);
		node->children[1] = geometry_TreeNode_Construct(midX, node->y1, node->x2, midY);
		node->children[2] = geometry_TreeNode_Construct(node->x1, midY, midX, node->y2);
		node->children[3] = geometry_TreeNode_Construct(midX, midY, node->x2, node->y2);

		int target;
		if (vertex->x < midX)
		{
			if (vertex->y < midY)
			{
				target = 0;
			}
			else
			{
				target = 1;
			}
		}
		else
		{
			if (vertex->y < midY)
			{
				target = 2;
			}
			else
			{
				target = 3;
			}
		}
		geometry_addVertex(node->children[target], vertex, shape);
	}
}

void geometry_init(float x1, float y1, float x2, float y2)
{
	geometry_Root = geometry_TreeNode_Construct(x1, y1, x2, y2);
}

struct geometry_TreeNode * geometry_TreeNode_Construct(float x1, float y1, float x2, float y2)
{
	struct geometry_TreeNode * node;
	node = (struct geometry_TreeNode *) malloc(sizeof(struct geometry_TreeNode));
	node->children = (struct geometry_TreeNode **) malloc (TREE_ORDER * sizeof(struct geometry_TreeNode *));
	memset(node->children, 0, TREE_ORDER);
	node->x1 = x1;
	node->y1 = y1;
	node->x2 = x2;
	node->y2 = y2;

	return node;
}

void geometry_destroy(struct geometry_Shape * node)
{

}
