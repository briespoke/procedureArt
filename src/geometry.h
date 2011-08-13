#define BUFFER_SIZE 500
#define GEOMETRY_BUCKET_SIZE 4
#define TREE_ORDER 4
struct geometry_TreeNode;
struct geometry_Shape
{
	int refCount;
	int type;
	struct geometry_Vertex* vertices;
};
struct geometry_Vertex
{
	float x, y;
	struct geometry_Shape shape;
};
struct geometry_TreeNode
{
	float x1, y1, x2, y2;
	int numVertices;
	struct geometry_Vertex vertices[GEOMETRY_BUCKET_SIZE];

	struct geometry_TreeNode ** children;
};

void geometry_Init();

void geometry_addShape(int type, float * vertices);
void geometry_addVertex(struct geometry_TreeNode* node, struct geometry_Vertex * vertex, struct geometry_Shape* shape);

struct geometry_TreeNode * geometry_TreeNode_Construct(float x1, float y1, float x2, float y2);
void geometry_destroy(struct geometry_Shape * node);
