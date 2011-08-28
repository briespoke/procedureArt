/*
 * quadtree.c
 *
 *  Created on: Aug 27, 2011
 *      Author: wavenger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "geometry.h"
#include "util.h"
#include "constants.h"

struct geometry_TreeNode
{
	float x1, y1, x2, y2;
	int numLines;
	int id;
	struct geometry_LineSegment * lines[GEOMETRY_BUCKET_SIZE + 1];
	struct geometry_TreeNode ** children;
};


struct geometry_TreeNode * geometry_quadtree_TreeNode_construct(float x1, float y1, float x2, float y2);

void geometry_quadtree_addLineRecurse(struct geometry_TreeNode* node, struct geometry_LineSegment * newLine);

void geometry_quadtree_TreeNode_destroyRecurse(struct geometry_TreeNode * node);
void geometry_quadtree_CheckCollisionRecurse(struct geometry_LineSegment * line, struct geometry_TreeNode * node, float * resultX, float * resultY);

int geometry_quadtree_LineFitsInNode(struct geometry_LineSegment * line , struct geometry_TreeNode *childNode);

void geometry_quadtree_WalkTree();
void geometry_quadtree_WalkTreeRecurse(struct geometry_TreeNode * node);

int geometry_quadtree_BoxTest(struct geometry_LineSegment * line, struct geometry_TreeNode * node);

struct geometry_TreeNode * geometry_quadtree_GetRootNode();

void geometry_quadtree_ResetLineIteratorRecurse(struct geometry_TreeNode * currentNode);

//struct geometry_TreeNode *geometry_nodeStack[MAX_TREE_DEPTH];
//int geometry_NodeStackIndex = 0;
struct geometry_LineSegment ** geometry_AllLines = 0;
int geometry_AllLinesIndex;
int geometry_AllLinesTotal;

struct geometry_TreeNode *geometry_Root = 0;

int geometry_NodeCount = 0;

void geometry_quadtree_init(float x1, float y1, float x2, float y2)
{
	geometry_Root = geometry_quadtree_TreeNode_construct(x1, y1, x2, y2);
}

void geometry_quadtree_addLine(struct geometry_LineSegment * newLine)
{
	geometry_quadtree_addLineRecurse(geometry_Root, newLine);
}

void geometry_quadtree_addLineRecurse(struct geometry_TreeNode* node, struct geometry_LineSegment * newLine)
{
	if (node->numLines < GEOMETRY_BUCKET_SIZE)
	{
		node->lines[node->numLines] = newLine;
		node->numLines++;
	}
	else
	{
		int numLinesToTest = 0;
		struct geometry_LineSegment * linesToTest[GEOMETRY_BUCKET_SIZE + 1];

		int i;
		float midX = (node->x1 + node->x2) / 2;
		float midY = (node->y1 + node->y2) / 2;

		if (node->children == 0)
		{
			node->children = (struct geometry_TreeNode**) malloc (TREE_ORDER * sizeof (struct geometry_TreeNode*));

			node->children[0] = geometry_quadtree_TreeNode_construct(node->x1, node->y1, midX, midY);
			node->children[1] = geometry_quadtree_TreeNode_construct(midX, node->y1, node->x2, midY);
			node->children[2] = geometry_quadtree_TreeNode_construct(node->x1, midY, midX, node->y2);
			node->children[3] = geometry_quadtree_TreeNode_construct(midX, midY, node->x2, node->y2);

			while (numLinesToTest < GEOMETRY_BUCKET_SIZE)
			{
				linesToTest[numLinesToTest] = node->lines[numLinesToTest];
				numLinesToTest++;
			}
		}
		linesToTest[numLinesToTest] = newLine;
		numLinesToTest++;

		//Here we're going to chop the line segments up until they will all fit in exactly one child.
		node->lines[GEOMETRY_BUCKET_SIZE] = newLine;
		int numChoppedLines = 0;

		struct geometry_LineSegment * choppedLines[BUFFER_SIZE];

		struct geometry_LineSegment * test1 = geometry_LineSegment_constructDummy(midX, node->y1, midX, node->y2, 0);
		struct geometry_LineSegment * test2 = geometry_LineSegment_constructDummy(node->x1, midY, node->x2, midY, 0);

		for (i = 0; i < numLinesToTest && numChoppedLines < BUFFER_SIZE; i ++)
		{
			struct geometry_LineSegment * line = linesToTest[i];
			float resultX1, resultY1, resultX2, resultY2;

			int intersect1, intersect2;
			intersect1 = geometry_LineIntersect(line, test1, &resultX1, &resultY1);
			intersect2 = geometry_LineIntersect(line, test2, &resultX2, &resultY2);

			if (intersect1 && intersect2)
			{
				struct geometry_LineSegment * newLine1 = geometry_LineSegment_constructSplit(line->x1, line->y1, resultX1, resultY1, line);
				struct geometry_LineSegment * newLine2 = geometry_LineSegment_constructSplit(resultX1, resultY1, resultX2, resultY2, line);
				struct geometry_LineSegment * newLine3 = geometry_LineSegment_constructSplit(resultX2, resultY2, line->x2, line->y2, line);

				choppedLines[numChoppedLines] = newLine1;
				choppedLines[numChoppedLines + 1] = newLine2;
				choppedLines[numChoppedLines + 2] = newLine3;
				numChoppedLines += 3;

				free(line);
			}
			else if (intersect1)
			{

				struct geometry_LineSegment * newLine1 = geometry_LineSegment_constructSplit(line->x1, line->y1, resultX1, resultY1, line);
				struct geometry_LineSegment * newLine2 = geometry_LineSegment_constructSplit(resultX1, resultY1, line->x2, line->y2, line);

				choppedLines[numChoppedLines] = newLine1;
				choppedLines[numChoppedLines + 1] = newLine2;
				numChoppedLines += 2;

				free(line);
			}
			else if (intersect2)
			{

				struct geometry_LineSegment * newLine1 = geometry_LineSegment_constructSplit(line->x1, line->y1, resultX2, resultY2, line);
				struct geometry_LineSegment * newLine2 = geometry_LineSegment_constructSplit(resultX2, resultY2, line->x2, line->y2, line);

				choppedLines[numChoppedLines] = newLine1;
				choppedLines[numChoppedLines + 1] = newLine2;
				numChoppedLines += 2;

				free(line);
			}
			else
			{
				choppedLines[numChoppedLines] = line;
				numChoppedLines++;
			}

		}

		free(test1);
		free(test2);

		for (i = 0; i < numChoppedLines; i++)
		{
			struct geometry_LineSegment * line = choppedLines[i];

			int j;
			int found = 0;

			for (j = 0; j < TREE_ORDER && ! found; j++)
			{
				struct geometry_TreeNode * childNode = node->children[j];

				if (geometry_quadtree_LineFitsInNode(line, childNode))
				{
					found = 1;
					geometry_quadtree_addLineRecurse(childNode, line);
				}
			}
		}
	}
}

int geometry_quadtree_LineFitsInNode(struct geometry_LineSegment * line, struct geometry_TreeNode *childNode)
{
	if (line->x1 <= childNode->x1 &&
		line->x2 <= childNode->x1 &&
		line->y1 <= childNode->y1 &&
		line->y2 <= childNode->y1 &&
		line->x1 >= childNode->x2 &&
		line->x2 >= childNode->x2 &&
		line->y1 >= childNode->y2 &&
		line->y2 >= childNode->y2
		)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

struct geometry_TreeNode * geometry_quadtree_TreeNode_construct(float x1, float y1, float x2, float y2)
{
	struct geometry_TreeNode * node;
	node = (struct geometry_TreeNode *) malloc(sizeof(struct geometry_TreeNode));
	node->children = 0;
	node->numLines = 0;

	node->id = geometry_NodeCount++;
	//printf("%d\n", node->id);

	if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
	{
		puts("Infinite subdivision");
	}
	node->x1 = x1;
	node->y1 = y1;
	node->x2 = x2;
	node->y2 = y2;
	//printf("Node construct: %f, %f, %f, %f\n", x1, y1, x2, y2);
	return node;
}

void geometry_quadtree_destroy()
{
	geometry_quadtree_TreeNode_destroyRecurse(geometry_Root);
}

void geometry_quadtree_TreeNode_destroyRecurse(struct geometry_TreeNode * node)
{
	int i;
	if (node->children != 0)
	{
		for (i = 0; i < TREE_ORDER; i++)
		{
			geometry_quadtree_TreeNode_destroyRecurse(node->children[i]);
		}
	}
	else
	{
		for (i = 0; i < node->numLines; i++)
		{
			free(node->lines[i]);
		}
	}
	free(node);
}
struct geometry_TreeNode * geometry_quadtree_GetRootNode()
{
	return geometry_Root;
}


void geometry_quadtree_WalkTree()
{
	geometry_quadtree_WalkTreeRecurse(geometry_Root);
}

void geometry_quadtree_WalkTreeRecurse(struct geometry_TreeNode * node)
{
	int i;
	if (node->children == 0)
	{
		for (i = 0; i < node->numLines; i++)
		{
			printf("%f, %f : %f, %f\n", node->lines[i]->x1, node->lines[i]->y1, node->lines[i]->x2, node->lines[i]->y2);
		}
	}
	else
	{
		for (i = 0; i < TREE_ORDER; i++)
		{
			geometry_quadtree_WalkTreeRecurse(node->children[i]);
		}
	}

}

void geometry_quadtree_CheckCollision(struct geometry_LineSegment * line, float * resultX, float * resultY)
{
	geometry_quadtree_CheckCollisionRecurse(line, geometry_quadtree_GetRootNode(), resultX, resultY);
}

void geometry_quadtree_CheckCollisionRecurse(struct geometry_LineSegment * line, struct geometry_TreeNode * node, float * resultX, float * resultY)
{
	int i;
	if (geometry_quadtree_LineFitsInNode(line, node) || geometry_quadtree_BoxTest(line, node))
	{
		//printf("%d\n", node->id);
		if (node->children != 0)
		{
			for (i = 0; i < TREE_ORDER; i++)
			{
				geometry_quadtree_CheckCollisionRecurse(line, node->children[i], resultX, resultY);
			}
		}
		else
		{
			for (i = 0; i < node->numLines; i++)
			{
				float tmpX, tmpY;
				if(geometry_LineIntersect(line, node->lines[i], &tmpX, &tmpY))
				{
					if (fabs(tmpX - line->x1) < fabs((* resultX) - line->x1))
					{
						* resultX = tmpX;
						* resultY = tmpY;
					}
				}

			}
		}
		if (*resultY == -1.25)
		{
			puts("Moo!");
		}
	}
}
int geometry_quadtree_BoxTest(struct geometry_LineSegment * line, struct geometry_TreeNode * node)
{
	int ret = 0;
	struct geometry_LineSegment * test1 = geometry_LineSegment_constructDummy(node->x1, node->y1, node->x2, node->y1, 0);
	struct geometry_LineSegment * test2 = geometry_LineSegment_constructDummy(node->x1, node->y1, node->x1, node->y2, 0);
	struct geometry_LineSegment * test3 = geometry_LineSegment_constructDummy(node->x1, node->y2, node->x2, node->y2, 0);
	struct geometry_LineSegment * test4 = geometry_LineSegment_constructDummy(node->x2, node->y1, node->x2, node->y2, 0);

	float dummy;

	if (	geometry_LineIntersect(line, test1, &dummy, &dummy) ||
			geometry_LineIntersect(line, test2, &dummy, &dummy) ||
			geometry_LineIntersect(line, test3, &dummy, &dummy) ||
			geometry_LineIntersect(line, test4, &dummy, &dummy))
	{
		ret = 1;
	}

	free(test1);
	free(test2);
	free(test3);
	free(test4);

	return ret;
}

void geometry_quadtree_ResetLineIterator()
{
	if (geometry_AllLines != 0)
	{
		free(geometry_AllLines);
	}

	geometry_AllLines = (struct geometry_LineSegment **) malloc(sizeof(struct geometry_LineSegment *) * BIG_BUFFER_SIZE);

	geometry_AllLinesTotal = 0;
	geometry_quadtree_ResetLineIteratorRecurse(geometry_quadtree_GetRootNode());
	geometry_AllLinesIndex = -1;
}

void geometry_quadtree_ResetLineIteratorRecurse(struct geometry_TreeNode * currentNode)
{
	if (currentNode->children == 0)
	{
		int i;
		for (i = 0; i < currentNode->numLines; i++)
		{
			geometry_AllLines[geometry_AllLinesTotal] = currentNode->lines[i];
			geometry_AllLinesTotal++;
		}
	}
	else
	{
		int i;
		for (i = 0; i < TREE_ORDER; i++)
		{
			geometry_quadtree_ResetLineIteratorRecurse(currentNode->children[i]);
		}
	}
}

struct geometry_LineSegment * geometry_quadtree_GetNextLine()
{
	geometry_AllLinesIndex++;

	if (geometry_AllLinesIndex < geometry_AllLinesTotal)
	{
		return geometry_AllLines[geometry_AllLinesIndex];
	}
	else
	{
		return 0;
	}
}
