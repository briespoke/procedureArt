//#include <SDL.h>
#include <gl.h>
#include <glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "geometry.h"
#include <math.h>
#include "util.h"


struct geometry_TreeNode *geometry_Root = 0;
int geometry_NodeCount = 0;
int geometry_LineCount;

void geometry_init(float x1, float y1, float x2, float y2)
{
	geometry_Root = geometry_TreeNode_construct(x1, y1, x2, y2);
}

void geometry_addLine(struct geometry_LineSegment * newLine)
{
	geometry_addLineRecurse(geometry_Root, newLine);
}

void geometry_addLineRecurse(struct geometry_TreeNode* node, struct geometry_LineSegment * newLine)
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

			node->children[0] = geometry_TreeNode_construct(node->x1, node->y1, midX, midY);
			node->children[1] = geometry_TreeNode_construct(midX, node->y1, node->x2, midY);
			node->children[2] = geometry_TreeNode_construct(node->x1, midY, midX, node->y2);
			node->children[3] = geometry_TreeNode_construct(midX, midY, node->x2, node->y2);

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

		struct geometry_LineSegment * test1 = geometry_LineSegment_constructDummy(midX, node->y1, midX, node->y2);
		struct geometry_LineSegment * test2 = geometry_LineSegment_constructDummy(node->x1, midY, node->x2, midY);

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

				if (geometry_LineFitsInNode(line, childNode))
				{
					found = 1;
					geometry_addLineRecurse(childNode, line);
				}
			}
		}
	}
}

int geometry_LineFitsInNode(struct geometry_LineSegment * line, struct geometry_TreeNode *childNode)
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

