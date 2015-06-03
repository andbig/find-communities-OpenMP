#define DEFAULT_INIT_NODES_SIZE 100
#define DEFAULT_INIT_NEIGHBOURS_SIZE 100

//DynamicGraph

typedef struct weighted_edge {
	int dest;
	int weight;
} weighted_edge;

typedef struct dynamic_weighted_edge_array{
	weighted_edge *addr;
	int self_loop;
	int count;
	int size;
} dynamic_weighted_edge_array;

int dynamic_weighted_edge_array_init(dynamic_weighted_edge_array *da, int initSize);

int dynamic_weighted_edge_array_insert(dynamic_weighted_edge_array *da, int dest, int weight);

void dynamic_weighted_edge_array_print(dynamic_weighted_edge_array da);

weighted_edge dynamic_weighted_edge_array_retrieve(dynamic_weighted_edge_array da, int index);

int dynamic_weighted_edge_array_resize(dynamic_weighted_edge_array *da, int size);

typedef struct dynamic_weighted_graph{
		dynamic_weighted_edge_array *edges;       //List of adjacency lists of vertices in array form
        int size;
        int maxn;
        } dynamic_weighted_graph;


int dynamic_weighted_graph_init(dynamic_weighted_graph *da, int initSize);

int dynamic_weighted_graph_resize(dynamic_weighted_graph *da, int size);

//Bi-directional link
int dynamic_weighted_graph_insert(dynamic_weighted_graph *da, int n1, int n2, int weight);

void dynamic_weighted_graph_print(dynamic_weighted_graph dg);

// Retrieve neighborhood dynamic array of given node
dynamic_weighted_edge_array DynGraphRetrieveNeighbors(dynamic_weighted_graph da, int node);

int dynamic_weighted_graph_reduce (dynamic_weighted_graph *dg);


int dynamic_weighted_graph_parse_file(dynamic_weighted_graph *dg, char *filename);

int dynamic_weighted_graph_node_degree(dynamic_weighted_graph *dg, int index);
