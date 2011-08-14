#define BUFFER_SIZE 500
#define GEOMETRY_BUCKET_SIZE 4
#define TREE_ORDER 4

struct geometry_LineSegment
{
	float x1, y1, x2, y2;
};
struct geometry_TreeNode
{
	float x1, y1, x2, y2;
	int numLines;
	struct geometry_LineSegment * lines[GEOMETRY_BUCKET_SIZE + 1];
	struct geometry_TreeNode ** children;
};

void geometry_init(float x1, float y1, float x2, float y2);


void geometry_addLine(struct geometry_LineSegment * newLine);
void geometry_addLineRecurse(struct geometry_TreeNode* node, struct geometry_LineSegment * newLine);

struct geometry_TreeNode * geometry_TreeNode_construct(float x1, float y1, float x2, float y2);

void geometry_destroy();
void geometry_TreeNode_destroyRecurse(struct geometry_TreeNode * node);

struct geometry_LineSegment * geometry_LineSegment_construct(float x1, float y1, float x2, float y2);

int geometry_LineFitsInNode(struct geometry_LineSegment * line , struct geometry_TreeNode *childNode);

int geometry_LineIntersect(struct geometry_LineSegment * line1, struct geometry_LineSegment * line2, float * resultX, float *resultY);

int inRange(struct geometry_LineSegment * line, float test);

int slope(struct geometry_LineSegment * line, float * result);
int displacement(struct geometry_LineSegment * line, float slope, float * result);

void geometry_WalkTree();
void geometry_WalkTreeRecurse(struct geometry_TreeNode * node);

struct geometry_TreeNode * geometry_GetRootNode();
