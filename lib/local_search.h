#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "Snap.h"
#include "Utility.h"

#define CLOCK_SECOND 1000000.0
#define MAXST 4096
#define N_LABEL_BIT 3
#define USED_BIT 4
#define KEY_MAX 16

#define LABEL_TREE 0
#define LABEL_PATH 1

// para for tree
#define PARA_MAX 1
#define PARA_VTOUR 2
#define PARA_BIDIRECT 4

// para for tree inefficient
#define PARA_SUBVTOUR 8
#define PARA_PATH 16
#define PARA_VPATH 32
#define PARA_TREE 128

// para for path
#define PARA_OPATH 256
#define PARA_OPATH_MAX 512
#define PARA_OPATH_VPATH 1024
#define PARA_OPATH_BIDIRECT 2048

#define PATH_MUL 1

#define LMD 0.5

const int INTINF = 100000000;

const int MAXHEAP = 1500000000;

using namespace std;

class Word {
  public:
    string st;
    vector<int> v_node;
    int pos;
    int tlen;

  public:
    Word(string st);
};

int compare_word(const void *w1, const void *w2);
int compare_word_freq(const void *w1, const void *w2);

class Application {
  private:
    static void add_st(char *st, int id, vector<Word *> &v_word,
                       map<string, Word *> &m_word);
    static void save_word_ind_bin(Word **l_word, FILE *f_word_ind, int &now_pos,
                                  int s, int t);
    static void union_node_set(vector<int> &v_rst, vector<int> &v_node);
    static void find_word_node(string path, string w, vector<int> &v_node);

  public:
    static void create_node_index(string path);
    static void create_edge_index(string path);
    static void create_word_index(string path);

    static int get_n_node(string path);
    static int get_n_edge(string path);
    static void find_term_node(string path, string term,
                               vector<int> &v_node); // Lu Qin
    static string find_node_st(string path, int id);
};

class S {
  public:
    static int *n;
    static vector<int> *v;

    static int *ns;
    static vector<int> *vs;

  public:
    static void init();
    static void clear();
};

class PEntry {
  public:
    int a, b, set;
    int idx;
    int info; // 0:not used, 1:used
    int cost;

  public:
    PEntry();
};

class KeyInfo {
  public:
    string key;
    int n_node;
    vector<int> v_node;

  public:
    KeyInfo(string key);
};

class Node {
  public:
    int len;
    int *adj;
    int *dis;

  public:
    Node();
};

class Graph {
  private:
    static const int max_node = 30000000;

  public:
    string path;
    int n, m;
    int *adj;    // of size m
    int *dis;    // of size m
    Node *nodes; // of size n

  public:
    Graph();
    ~Graph();

  public:
    void print();
    void trans_graph(PUndirNet &cnet);
};

class DisOracle {
  public:
    Graph *g;
    int n_key;

    int n;
    int *timestamp;
    int nowtime;

    int *visit_now;
    int n_now;

    int *src_now;
    int *src_nxt;

    int *visit_nxt;
    int n_nxt;

    int *seen;

    int **dis;
    int **dis_key;

    int *key_set;

    bool empty;
    int nowd;

    bool can_key_dis;

  public:
    DisOracle(Graph *g, vector<KeyInfo *> &v_key, int *key_set);
    ~DisOracle();
    int get_dis(int i_key, int u);
    int get_dis_key(int i_key, int key);
    void bfs_one_round();
    void disable_key_dis();

    bool get_path(int i_key, int u, vector<int> &v_path);
};

class PEntryHeap {
  public:
    static const int n_block_pentry = 1000000;

  public:
    PEntryHeap(DisOracle *d);
    ~PEntryHeap();

  private:
    vector<PEntry *> v_block;
    vector<int> v_pos;

  private:
    PEntry *get_entry();

  private:
    int l;
    PEntry ****tab;  // a, b, set
    PEntry ***tab_a; // a, set

  public:
    int get_dis(int a, int b, int set); // from a to set to b
    int get_dis(int a, int set);        // from a to set

  private:
    vector<PEntry *> heap;
    int tot;

  private:
    void up(int p);
    void down(int p);

    void enheap(int a, int b, int set, int cost);
    void update(PEntry *e);
    PEntry *deheap();
    bool empty();

  private:
    void update(int a, int b, int set, int cost);

  public:
    void init(DisOracle *d);
};

class Entry {
  public:
    int set;  // set of keywords
    int p;    // node id
    int idx;  // index in heap
    int cost; // cost in heap
    int val;  // the current cost
    int info; // first three bits: label; fourth bit: used
    Entry *next;

  public:
    Entry();
};

class EntryHeap {
  private:
    static const int n_block_entry = 1000000;
    static const int init_hash_len = 1111111;

