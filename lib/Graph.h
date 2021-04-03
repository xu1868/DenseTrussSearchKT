#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "Utility.h"
#include "mynet.h"
#include "Heap.h"
#include "LinearHeap.h"
#include "wgtnet.h"

struct Bin;

struct Element{
	Element *next;//结构体的自引用
	int value;
};

struct Edge {
	Edge *pre, *next;
	Edge *duplicate;
	Bin *nontree;
	int node_id, sc;
	char deleted;
};

struct Node {
	Element *head, *tail;
	Edge *first, *last;
	Node():head(NULL),tail(NULL),first(NULL),last(NULL){}
};

struct Bin {
	Edge *edge;
	Bin *pre, *next;
};

class Graph{
private:
	std::string dir;
	int K;
	int n, m;

	Element *elements;
	Node *nodes;
	Edge *edges;

	Node *pnodes;
	Edge *pedges;

	char *inL;
	int *computed;
	int *height;

	int *q;

	int *degrees;

	Heap *heap;

	int *cids;
	int *levels;
	int *pos_id;
	int *parent;
	int weights_n;
	int *weights;
	int ns, logns;
	int **st;
	int nt, logn;
	int **table;
	int *log_table;
	int *t_id;

public:
	//Graph(const char *_dir, const PUndirNet& Graph);
	Graph( );
	~Graph();

	void read_graph(int binary);

	void read_graph_Gsc(const PWgtNet& Graph);
	void read_graph(int binary, std::string dir);
	void add_edge(Node &node, Edge *edge);
	void KECC_start(const PUndirNet& net);
	void KECC(int q_n, int *qq, int L);
	void kcore_optimization(int q_c, int assign_sc = 0) ;
	void delete_edge_to_last(Node &node, Edge *edge) ;
	int construct_pgraph(int s, int *height) ;
	void decomposition(int s, std::vector<Element *> &cc, int &max_l) ;
	void merge(int s, int t, Heap *heap) ;
	void delete_edge(Node &node, Edge *edge) ;
	void remove_inter_edges(const std::vector<Element *> &cc, int assign_sc = 0) ;
	int kSC(int _K);
	void extract_mcc() ;
	void output_k_edge_connected_subgraphs(int o, int sc_max, int sc_min) ;
	void expand(const PUndirNet& net, int k, TIntV& QueryV, TIntV& NodeV);
	void compute_Gk();
	void all_SC_naive(int sc_max, int sc_min);
	void output_all_sc(FILE *fout);
	void optimization_tree(int output);
	int find_root(int x, int *parent);
	void construct_LCA_index(FILE *fout, FILE *fout_bin, int n_levels, int *levels, int *weights, int output);
	void DFS(int pid, int id, int lid, const Node *pnodes, const Edge *pedges, int *weight, int &n_levels, int *levels, int *id_pos, int *weights);
	void max_spanning_tree();
	void read_mSPTO();
	int sc_query_mSPTO(int q_n, int *qq);
	int mSPTO_P2P(int a, int b);
	void sc(const char *file_name, int method);
	void read_graph_nodes(const PUndirNet& Graph, TIntV& nodev);
	void read_graph_Gsc(const PUndirNet& Graph);
	void read_graph_new(int binary);
	void count_cc(int k);
	//void output_k_edge_connected_subgraphs_refine(int o) ;
};
#endif