int geometry_LineIntersect(struct geometry_LineSegment * line1, struct geometry_LineSegment * line2, float * resultX, float *resultY)
{
	//y = mx + c
	float m1, m2, c1, c2;
	int nonVertical1 = slope(line1, &m1);
	int nonVertical2 = slope(line2, &m2);

	if (nonVertical1 && nonVertical2)
	{
		if (m1 == m2)
		{
			// Lines are parallel, they never intersect
			return 0;
		}
		else
		{
			displacement(line1, &c1);
			displacement(line2, &c2);

			float xIntersect = (c2 - c1) / (m1 - m2);

			if (inRange(line1, xIntersect) && inRange(line2, xIntersect))
			{
				*resultX = xIntersect;
				*resultY = xIntersect * m1 + c1;
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		if (!nonVertical1 && !nonVertical2)
		{
			//Both lines are vertical, they never intersect
			return 0;
		}
		else if (! nonVertical1)
		{
			//line 1 is vertical
			if (inRange(line2, line1->x1))
			{
				displacement(line2, &c2);
				* resultX = line1->x1;
				* resultY = line1->x1 * m2 + c2;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			//line 2 is vertical
			if (inRange(line1, line2->x1))
			{
				displacement(line1, &c1);
				* resultX = line2->x1;
				* resultY = line2->x1 * m1 + c1;
				return 1;
			}
			else
			{
				return 0;
			}

		}
	}
}

int inRange(struct geometry_LineSegment * line, float test)
{
	return  (test > line->x1 && test < line->x2) ||
			(test > line->x2 && test < line->x1);
}

int slope(struct geometry_LineSegment * line, float * result)
{
	float rise = (line->y1 - line->y2);
	float run = (line->x1 - line->x2);
	if (run == 0)
	{
		return 0;
	}
	else
	{
		*result = rise / run;
		return 1;
	}
}

int length(struct geometry_LineSegment * line, float * result)
{
	float rise = line->y2 - line->y1;
	float run = line->x2 - line->x1;

	*result = sqrt(fabs((rise*rise)/(run*run)));

	return 1;
}

int displacement(struct geometry_LineSegment * line, float * result)
{
	float m;
	slope(line, &m);
	//c = y - xm
	* result = line->y1 - line->x1 * m;
	return 1;
}

struct geometry_TreeNode * geometry_TreeNode_construct(float x1, float y1, float x2, float y2)
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

void geometry_destroy()
{
	geometry_TreeNode_destroyRecurse(geometry_Root);
}

void geometry_TreeNode_destroyRecurse(struct geometry_TreeNode * node)
{
	int i;
	if (node->children != 0)
	{
		for (i = 0; i < TREE_ORDER; i++)
		{
			geometry_TreeNode_destroyRecurse(node->children[i]);
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
struct geometry_TreeNode * geometry_GetRootNode()
{
	return geometry_Root;
}
void geometry_WalkTree()
{
	geometry_WalkTreeRecurse(geometry_Root);
}
void geometry_WalkTreeRecurse(struct geometry_TreeNode * node)
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
			geometry_WalkTreeRecurse(node->children[i]);
		}
	}

}

struct geometry_LineSegment * geometry_LineSegment_construct(float x1, float y1, float x2, float y2)
{
	struct geometry_LineSegment * line;
	line = (struct geometry_LineSegment *) malloc(sizeof (struct geometry_LineSegment));
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;
	line->id = geometry_LineCount++;


	line->angle = randRange(0.0, 360.0);
	line->rx = randRange(-5.0, 5.0);
	line->ry = randRange(-5.0, 5.0);
	line->rz = randRange(-5.0, 5.0);
	line->dx = randRange(-5.0, 5.0);
	line->dy = randRange(-5.0, 5.0);
	line->dz = 5.0;
	line->ticks = (int)randRange(0.0, 300);

	if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
	{
		puts("Infinite subdivision");
	}
	return line;
}
struct geometry_LineSegment * geometry_LineSegment_constructDummy(float x1, float y1, float x2, float y2)
{
	struct geometry_LineSegment * line;
	line = (struct geometry_LineSegment *) malloc(sizeof (struct geometry_LineSegment));
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;

	if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
	{
		puts("Infinite subdivision");
	}
	return line;
}
struct geometry_LineSegment * geometry_LineSegment_constructSplit(float x1, float y1, float x2, float y2, struct geometry_LineSegment * init)
{
	struct geometry_LineSegment * line;
	line = (struct geometry_LineSegment *) malloc(sizeof (struct geometry_LineSegment));
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;
	line->id = init->id;

	line->angle = init->angle;
	line->rx = init->rx;
	line->ry = init->ry;
	line->rz = init->rz;
	line->dx = init->dx;
	line->dy = init->dy;
	line->dz = init->dz;
	line->ticks = init->ticks;

	if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
	{
		puts("Infinite subdivision");
	}
	return line;
}
void geometry_CheckCollision(struct geometry_LineSegment * line, struct geometry_TreeNode * node, float * resultX, float * resultY)
{
	int i;
	if (geometry_LineFitsInNode(line, node) || geometry_BoxTest(line, node))
	{
		//printf("%d\n", node->id);
		if (node->children != 0)
		{
			for (i = 0; i < TREE_ORDER; i++)
			{
				geometry_CheckCollision(line, node->children[i], resultX, resultY);
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
						if (*resultY == -1.25)
						{
							puts("Moo!");
						}
						//printf("Collision! %d %f %f\n", line->id, tmpX, tmpY);
					}
					if (tmpY == -1.25)
					{
						puts("Moo!");
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
int geometry_BoxTest(struct geometry_LineSegment * line, struct geometry_TreeNode * node)
{
	int ret = 0;
	struct geometry_LineSegment * test1 = geometry_LineSegment_constructDummy(node->x1, node->y1, node->x2, node->y1);
	struct geometry_LineSegment * test2 = geometry_LineSegment_constructDummy(node->x1, node->y1, node->x1, node->y2);
	struct geometry_LineSegment * test3 = geometry_LineSegment_constructDummy(node->x1, node->y2, node->x2, node->y2);
	struct geometry_LineSegment * test4 = geometry_LineSegment_constructDummy(node->x2, node->y1, node->x2, node->y2);

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