  private:
    vector<Entry *> v_block;
    vector<int> v_pos;

  public:
    EntryHeap();
    ~EntryHeap();

  private:
    Entry *get_entry();

  private:
    Entry **hash;
    int len_hash;
    int n_hash;

  private:
    int h(Entry *e);
    int h(int p, int set, int label);

  public:
    void add(Entry *e);
    Entry *find(int p, int set, int label);
    int len();

  private:
    vector<Entry *> heap;
    int tot;

    void up(int p);
    void down(int p);

  public:
    Entry *enheap(int p, int set, int label, int val, int cost);
    void update(Entry *e);
    Entry *deheap();
    bool empty();
};

class ResultNode {
  public:
    int id;
    vector<ResultNode *> con;
    bool used;

  public:
    ResultNode(int id);
};

class ResultTree {
  public:
    bool is_tree;
    string path;
    vector<ResultNode *> v_node;
    map<int, ResultNode *> m_node;
    int n_edge;

  public:
    ResultTree(string path);
    ~ResultTree();
    ResultNode *get_node(int id);
    void add_edge(int id1, int id2);
};

class Algorithm {
  private:
    Graph *g;
    string query;
    vector<KeyInfo *> v_key;
    // vector<Dijkstra*> v_dij;
    // vector<BFS*> v_dij;
    DisOracle *d;
    int *key_set;
    int n_key;
    int type;
    ResultTree *rt;

  private:
    EntryHeap *heap;
    PEntryHeap *pheap;

  private:
    vector<Entry *> *v_entry;

  private:
    int best;
    int pre_best;
    int pre_min_cost;
    bool show_progress_result;

  public:
    int min_cost;
    long tm_start;
    double processing_time;
    double total_memory;
    int heap_len;
    bool succ;
    vector<double> diagram_time;
    vector<double> diagram_qlty;
    vector<double> diagram_memy;
    vector<int> diagram_best;
    vector<int> diagram_minc;
    string fail_reason;

  public:
    static void parse_query(std::vector<TIntV> &QInComV, Graph *g,
                            vector<KeyInfo *> &v_key, int *&key_set);
    static void parse_query(std::vector<TIntV> &QInComV, Graph *g,
                            vector<KeyInfo *> &v_key, DisOracle *&d,
                            int *&key_set);

  private:
    void update(int u, int set, int val);
    void update_path(int u, int set, int val);

    int get_lb(int u, int set, int val);
    int get_lb_path(int u, int set, int val);

  private:
    void create_result_tree(Entry *e);
    void create_result_path_tree(Entry *e);

  public:
    static void print_result(ResultTree *rt);
    static void print_result(ResultTree *rt, string st, ResultNode *pre,
                             ResultNode *now);

  public:
    void run(PUndirNet &net, std::vector<TIntV> &QInComV, int type,
             bool show_progress_result = false);

  public:
    void print_progress(int cnt, int cnt_tree, int cnt_path);
    void record_progress();
    void print_result(Entry *e);
    void print_progress_result(Entry *e);
    void print_diagram();
    double get_memory();
};

class BanksNode {
  public:
    int dis;
    double val;
    int id;
    BanksNode *pre;
    int idx;
};

class BanksKeyIter {
  public:
    Graph *g;
    int key_id;
    KeyInfo *key;
    BanksNode *nodes;
    BanksNode **heap;
    BanksNode ***key_dis;
    int tot;

  public:
    void init();
    void clear();
    void up(int p);
    void down(int p);
    bool empty();
    double get_max_val();
    void enheap(BanksNode *bn);
    void update(BanksNode *bn);
    BanksNode *deheap();
    int expand();
    void update_dis(BanksNode *bn);
};

class BanksAlgorithm {
  public:
    Graph *g;
    string query;
    vector<KeyInfo *> v_key;
    int n_key;
    BanksKeyIter *key_iter;
    int *key_set;

    BanksNode ***key_dis;

    int best_cost;
    int best_root;
    double total_memory;
    double process_time;

  public:
    void update_best(int u);
    void output(int u);
    bool is_complete(int u);
    int find_best();
    void run(PUndirNet &net, std::vector<TIntV> &QInComV, int type,
             bool show_progress_result);
};

class Exp {
  public:
    static const string in_path;
    static const string out_path;

    static const int n_knum;
    static const string v_knum[];
    static const string default_knum;

    static const int n_dataset;
    static const string v_dataset[];

    static const int n_alg;
    static const string v_alg[];

    static const int n_kwf;
    static const string v_kwf[];
    static const string default_kwf;

    static const double max_apr;

  public:
    static void draw_diagram(string dataset, string alg, string knum,
                             string kwf);
    static void exp_all();
};

#endif