#include "Utility.h"
#include "Graph.h"

/*Graph::Graph(const char *_dir, const PUndirNet& Graph){
	dir = std::string(_dir);

	K = -1;

	nodes = NULL;
	edges = NULL;
	pnodes = NULL;
	pedges = NULL;

	inL = NULL;
	computed = NULL;
	height = NULL;

	elements = NULL;

	q = NULL;

	degrees = NULL;

	heap = NULL;
}*/

Graph::Graph( ){
	dir = std::string("../0828");

	K = -1;

	nodes = NULL;
	edges = NULL;
	pnodes = NULL;
	pedges = NULL;

	inL = NULL;
	computed = NULL;
	height = NULL;

	elements = NULL;

	q = NULL;

	degrees = NULL;

	heap = NULL;

	cids = NULL;
	levels = NULL;

	parent = NULL;
	weights = NULL;

	pos_id = NULL;
	st = NULL;
	t_id = NULL;
	table = NULL;

	log_table = NULL;
}
Graph::~Graph(){
	if(nodes != NULL) delete[] nodes;
	if(edges != NULL) delete[] edges;
	if(pnodes != NULL) delete[] pnodes;
	if(pedges != NULL) delete[] pedges;

	if(inL != NULL) delete[] inL;

	if(elements != NULL) delete[] elements;

	if(computed != NULL) delete[] computed;
	if(height != NULL) delete[] height;
	if(degrees != NULL) delete[] degrees;

	if(q != NULL) delete[] q;

	if(heap != NULL) delete heap;

	if(cids != NULL) delete[] cids;
	if(levels != NULL) delete[] levels;

	if(parent != NULL) delete[] parent;
	if(weights != NULL) delete[] weights;

	if(pos_id != NULL) delete[] pos_id;
	if(st != NULL) {
		for(int i = 0;i < logns;i ++) {
			delete[] st[i];
			st[i] = NULL;
		}
		delete[] st;
	}
	if(t_id != NULL) delete[] t_id;
	if(table != NULL) {
		for(int i = 0;i < nt;i ++) {
			delete[] table[i];
			table[i] = NULL;
		}
		delete[] table;
	}

	if(log_table != NULL) delete[] log_table;
}

void Graph::read_graph(int binary, std::string dir) {
	FILE *f;
	if(binary) f = open_file((dir + std::string("/edges.bin")).c_str(), "rb");
	else f = open_file((dir + std::string("/edges.txt")).c_str(), "r");

	if(binary) {
		fread(&n, sizeof(int), 1, f);
		fread(&m, sizeof(int), 1, f);
	}
	else fscanf(f, "%d%d", &n, &m);
	//n = 1547493;
	//printf("%d %d\n", n, m);

	nodes = new Node[n];//n=13 13个Node结构体类型的变量。
	edges = new Edge[2*m];//m=24 edges是结构体数组

	q = new int[n];

	int edge_c = 0;

	for(int i = 0;i < n;i ++) nodes[i].first = NULL;

	int *buf = NULL;
	//printf("ddddd");
	if(binary) {
		buf = new int[2*n+m];
		fread(buf, sizeof(int), 2*n+m, f);

		int j = 0;
		//printf("aaa");
		for(int i = 0;i < n;i ++) {
			if(buf[j] != i) {
				printf("WA in read binary input graph!\n");
				return ;
			}

			int d = buf[j+1];
			j += 2;
			while(d --) {
				int a = i, b = buf[j];
				++ j;

				edges[edge_c].node_id = b;
				//std::cout << edges[edge_c].node_id << std::endl ;
				edges[edge_c].sc = 1;
				edges[edge_c].deleted = 0;
				edges[edge_c].duplicate = &edges[edge_c+1];//结构体edges[1]的地址
				add_edge(nodes[a], &edges[edge_c]);//参数nodes[0]是结构体，&edges[0]是地址
				++ edge_c;

				edges[edge_c].node_id = a;
				//std::cout << edges[edge_c].node_id << std::endl ;
				edges[edge_c].sc = 1;
				edges[edge_c].deleted = 0;
				edges[edge_c].duplicate = &edges[edge_c-1];
				add_edge(nodes[b], &edges[edge_c]);
				++ edge_c;
			}
		}
	}

	else {

		for(int i = 0;i < m;i ++) {
			//printf("WA in read graph!\n");
			int a, b;
			fscanf(f, "%d%d", &a, &b);

			edges[edge_c].node_id = b;
			//std::cout << edges[edge_c].node_id << std::endl ;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c+1];
			++ edge_c;

			edges[edge_c].node_id = a;
			//std::cout << edges[edge_c].node_id << std::endl ;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c-1];
			add_edge(nodes[b], &edges[edge_c]);
			++ edge_c;
		}
	}

	fclose(f);
	//printf("here");
	if(buf != NULL) delete[] buf;
	
	pnodes = new Node[n];
	pedges = new Edge[2*m];

	//std::cout<< pnodes<< std::endl;
	//printf("here2");
	for(int i = 0;i < n;i ++) pnodes[i].first = NULL;

	elements = new Element[n];
	for(int i = 0;i < n;i ++) elements[i].value = i;
}

void Graph::read_graph_new(int binary) {
	FILE *f;
	if(binary) f = open_file((dir + std::string("/newedges.bin")).c_str(), "rb");
	else f = open_file((dir + std::string("/newedges.txt")).c_str(), "r");

	if(binary) {
		fread(&n, sizeof(int), 1, f);
		fread(&m, sizeof(int), 1, f);
	}
	else fscanf(f, "%d%d", &n, &m);
	//n = 1547493;
	printf("%d %d\n", n, m);

	nodes = new Node[n];//n=13 13个Node结构体类型的变量。
	edges = new Edge[2*m];//m=24 edges是结构体数组

	q = new int[n];

	int edge_c = 0;

	for(int i = 0;i < n;i ++) nodes[i].first = NULL;

	int *buf = NULL;

	if(binary) {
		buf = new int[2*n+m];
		fread(buf, sizeof(int), 2*n+m, f);

		int j = 0;
		for(int i = 0;i < n;i ++) {
			if(buf[j] != i) {
				printf("WA in read binary input graph!\n");
				return ;
			}

			int d = buf[j+1];
			j += 2;
			while(d --) {
				int a = i, b = buf[j];
				++ j;

				edges[edge_c].node_id = b;
				//std::cout << edges[edge_c].node_id << std::endl ;
				edges[edge_c].sc = 1;
				edges[edge_c].deleted = 0;
				edges[edge_c].duplicate = &edges[edge_c+1];//结构体edges[1]的地址
				add_edge(nodes[a], &edges[edge_c]);//参数nodes[0]是结构体，&edges[0]是地址
				++ edge_c;

				edges[edge_c].node_id = a;
				//std::cout << edges[edge_c].node_id << std::endl ;
				edges[edge_c].sc = 1;
				edges[edge_c].deleted = 0;
				edges[edge_c].duplicate = &edges[edge_c-1];
				add_edge(nodes[b], &edges[edge_c]);
				++ edge_c;
			}
		}
	}

	else {

		for(int i = 0;i < m;i ++) {
			//printf("WA in read graph!\n");
			int a, b;
			fscanf(f, "%d%d", &a, &b);

			edges[edge_c].node_id = b;
			//std::cout << edges[edge_c].node_id << std::endl ;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c+1];
			++ edge_c;

			edges[edge_c].node_id = a;
			//std::cout << edges[edge_c].node_id << std::endl ;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c-1];
			add_edge(nodes[b], &edges[edge_c]);
			++ edge_c;
		}
	}

	fclose(f);
	//printf("here");
	if(buf != NULL) delete[] buf;
	
	pnodes = new Node[n];
	pedges = new Edge[2*m];

	//std::cout<< pnodes<< std::endl;
	//printf("here2");
	for(int i = 0;i < n;i ++) pnodes[i].first = NULL;

	elements = new Element[n];
	for(int i = 0;i < n;i ++) elements[i].value = i;
}

void Graph::read_graph_Gsc(const PWgtNet& Graph) {
	n = Graph->GetMxNId();
	m = Graph->GetEdges();
	printf("\nnodes %d edges %d\n", n, m);
	//n = 2095;
	//m = 48976;
	nodes = new Node[n];//n=13 13个Node结构体类型的变量。
	//printf("nodes");
	edges = new Edge[2*m];//m=24 edges是结构体数组
	//printf("11");
	q = new int[n];
	//printf("12");
	int edge_c = 0;
	//printf("13");
	for(int i = 0;i < n;i ++) nodes[i].first = NULL;
	//printf("14");
	//Graph->SortNIdById();
	//printf("...");
	for(PWgtNet::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
		int a = NI.GetId();
		//printf("%d", a);
		for(int j = 0; j < NI.GetDeg(); j++){
			int b = NI.GetNbrNId(j);
			edges[edge_c].node_id = b;//这里有问题
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c+1];
			++ edge_c;

			edges[edge_c].node_id = a;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c-1];
			add_edge(nodes[b], &edges[edge_c]);
			++ edge_c;
		}
	}
	/*for(PWgtNet::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {

			int a, b;
			a = EI.GetSrcNId();
			b = EI.GetDstNId();

			edges[edge_c].node_id = b;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c+1];
			++ edge_c;

			edges[edge_c].node_id = a;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c-1];
			//printf("%d %d\n", a, b);
			//printf("%x\n",nodes[b].first);
			add_edge(nodes[b], &edges[edge_c]);
			++ edge_c;
			//printf("%d %d\n", edge_c, b);
	}*/
	
	pnodes = new Node[n];
	pedges = new Edge[2*m];

	for(int i = 0;i < n;i ++) pnodes[i].first = NULL;

	elements = new Element[n];
	for(int i = 0;i < n;i ++) elements[i].value = i;
}

void Graph::read_graph_nodes(const PUndirNet& Graph, TIntV& nodev){
	n = nodev.Len();
	PUndirNet net = PUndirNet::New();

	for(int i = 0; i < n; i++){
		int scr = nodev[i];
		if(!net->IsNode(scr)){
			net->AddNode(scr);
		}
		for(int j = 0; j < n; j++){
			int dst = nodev[j];
			if(!net->IsNode(dst)){
				net->AddNode(dst);
			}
			if(Graph->IsEdge(scr, dst) && (!net->IsEdge(scr, dst))){
				net->AddEdge(scr, dst);
			}
		}
	}
	
	n = net->GetMxNId();
	m = net->GetEdges();
	printf("\nnodes %d edges %d\n", n, m);

	nodes = new Node[n];//n个Node结构体类型的变量。
	edges = new Edge[2*m];//edges是结构体数组

	q = new int[n];

	int edge_c = 0;

	for(int i = 0;i < n;i ++) nodes[i].first = NULL;
	
	for(PUndirNet::TObj::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++){
		int a = NI.GetId();
		for(int j = 0; j < NI.GetDeg(); j++){
			int b = NI.GetNbrNId(j);
			edges[edge_c].node_id = b;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c+1];
			++ edge_c;

			edges[edge_c].node_id = a;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c-1];
			add_edge(nodes[b], &edges[edge_c]);
			++ edge_c;
		}
	}
	
	pnodes = new Node[n];
	pedges = new Edge[2*m];

	for(int i = 0;i < n;i ++) pnodes[i].first = NULL;

	elements = new Element[n];
	for(int i = 0;i < n;i ++) elements[i].value = i;
}

void Graph::read_graph_Gsc(const PUndirNet& Graph) {
	n = Graph->GetMxNId();
	m = Graph->GetEdges();
	printf("\nnodes %d edges %d\n", n, m);
	//n = 2095;
	//m = 48976;
	nodes = new Node[n];//n=13 13个Node结构体类型的变量。
	edges = new Edge[2*m];//m=24 edges是结构体数组

	q = new int[n];

	int edge_c = 0;

	for(int i = 0;i < n;i ++) nodes[i].first = NULL;
	
	for(PUndirNet::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
		int a = NI.GetId();
		for(int j = 0; j < NI.GetDeg(); j++){
			int b = NI.GetNbrNId(j);
			edges[edge_c].node_id = b;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c+1];
			++ edge_c;

			edges[edge_c].node_id = a;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c-1];
			add_edge(nodes[b], &edges[edge_c]);
			++ edge_c;
		}
	}
	/*for(PWgtNet::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {

			int a, b;
			a = EI.GetSrcNId();
			b = EI.GetDstNId();

			edges[edge_c].node_id = b;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c+1];
			++ edge_c;

			edges[edge_c].node_id = a;
			edges[edge_c].sc = 1;
			edges[edge_c].deleted = 0;
			edges[edge_c].duplicate = &edges[edge_c-1];
			//printf("%d %d\n", a, b);
			//printf("%x\n",nodes[b].first);
			add_edge(nodes[b], &edges[edge_c]);
			++ edge_c;
			//printf("%d %d\n", edge_c, b);
	}*/
	
	pnodes = new Node[n];
	pedges = new Edge[2*m];

	for(int i = 0;i < n;i ++) pnodes[i].first = NULL;

	elements = new Element[n];
	for(int i = 0;i < n;i ++) elements[i].value = i;
}

void Graph::add_edge(Node &node, Edge *edge){
	edge->next = NULL;
	//printf("%x\n", node.first);
	if(node.first == NULL) {
		node.first = node.last = edge;
		edge->pre = NULL;
		//printf("if %d\n", edge->node_id);
	}
	else {
		node.last->next = edge;
		edge->pre = node.last;
		node.last = edge;
		//printf("else %d\n", edge->node_id);
	}
}

void Graph::KECC_start(const PUndirNet& net){
	std::vector<int> query;

	int t = net->GetNodes();
	PUndirNet::TObj::TNodeI TI = net->BegNI();
	for(int i = 0; i < t; i++){
		int d;
		d = TI.GetId();
		query.push_back(d);
		if(TI.GetId() == t-1){ break; }
		TI++;
	}

	int start, end;
	start = clock();

	int qq[40], q_n;
	
	q_n = query.size();
	for(int j = 0;j < q_n;j ++) qq[j] = query[j];
	//printf("q_n: %d\n", q_n);
	KECC(q_n, qq, 0);
	
	end = clock();

	printf("smcc_naive time: %d\n", end-start);
}

void Graph::KECC(int q_n, int *qq, int L){
	if(computed == NULL) computed = new int[n];
	if(height == NULL) height = new int[n];
	if(degrees == NULL) degrees = new int[n];

	for(int ii = 2;ii <= n;ii ++) {
		K = ii;
		printf("%d \n", K);//2
		for(int i = 0;i < n;i ++) computed[i] = height[i] = 0;

		int q_c = 0;
		for(int i = 0;i < n;i ++) {
			int cnt = 0;
			for(Edge *edge = nodes[i].first;edge != NULL;edge = edge->next) {
				edge->deleted = 0;
				nodes[i].last = edge;
				++ cnt;
			}

			if(cnt < K) {
				q[q_c] = i;
				++ q_c;
			}

			degrees[i] = cnt;
		}
		printf("%d \n", q_c);//1084
		kcore_optimization(q_c, 1);

		int max_l = 0;
		Element *res = NULL;

		while(1) {
			construct_pgraph(qq[0], height);

			std::vector<Element *> cc;
			decomposition(qq[0], cc, max_l);
			printf("%d \n", cc.size());//1
			if(cc.size() == 1) {
				res = cc[0];
				break;
			}
			else remove_inter_edges(cc, 1);
		}

		for(int i = 0;i < n;i ++) computed[i] = 0;
		int cnt = 0;
		for(Element *e = res;e != NULL;e = NULL) {
			computed[e->value] = 1;
			++ cnt;
		}
		int find = 1;
		for(int i = 0;i < q_n;i ++) if(!computed[qq[i]]) find = 0;

		/*int *id = new int[n];
		for(int i = 0;i < n;i ++) id[i] = -1;
		for(int i = 0;i < q_c;i ++) id[q[i]] = i;

		printf("n = %d, m = %d; nn = %d, nm = %d\n", n, m, q_c, cnt);
		
		//FILE *open_file(const char *file_name, const char *mode) ;
		
		FILE *fout = open_file((std::string("/edges_mcc.txt")).c_str(), "w");

		fprintf(fout, "%d %d\n", q_c, cnt);
		for(int i = 0;i < q_c;i ++) for(Edge *e = nodes[q[i]].first;e != NULL;e = e->next) if(e->node_id > q[i]) {
			if(id[q[i]] == id[e->node_id]) printf("WA in extract_mcc!\n");

			fprintf(fout, "%d %d\n",  id[q[i]], id[e->node_id]);
		}

		fclose(fout);*/

		if(!find||cnt < L) break;
	}
}

void Graph::kcore_optimization(int q_c, int assign_sc) {
	for(int i = 0;i < q_c;i ++) {
		int s = q[i];
		computed[s] = 1;
		//printf("s %d\n", s);
		for(Edge *edge = nodes[s].first;edge != NULL&&!edge->deleted;edge = edge->next) {
			int t = edge->node_id;
			//printf("%d\n", t);
			delete_edge_to_last(nodes[t], edge->duplicate);//107
			//printf("aaaa\n");
			-- degrees[t];

			if(degrees[t] == K-1) {
				q[q_c] = t;
				++ q_c;
			}
			edge->deleted = 1;

			if(assign_sc) edge->sc = edge->duplicate->sc = K-1;
		}

		nodes[s].last = nodes[s].first;
	}
}

void Graph::delete_edge_to_last(Node &node, Edge *edge) {
	edge->deleted = 1;
	if(node.first == node.last) {
		//printf("one");
#ifdef _DEBUG_
		if(node.first != edge) printf("WA in delete_edge_to_last!\n");
#endif
	
		return ;
	}

	if(edge->pre == NULL) {
		//printf("two");
		node.first = edge->next;
		if(edge->next != NULL) edge->next->pre = NULL;
	}
	else {
		//printf("three");
			if(edge == node.last) {
				node.last = edge->pre;
			}

			Edge *tmp = edge->pre;
			tmp->next = edge->next;
		
			if(edge->next != NULL) edge->next->pre = tmp;
	}

	edge->next = node.last->next;
	if(edge->next != NULL) edge->next->pre = edge;
	node.last->next = edge;
	edge->pre = node.last;
}

int Graph::construct_pgraph(int s, int *height) {
	int pedge_c = 0;
	int q_c = 1;

	computed[s] = 1;
	q[0] = s;

	for(int i = 0;i < q_c;i ++) {
		s = q[i];
		++ height[q[i]];

		pnodes[s].head = pnodes[s].tail = &elements[s];
		elements[s].next = NULL;

		for(Edge *edge = nodes[s].first;edge != NULL&&!edge->deleted;edge = edge->next) {
			if(!computed[edge->node_id]) {
				computed[edge->node_id] = 1;
				q[q_c] = edge->node_id;
				++ q_c;
			}

			if(edge->node_id > s) {
				int a = s, b = edge->node_id;

				pedges[pedge_c].node_id = b;
				pedges[pedge_c].sc = 1;
				pedges[pedge_c].duplicate = &pedges[pedge_c+1];
				add_edge(pnodes[a], &pedges[pedge_c]);
				++ pedge_c;

				pedges[pedge_c].node_id = a;
				pedges[pedge_c].sc = 1;
				pedges[pedge_c].duplicate = &pedges[pedge_c-1];
				add_edge(pnodes[b], &pedges[pedge_c]);
				++ pedge_c;
			}
		}
	}

	for(int i = 0;i < q_c;i ++) computed[q[i]] = 0;

	//printf("%d ", q_c);//1
	return q_c;
}

void Graph::decomposition(int ss, std::vector<Element *> &cc, int &max_l) {
	if(heap == NULL) heap = new LinearHeap(n);

	if(inL == NULL) {
		inL = new char[n];
		memset(inL, 0, sizeof(char)*n);
	}

	cc.clear();

	int cnt = 0;

	while(pnodes[ss].first != NULL) {
		int s = ss;

		++ cnt;

		heap->insert(s, 0);

		int q_c = 0;
		int key;

		while(1) {
			
			if(!heap->extract_max(s, key)) break;

			inL[s] = 1;
			//
			q[q_c] = s;
			++ q_c;

			int new_qc = q_c;
			for(int i = q_c - 1;i < new_qc;i ++) {
				int u = q[i];
				for(Edge *e = pnodes[u].first;e != NULL;e = e->next) if(!inL[e->node_id]) {
					int new_key = heap->get_key(e->node_id);
					if(new_key < K) {
						if(new_key > 0) heap->remove(e->node_id);

						new_key += e->sc;
						if(new_key >= K) {
							heap->set_key(e->node_id, new_key);
							q[new_qc ++] = e->node_id;
						}
						else heap->insert(e->node_id, new_key);
					}
					else heap->set_key(e->node_id, new_key + e->sc);
				}

				if(u == s) continue;

				heap->set_key(s, heap->get_key(s) + heap->get_key(u));
				heap->set_key(u, 0);
				inL[u] = 0;
				merge(s, u, heap);
			}
		}

		-- q_c;
		while(q_c > 0&&heap->get_key(q[q_c]) < K) {
			int t = q[q_c]; -- q_c;
			cc.push_back(pnodes[t].head);

			heap->set_key(t, 0);
			inL[t] = 0;

			for(Edge *e = pnodes[t].first;e != NULL;e = e->next) delete_edge(pnodes[e->node_id], e->duplicate);

			pnodes[t].first = NULL;
		}

		for(int i = 0;i <= q_c;i ++) {
			heap->set_key(q[i], 0);
			inL[q[i]] = 0;
		}
	}

	if(cnt > max_l) max_l = cnt;

	cc.push_back(pnodes[ss].head);
}

void Graph::merge(int s, int t, Heap *heap) {
	pnodes[s].tail->next = pnodes[t].head;
	pnodes[s].tail = pnodes[t].tail;

	Edge *e = pnodes[t].first;
	Edge *tmp;

	while(e != NULL) {
		tmp = e->next;

		if(e->node_id == s) {
			if(heap != NULL) heap->set_key(s, heap->get_key(s) - e->sc);
			delete_edge(pnodes[e->node_id], e->duplicate);
		}
		else {
			e->duplicate->node_id = s;
			add_edge(pnodes[s], e);
		}

		e = tmp;
	}

	pnodes[t].first = NULL;
}

void Graph::delete_edge(Node &node, Edge *edge) {
	if(edge->pre == NULL) {
		node.first = edge->next;
		if(edge->next != NULL) edge->next->pre = NULL;
	}
	else {
		if(edge == node.last) node.last = edge->pre;

		Edge *tmp = edge->pre;
		tmp->next = edge->next;

		if(edge->next != NULL) edge->next->pre = tmp;
	}
}

void Graph::remove_inter_edges(const std::vector<Element *> &cc, int assign_sc) {
	for(int j = 0;j < (int)cc.size();j ++) for(Element *e = cc[j];e != NULL;e = e->next) computed[e->value] = j+1;
	
	int q_c = 0;

	for(int j = 0;j < (int)cc.size();j ++) for(Element *e = cc[j];e != NULL;e = e->next) {
		int s = e->value;
		Edge *list = nodes[s].first;
		Edge *delete_first = nodes[s].last->next;
		nodes[s].first = nodes[s].last = NULL;
		int cnt = 0;

		while(list != NULL&&!list->deleted) {
			Edge *tmp = list->next;
			if(computed[list->node_id] == computed[s]) {

				if(nodes[s].first == NULL) {
					nodes[s].first = nodes[s].last = list;
					list->pre = NULL;
				}
				else {
					nodes[s].last->next = list;
					list->pre = nodes[s].last;
					nodes[s].last = list;
				}

				++ cnt;
			}
			else {
				list->deleted = 1;

				if(assign_sc) list->sc = K-1;

				list->next = delete_first;
				if(delete_first != NULL) delete_first->pre = list;
				delete_first = list;
			}

			list = tmp;
		}

		degrees[s] = cnt;
		if(cnt < K) {
			q[q_c] = s;
			++ q_c;
		}

		if(nodes[s].first == NULL) {
			nodes[s].first = nodes[s].last = delete_first;
			if(delete_first != NULL) delete_first->pre = NULL;
		}
		else {
			nodes[s].last->next = delete_first;
			if(delete_first != NULL) delete_first->pre = nodes[s].last;
		}
	}

	for(int j = 0;j < (int)cc.size();j ++) for(Element *e = cc[j];e != NULL;e = e->next) computed[e->value] = 0;

	kcore_optimization(q_c, assign_sc);
}

int Graph::kSC(int _K) {
	K = _K;
	//printf("%d\n",K);
	if(computed == NULL) computed = new int[n];
	if(height == NULL) height = new int[n];
	if(degrees == NULL) degrees = new int[n];
	
	memset(computed, 0, sizeof(int)*n);
	memset(height, 0, sizeof(int)*n);

	int q_c = 0;
	for(int i = 0;i < n;i ++) {
		int cnt = 0;
		for(Edge *edge = nodes[i].first;edge != NULL&&!edge->deleted;edge = edge->next) ++ cnt;
		if(cnt < K) {
			q[q_c] = i;
			++ q_c;
		}

		degrees[i] = cnt;
	}
	//printf("q_c %d\n", q_c);
	kcore_optimization(q_c);
	int max_l = 0, non_trivial = 0;

	for(int i = 0;i < n;i ++) {
		// if(i%10000 == 0) printf(".");

		if(computed[i]) continue;

		if(construct_pgraph(i, height) > 1) non_trivial = 1;

		std::vector<Element *> cc;
		decomposition(i, cc, max_l);

		if(cc.size() == 1) for(Element *e = cc[0];e != NULL;e = e->next) computed[e->value] = 1;
		else remove_inter_edges(cc);

		-- i;
	}

	int max_height = height[0];
	for(int i = 0;i < n;i ++) if(height[i] > max_height) max_height = height[i];

//	printf("K: %d, Height: %d, Max L: %d\n", K, max_height, max_l);

	return non_trivial;
}

void Graph::extract_mcc() {
	int cnt = 0;

	if(computed == NULL) computed = new int[n];
	memset(computed, 0, sizeof(int)*n);

	int q_c = 0;

	int sa, ss = -1;

	for(int i = 0;i < n;i ++) {
		if(computed[i]) continue;

		++ cnt;

		q[0] = i;
		q_c = 1;
		computed[i] = 1;

		for(int j = 0;j < q_c;j ++) {
			for(Edge *e = nodes[q[j]].first;e != NULL;e = e->next) if(!computed[e->node_id]) {
				computed[e->node_id] = 1;
				q[q_c++] = e->node_id;
			}
		}

		if(q_c > ss) {
			ss = q_c;
			sa = i;
		}
	}

	memset(computed, 0, sizeof(int)*n);

	q[0] = sa; computed[sa] = 1;
	q_c = 1;

	cnt = 0;
	for(int j = 0;j < q_c;j ++) {
		for(Edge *e = nodes[q[j]].first;e != NULL;e = e->next) {
			++ cnt;
			if(!computed[e->node_id]) {
				computed[e->node_id] = 1;
				q[q_c++] = e->node_id;
			}
		}
	}

	std::sort(q, q+q_c);

	int *id = new int[n];
	for(int i = 0;i < n;i ++) id[i] = -1;
	for(int i = 0;i < q_c;i ++) id[q[i]] = i;

	cnt /= 2;

	printf("n = %d, m = %d; nn = %d, nm = %d\n", n, m, q_c, cnt);
	//char *input = ""
	FILE *fout = open_file((dir + std::string("/edges_mcc.txt")).c_str(), "w");

	fprintf(fout, "%d %d\n", q_c, cnt);
	for(int i = 0;i < q_c;i ++) for(Edge *e = nodes[q[i]].first;e != NULL;e = e->next) if(e->node_id > q[i]) {
		if(id[q[i]] == id[e->node_id]) printf("WA in extract_mcc!\n");

		fprintf(fout, "%d %d\n",  id[q[i]], id[e->node_id]);
	}

	fclose(fout);

	delete[] id;
}

void Graph::output_k_edge_connected_subgraphs(int o, int sc_max, int sc_min) {
	std::ostringstream os;
	os<<dir<<"/decomposition_"<<K<<"_"<<o<<".txt";
	//os<<dir<<"/my.txt";
	if(K < sc_min){
		sc_min = K;
	}
	if(K > sc_max){
		sc_max = K;
	}
	FILE *fout = open_file(os.str().c_str(), "w");

	int cnt = 0, max_size = 0;

	memset(computed, 0, sizeof(int)*n);
	for(int i = 0;i < n;i ++) if(!computed[i]) {
		int q_c = 1;
		q[0] = i;
		computed[i] = 1;

		for(int j = 0;j < q_c;j ++) {
			int s = q[j];
			for(Edge *e = nodes[s].first;e != NULL&&!e->deleted;e = e->next) if(!computed[e->node_id]) {
				computed[e->node_id] = 1;
				q[q_c] = e->node_id;
				++ q_c;
			}
		}

		if(q_c == 1) continue;

		std::sort(q, q+q_c);
		fprintf(fout, "%d", q[0]);
		for(int j = 1;j < q_c;j ++) fprintf(fout, " %d", q[j]);
		fprintf(fout, "\n");

		++ cnt;
		if(q_c > max_size) max_size = q_c;
	}

	printf("%d %d\n", cnt, max_size);

	fclose(fout);
}

void Graph::all_SC_naive(int sc_max, int sc_min) {
	printf("Compute all SCs using naive approach!\n");
#ifdef _LINUX_
        struct timeval start, end;
        gettimeofday(&start, NULL);
#else
        int start, end;
        start = clock();
#endif
		int na = 99;
	for(int i = 1;i <= n;i ++) {
		if(i > 1) if(!kSC(i)) break;

		if(i > 1) output_k_edge_connected_subgraphs(na, sc_max, sc_min);

		for(int j = 0;j < n;j ++) {
			for(Edge* e = nodes[j].first;e != NULL;e = e->next) {
				if(!e->deleted) e->sc = i;
				else e->deleted = 0;
				nodes[j].last = e;
			}
		}
	}

#ifdef _LINUX_
        gettimeofday(&end, NULL);

        long long mtime, seconds, useconds;
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = seconds*1000000 + useconds;

        printf("all_SC_naive ltime: %lld\n", mtime);

#else
        end = clock();

        printf("all_SC_naive time: %d\n", end-start);
#endif

	printf("Here\n");
	
	std::string output_name = std::string(dir) + "/all-SC.bin";
	FILE *fout = open_file(output_name.c_str(), "wb");

	output_all_sc(fout);

	fclose(fout);
}

void Graph::output_all_sc(FILE *fout) {
	int *buf = new int[3*m];
	std::vector<std::pair<int,std::pair<int,int> > > vp;
	vp.reserve(m);

	FILE *ff = open_file((dir+std::string("/cg.txt")).c_str(), "w");

	fprintf(ff, "%d %d\n", n, m);

	fwrite(&n, sizeof(int), 1, fout);
	fwrite(&m, sizeof(int), 1, fout);

	int edge_c = 0;

	for(int i = 0;i < n;i ++) for(Edge *e = nodes[i].first;e != NULL;e = e->next) if(e->node_id > i) {
		vp.push_back(std::make_pair(e->sc, std::make_pair(i,e->node_id)));
	}

	std::sort(vp.begin(), vp.end());

	for(int i = (int)vp.size()-1;i >= 0;i --) {
		buf[edge_c] = vp[i].second.first; buf[edge_c+1] = vp[i].second.second; buf[edge_c+2] = vp[i].first;
		edge_c += 3;

		fprintf(ff, "%d %d %d\n", vp[i].second.first, vp[i].second.second, vp[i].first);
	}

	fclose(ff);

	if(edge_c/3 != m) printf("WA edge count in output_all_sc!\n");

	fwrite(buf, sizeof(int), edge_c, fout);

	/*for(int i = 0;i < n;i ++) {
		int cnt = 2;
		vector<pair<int, int> > vp;
		for(Edge *e = nodes[i].first;e != NULL;e = e->next) if(e->node_id > i) {
			vp.push_back(make_pair(e->node_id, e->sc));
			//buf[cnt] = e->node_id;
			//buf[cnt+1] = e->sc;
			//cnt += 2;
		}
		sort(vp.begin(), vp.end());
		for(int j = 0;j < (int)vp.size();j ++) {
			buf[cnt] = vp[j].first;
			buf[cnt+1] = vp[j].second;
			cnt += 2;
		}

		buf[0] = i; buf[1] = cnt/2-1;
		//fwrite(buf, sizeof(int), cnt, fout);

		for(int j = 0;j < (int)vp.size();j ++) fprintf(fout, "%d %d %d\n", i, vp[j].first, vp[j].second);
	}*/

	delete[] buf;
}

void Graph::optimization_tree(int output) {
	std::string input_name = std::string(dir) + "/mSPT.txt";
	FILE *fin = open_file(input_name.c_str(), "r");

	fscanf(fin, "%d%d", &n, &m);

	int *cid = new int[n];
	for(int i = 0;i < n;i ++) {
		int a, b;
		fscanf(fin, "%d%d", &a, &b);
		cid[i] = a;
	}

	int *a = new int[m];
	int *b = new int[m];
	int *c = new int[m];
	for(int i = 0;i < m;i ++) fscanf(fin, "%d%d%d", a+i, b+i, c+i);

	fclose(fin);

#ifdef _LINUX_
        struct timeval start, end;
        gettimeofday(&start, NULL);
#else
        int start, end;
        start = clock();
#endif

	int *weight = new int[n];
	std::vector<std::pair<int,int> > vp;
	vp.reserve(2*n);

	int *parent = new int[2*n];
	for(int i = 0;i < 2*n;i ++) parent[i] = i;

	if(degrees == NULL) degrees = new int[n];

	for(int i = 0;i < n;i ++) degrees[i] = 0;

	int *i_parent = new int[n];
	for(int i = 0;i < n;i ++) i_parent[i] = -1;
	
	if(pnodes == NULL) pnodes = new Node[n];
	if(pedges == NULL) pedges = new Edge[2*n];

	for(int i = 0;i < m;i ++) pnodes[i].first = pnodes[i].last = NULL;
	int edge_c = 0;
	
	for(int i = 0;i < m;i ++) {
		weight[i] = c[i];

		int pa = find_root(a[i], parent);
		int pb = find_root(b[i], parent);

		parent[n+i] = n+i;
		parent[pa] = n+i;
		parent[pb] = n+i;
		vp.push_back(std::make_pair(pa, n+i));
		vp.push_back(std::make_pair(pb, n+i));

		if(pa >= n) {
			++ degrees[pa-n];

			pedges[edge_c].node_id = i;
			pedges[edge_c].next = pnodes[pa-n].first;
			pnodes[pa-n].first = &pedges[edge_c];
			++ edge_c;

			pedges[edge_c].node_id = pa-n;
			pedges[edge_c].next = pnodes[i].first;
			pnodes[i].first = &pedges[edge_c];
			++ edge_c;
		}
		else i_parent[pa] = i;
		if(pb >= n) {
			++ degrees[pb-n];

			pedges[edge_c].node_id = i;
			pedges[edge_c].next = pnodes[pb-n].first;
			pnodes[pb-n].first = &pedges[edge_c];
			++ edge_c;

			pedges[edge_c].node_id = pb-n;
			pedges[edge_c].next = pnodes[i].first;
			pnodes[i].first = &pedges[edge_c];
			++ edge_c;
		}
		else i_parent[pb] = i;
	}

	delete[] a;
	delete[] b;
	delete[] c;

	int *levels = new int[2*m];
	int *weights = new int[2*m];
	int *id_pos = new int[m];

	for(int i = 0;i < m;i ++) id_pos[i] = -1;

	int n_levels = 0;

	for(int i = 0;i < m;i ++) if(degrees[i] == 0) {
		id_pos[i] = n_levels;
		weights[n_levels] = weight[i];
		levels[n_levels ++] = 0;

		DFS(-1, i, 0, pnodes, pedges, weight, n_levels, levels, id_pos, weights);
	}

	std::string output_name = std::string(dir) + "/mSPT-Opt.txt";
	FILE *fout = open_file(output_name.c_str(), "w");

	std::string output_bin = std::string(dir) + "/mSPT-Opt.bin";
	FILE *fout_bin = open_file(output_bin.c_str(), "wb");

	if(output) {
		fprintf(fout, "%d", n);
		for(int i = 0;i < n;i ++) fprintf(fout, " %d %d", cid[i], i_parent[i]);
		fprintf(fout, "\n");

		fprintf(fout, "%d", m);
		for(int i = 0;i < m;i ++) fprintf(fout, " %d", id_pos[i]);
		fprintf(fout, "\n");

		fwrite(&n, sizeof(int), 1, fout_bin);
		for(int i = 0;i < n;i ++) {
			fwrite(cid+i, sizeof(int), 1, fout_bin);
			fwrite(i_parent+i, sizeof(int), 1, fout_bin);
		}
		fwrite(&m, sizeof(int), 1, fout_bin);
		fwrite(id_pos, sizeof(int), m, fout_bin);
	}

	delete[] cid;
	delete[] weight;
	delete[] id_pos;
	delete[] i_parent;

	construct_LCA_index(fout, fout_bin, n_levels, levels, weights, output);

	//fprintf(fout, "%d %d %d\n", n, m, (int)vp.size());
	//for(int i = 0;i < m;i ++) fprintf(fout, "%d\n", weight[n+i]);
	//for(int i = 0;i < (int)vp.size();i ++) fprintf(fout, "%d %d\n", vp[i].first, vp[i].second);

	fclose(fout_bin);
	fclose(fout);

#ifdef _LINUX_
        gettimeofday(&end, NULL);

        long long mtime, seconds, useconds;
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = seconds*1000000 + useconds;

        printf("mSPTO ltime, %lld\n", mtime);

#else
        end = clock();

        printf("mSPTO time, %d\n", end-start);
#endif
	
	delete[] weights;
	delete[] levels;
}

int Graph::find_root(int x, int *parent) {
	int root = x;
	while(parent[root] != root) root = parent[root];

	while(parent[x] != root) {
		int tmp = parent[x];
		parent[x] = root;
		x = tmp;
	}

	return root;
}

void Graph::DFS(int pid, int id, int lid, const Node *pnodes, const Edge *pedges, int *weight, int &n_levels, int *levels, int *id_pos, int *weights) {
	Edge **s_e = new Edge*[n];
	int *pids = new int[n];
	int *ids = new int[n];
	int *lids = new int[n];

	s_e[0] = pnodes[id].first;
	pids[0] = pid;
	ids[0] = id;
	lids[0] = lid;

	int s_n = 0;

	while(s_n >= 0) {
		if(s_e[s_n] != NULL) {
			if(s_e[s_n]->node_id == pids[s_n]) {
				s_e[s_n] = s_e[s_n]->next;
				continue;
			}

			id_pos[s_e[s_n]->node_id] = n_levels;
			weights[n_levels] = weight[s_e[s_n]->node_id];
			levels[n_levels ++] = lids[s_n]+1;

			++ s_n;
			s_e[s_n] = pnodes[s_e[s_n-1]->node_id].first;
			pids[s_n] = ids[s_n-1];
			ids[s_n] = s_e[s_n-1]->node_id;
			lids[s_n] = lids[s_n-1]+1;
		}
		else {
			-- s_n;
			if(s_n >= 0) {
				weights[n_levels] = weight[ids[s_n]];
				levels[n_levels ++] = lids[s_n];

				s_e[s_n] = s_e[s_n]->next;
			}
		}
	}

	delete[] s_e;
	delete[] pids;
	delete[] ids;
	delete[] lids;

	/*
	for(Edge *e = pnodes[id].first;e != NULL;e = e->next) {
		if(e->node_id == pid) continue;

		id_pos[e->node_id] = n_levels;
		weights[n_levels] = weight[e->node_id];
		levels[n_levels ++] = lid+1;

		if(weight[e->node_id] < weight[id]) printf("WA in DFS\n");

		DFS(id, e->node_id, lid+1, pnodes, pedges, weight, n_levels, levels, id_pos, weights);

		weights[n_levels] = weight[id];
		levels[n_levels ++] = lid;
	}
	*/
}

void Graph::construct_LCA_index(FILE *fout, FILE *fout_bin, int n_levels, int *levels, int *weights, int output) {
	int logn = 1;
	while((1<<logn) < n_levels) ++ logn;
	logn /= 2;

	int newn = (n_levels+logn-1)/logn;
	int *min_levels = new int[newn];
	int *min_pos = new int[newn];

	for(int i = 0;i < newn;i ++) {
		min_levels[i] = n_levels*10;
		min_pos[i] = -1;
		for(int j = i*logn;j < (i+1)*logn&&j < n_levels;j ++) {
			if(levels[j] < min_levels[i]) {
				min_levels[i] = levels[j];
				min_pos[i] = j;
			}
		}
	}

	if(output) {
		fprintf(fout, "%d", n_levels);
		for(int i = 0;i < n_levels;i ++) fprintf(fout, " %d", weights[i]);
		fprintf(fout, "\n");

		fwrite(&n_levels, sizeof(int), 1, fout_bin);
		fwrite(weights, sizeof(int), n_levels, fout_bin);
	}

	int new_logn = 1;
	while((1<<new_logn) < newn) ++ new_logn;

	int **dp = new int*[new_logn+1];
	for(int i = 0;i <= new_logn;i ++) dp[i] = new int[newn];

	for(int i = 0;i < newn;i ++) dp[0][i] = min_pos[i];
	for(int i = 1;i <= new_logn;i ++) for(int j = 0;j < newn;j ++) {
		dp[i][j] = dp[i-1][j];
		if(j + (1<<(i-1)) < newn&&levels[dp[i-1][j + (1<<(i-1))]] < levels[dp[i][j]]) dp[i][j] = dp[i-1][j + (1<<(i-1))];
	}

	if(output) {
		fprintf(fout, "%d %d\n", new_logn+1, newn);
		for(int i = 0;i <= new_logn;i ++) {
			for(int j = 0;j < newn;j ++) fprintf(fout, "%d ", dp[i][j]);
			fprintf(fout, "\n");
		}

		int tmp = new_logn+1;
		fwrite(&tmp, sizeof(int), 1, fout_bin);
		fwrite(&newn, sizeof(int), 1, fout_bin);
		for(int i = 0;i <= new_logn;i ++) fwrite(dp[i], sizeof(int), newn, fout_bin);

		printf("LCA size: %d\n", (new_logn+1)*newn + (1<<logn)*(logn*logn));
	}

	int *hash = new int[(1<<logn)];
	for(int i = 0;i < (1<<logn);i ++) hash[i] = -1;

	int **table = new int*[(1<<logn)];
	for(int i = 0;i < (1<<logn);i ++) table[i] = new int[logn*logn];

	int n_table = 0;
	int *id_pos = new int[newn];

	//if(hash[3] != -1) printf("WAAA!\n");

	for(int i = 0;i < newn;i ++) {
		int bitset = 0;
		for(int j = i*logn+1;j < (i+1)*logn;j ++) {
			//bitset = (bitset<<1);
			if(j >= n_levels||levels[j] >= levels[j-1]) bitset = (bitset | (1<<(j-i*logn-1)));
		}

		if(hash[bitset] != -1) {
			id_pos[i] = hash[bitset];

			/*
			int *val = table[id_pos[i]];
			for(int j = i*logn;j < (i+1)*logn;j ++) for(int k = j;k < (i+1)*logn&&k < n_levels;k ++) {
				for(int ii = j;ii <= k;ii ++) if(levels[ii] < levels[val[(j-i*logn)*logn + k-i*logn]+i*logn]) {
					/*for(int jj = 0;jj < logn-1;jj ++) {
						if(bitset&(1<<jj)) printf("1 ");
						else printf("-1 ");
					}
					printf("\n");

					for(int jj = i*logn;jj < (i+1)*logn;jj ++) printf("%d ", levels[jj]);
					printf("\n");
					for(int jj = 0;jj < logn;jj ++) for(int kk = jj;kk < logn;kk ++) printf("[%d %d: %d], ", jj, kk, val[jj*logn+kk]);
					printf("\n");
					
					printf("WA1 %d!\n", bitset);

					return ;
				}
			}
			*/

			continue;
		}

		hash[bitset] = n_table;
		id_pos[i] = n_table;
		int *val = table[n_table];
		++ n_table;

		for(int j = i*logn;j < (i+1)*logn&&j < n_levels;j ++) {
			val[(j - i*logn)*logn + j - i*logn] = j - i*logn;
			for(int k = j+1;k < (i+1)*logn&&k < n_levels;k ++) {
				val[(j-i*logn)*logn + k-i*logn] = val[(j-i*logn)*logn + k-1-i*logn];
				if(levels[k] < levels[val[(j-i*logn)*logn + k-i*logn] + i*logn]) val[(j-i*logn)*logn + k-i*logn] = k - i*logn;
			}
		}

		/*
		if(bitset == 3) {
			int *val = table[id_pos[i]];
			{
					for(int jj = 0;jj < logn-1;jj ++) {
						if(bitset&(1<<jj)) printf("1 ");
						else printf("-1 ");
					}
					printf("\n");

					for(int jj = i*logn;jj < (i+1)*logn;jj ++) printf("%d ", levels[jj]);
					printf("\n");
					for(int jj = 0;jj < logn;jj ++) for(int kk = jj;kk < logn;kk ++) printf("[%d %d: %d], ", jj, kk, val[jj*logn+kk]);
					printf("\n");
					
					printf("bitset = %d!\n", bitset);
			}
		}
		*/

		//val = table[id_pos[i]];
		/*
		for(int j = i*logn;j < (i+1)*logn;j ++) for(int k = j;k < (i+1)*logn&&k < n_levels;k ++) {
			for(int ii = j;ii <= k;ii ++) if(levels[ii] < levels[val[(j-i*logn)*logn + k-i*logn]+i*logn]) {
				printf("WA2!\n");
			}
		}
		*/
	}

	if(output) {
		for(int i = 0;i < newn;i ++) fprintf(fout, "%d ", id_pos[i]);
		fprintf(fout, "\n");

		fprintf(fout, "%d %d\n", n_table, logn);
		for(int i = 0;i < n_table;i ++) {
			int *val = table[i];
			for(int j = 0;j < logn*logn;j ++) fprintf(fout, "%d ", val[j]);
			fprintf(fout, "\n");
		}

		fwrite(id_pos, sizeof(int), newn, fout_bin);
		fwrite(&n_table, sizeof(int), 1, fout_bin);
		fwrite(&logn, sizeof(int), 1, fout_bin);
		for(int i = 0;i < n_table;i ++) fwrite(table[i], sizeof(int), logn*logn, fout_bin);
	}

	delete[] id_pos;

	for(int i = 0;i < (1<<logn);i ++) {
		delete[] table[i];
		table[i] = NULL;
	}
	delete[] table;

	for(int i = 0;i <= new_logn;i ++) {
		delete[] dp[i];
		dp[i] = NULL;
	}
	delete[] dp;

	delete[] hash;

	delete[] min_levels;
	delete[] min_pos;
}

void Graph::max_spanning_tree() {
#ifdef _LINUX_
        struct timeval start, end;
        gettimeofday(&start, NULL);
#else
        int start, end;
        start = clock();
#endif

	for(int i = 0;i < n;i ++) pnodes[i].first = pnodes[i].last = NULL;
	int edge_c = 0;

	int max_sc = 0;

	for(int i = 0;i < n;i ++) for(Edge *e = nodes[i].first;e != NULL;e = e->next) {
		if(e->node_id > i) {
			if(e->sc > max_sc) max_sc = e->sc;

			pedges[edge_c].node_id = i;
			pedges[edge_c].sc = e->node_id;
			pedges[edge_c].next = NULL;
			
			if(pnodes[e->sc].first == NULL) pnodes[e->sc].first = pnodes[e->sc].last = &pedges[edge_c];
			else {
				pnodes[e->sc].last->next = &pedges[edge_c];
				pnodes[e->sc].last = &pedges[edge_c];
			}

			++ edge_c;
		}
	}

	int *parent = new int[n];
	int *rank = new int[n];

	std::vector<std::pair<std::pair<int,int>, int> > vp;
	vp.reserve(n);

	for(int i = 0;i < n;i ++) {
		parent[i] = i;
		rank[i] = 0;
	}

	for(int i = max_sc;i > 0;i --) {
		for(Edge *e = pnodes[i].first;e != NULL;e = e->next) {
			int pa = find_root(e->node_id, parent);
			int pb = find_root(e->sc, parent);

			if(pa == pb) continue;

			vp.push_back(std::make_pair(std::make_pair(e->node_id, e->sc), i));

			if(rank[pa] < rank[pb]) parent[pa] = pb;
			else if(rank[pa] > rank[pb]) parent[pb] = pa;
			else {
				parent[pa] = pb;
				++ rank[pb];
			}
		}
	}

	for(int i = 0;i < n;i ++) pnodes[i].first = pnodes[i].last = NULL;

	edge_c = 0;
	for(int i = 0;i < (int)vp.size();i ++) {
		int a = vp[i].first.first, b = vp[i].first.second;

		pedges[edge_c].node_id = b;
		pedges[edge_c].next = pnodes[a].first;
		pnodes[a].first = &pedges[edge_c];
		++ edge_c;

		pedges[edge_c].node_id = a;
		pedges[edge_c].next = pnodes[b].first;
		pnodes[b].first = &pedges[edge_c];
		++ edge_c;
	}

	int c_id = 0;
	int *c_ids = new int[n];
	int *levels = new int[n];

	if(computed == NULL) computed = new int[n];

	memset(computed, 0, sizeof(int)*n);

	//printf("27:");
	//for(Edge *e = pnodes[27].first;e != NULL;e = e->next) printf(" %d", e->node_id);
	//printf("\n");

	for(int i = 0;i < n;i ++) {
		if(computed[i]) continue;

		int q_c = 1;
		q[0] = i;
		levels[i] = 0;
		c_ids[i] = c_id;

		computed[i] = 1;
		
		for(int j = 0;j < q_c;j ++) {
			for(Edge *e = pnodes[q[j]].first;e != NULL;e = e->next) if(!computed[e->node_id]) {
				//if(q[j] == 27||e->node_id == 27) printf("Edge %d %d %d\n", q[j], e->node_id, levels[q[j]]);
				computed[e->node_id] = 1;
				q[q_c ++] = e->node_id;
				levels[e->node_id] = levels[q[j]]+1;
				c_ids[e->node_id] = c_id;
			}
		}

		++ c_id;
	}

#ifdef _LINUX_
        gettimeofday(&end, NULL);

        long long mtime, seconds, useconds;
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = seconds*1000000 + useconds;

        printf("mSPT ltime: %lld\n", mtime);

#else
        end = clock();

        printf("mSPT time: %d\n", end-start);
#endif

	std::string output_name = std::string(dir) + "/mSPT.txt";
	FILE *fout = open_file(output_name.c_str(), "w");

	fprintf(fout, "%d %d\n", n, vp.size());
	//if(vp.size() != n-1) printf("WA in tree size!\n");

	for(int i = 0;i < n;i ++) fprintf(fout, "%d %d\n", c_ids[i], levels[i]);

	for(int i = 0;i < (int)vp.size();i ++) fprintf(fout, "%d %d %d\n", vp[i].first.first, vp[i].first.second, vp[i].second);

	fclose(fout);

	delete[] c_ids;
	delete[] levels;
	delete[] parent;
	delete[] rank;
}

void Graph::read_mSPTO() {
	std::string input_name = dir + "/mSPT-Opt.txt";
	FILE *fin = open_file(input_name.c_str(), "r");

	fscanf(fin, "%d", &n);

	int max_id = 1547493;
	
	if(cids == NULL) cids = new int[max_id];
	if(pos_id == NULL) pos_id = new int[max_id];
	if(parent == NULL) parent = new int[max_id];
	
	for(int i = 0;i < n;i ++) fscanf(fin, "%d%d", cids+i, pos_id+i);

	fscanf(fin, "%d", &m);
	for(int i = 0;i < m;i ++) fscanf(fin, "%d", parent+i);

	for(int i = 0;i < n;i ++) pos_id[i] = parent[pos_id[i]];

	fscanf(fin, "%d", &weights_n);
	if(weights == NULL) weights = new int[weights_n];
	for(int i = 0;i < weights_n;i ++) fscanf(fin, "%d", &weights[i]);

	fscanf(fin, "%d %d", &logns, &ns);
	if(st == NULL) {
		st = new int*[logns];
		for(int i = 0;i < logns;i ++) st[i] = new int[ns];
	}

	for(int i = 0;i < logns;i ++) for(int j = 0;j < ns;j ++) {
		fscanf(fin, "%d", &st[i][j]);
		st[i][j] = weights[st[i][j]];
	}

	if(t_id == NULL) t_id = new int[ns];
	for(int i = 0;i < ns;i ++) fscanf(fin, "%d", &t_id[i]);

	fscanf(fin, "%d %d", &nt, &logn);
	if(table == NULL) {
		table = new int*[nt];
		for(int i = 0;i < nt;i ++) table[i] = new int[logn*logn];
	}
	for(int i = 0;i < nt;i ++) for(int j = 0;j < logn*logn;j ++) fscanf(fin, "%d", &table[i][j]);

	fclose(fin);

	if(log_table == NULL) log_table = new int[ns+1];

	log_table[0] = 0;
	for(int i = 0;i <= logns;i ++) for(int j = (1<<i);j < (1<<(i+1))&&j <= ns;j ++) log_table[j] = i;
}

int Graph::sc_query_mSPTO(int q_n, int *qq) {
	int res = 1000000;
	for(int i = 1;i < q_n;i ++) {
		int tmp = mSPTO_P2P(qq[0], qq[i]);
		if(tmp < res) res = tmp;
	}
	return res;
}

int Graph::mSPTO_P2P(int a, int b) {
	int ti = pos_id[a];
	int tj = pos_id[b];

	if(ti < 0 ||tj < 0){
		return 0;
	}
	if(ti > tj) std::swap(ti, tj);

	int res = n*2;

	int li = ti/logn, lj = tj/logn, tmp;

	if(li == lj) {
		res = weights[table[t_id[li]][(ti%logn)*logn + tj%logn] + li*logn];
		return res;
	}

	if(lj > li+1) {
		int x = log_table[lj-li-1];
		int *val = st[x];
		if((tmp = val[li+1]) < res) res = tmp;
		if((tmp = val[lj-(1<<x)]) < res) res = tmp;
	}

	ti %= logn; tj %= logn;
	if((tmp = weights[table[t_id[li]][ti*logn + logn-1] + li*logn]) < res) res = tmp;
	if((tmp = weights[table[t_id[lj]][tj] + lj*logn]) < res) res = tmp;

	return res;
}

void Graph::sc(const char *file_name, int method) {
	std::string input_name = dir + "/" + file_name + ".txt";
	FILE *fin = open_file(input_name.c_str(), "r");

	std::vector<std::vector<int> > query;
	TIntV sc;
	//std::vector<int> sc;

	if(computed == NULL) computed = new int[n];
	if(q == NULL) q = new int[n];
	for(int i = 0;i < n;i ++) computed[i] = 0;

	int t;
	fscanf(fin, "%d", &t);
	for(int i = 0;i < t;i ++) {
		int d;
		fscanf(fin, "%d", &d);
		std::vector<int> tmp;
		while(d --) {
			int tt;
			fscanf(fin, "%d", &tt);
			tmp.push_back(tt);
		}
		query.push_back(tmp);
	}
	fclose(fin);

#ifdef _LINUX_
	struct timeval start, end;
	gettimeofday(&start, NULL);
#else
	int start, end;
	start = clock();
#endif

	int qq[40], q_n;
	printf("here1");
	for(int i = 0;i < t;i ++) {
		std::vector<int> &tmp = query[i];
		q_n = tmp.size();
		for(int j = 0;j < q_n;j ++) qq[j] = tmp[j];

		//if(method == 0) smcc_query_kSC(q_n, qq, 0);
		//else if(method == 1) sc_query_mSPT(q_n, qq);
		int q_sc;
		if(method == 2) {
			q_sc = sc_query_mSPTO(q_n, qq);
			sc.Add(q_sc);
		}
		else {
			printf("WA in sc\n");
		}
	}
	printf("here2");
	std::string output = "../DBLP/steiner_tree_connectivity.txt";
	FILE *fout = open_file(output.c_str(), "w");
	fprintf(fout, "%d\n", t);
	while(t > 0){
		int max = sc.GetMxValN();//最大connectivity所在位置
		int scv = sc.GetVal(max);
		fprintf(fout, "%d ", scv);//connectivity
		std::vector<int> &tmp = query[max];
		q_n = tmp.size();
		fprintf(fout, "%d ", q_n);//length
		for(int q = 0; q < q_n; q++){
			qq[q] = tmp[q];
			fprintf(fout, "%d ", qq[q]);//nodes
		}
		fprintf(fout, "\n");
		
		query.erase(std::remove(query.begin(), query.end(), tmp), query.end());
		sc.Del(max);// Removes the element at position \c ValN.
		t--;
	}
	fclose(fout);

#ifdef _LINUX_
	gettimeofday(&end, NULL);

	long long mtime, seconds, useconds;
	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = seconds*1000000 + useconds;

	printf("smcc_%d ltime: %lld\n", method, mtime); 

#else
	end = clock();

	printf("smcc_%d time: %d\n", method, end-start);
#endif
}

//计算有多少个连通图，并输出
void Graph::count_cc(int k) {
	int cnt = 0;

	if(computed == NULL) computed = new int[n];
	memset(computed, 0, sizeof(int)*n);

	std::ostringstream out;
	out<<"../DBLPktruss/"<<k<<".txt";
	FILE *fout = open_file(out.str().c_str(), "w");

	int q_c = 0;

	for(int i = 0;i < n;i ++) {
		if(computed[i]) continue;

		++ cnt;

		q[0] = i;
		q_c = 1;
		computed[i] = 1;

		for(int j = 0;j < q_c;j ++) {
			for(Edge *e = nodes[q[j]].first;e != NULL;e = e->next) if(!computed[e->node_id]) {
				computed[e->node_id] = 1;
				q[q_c++] = e->node_id;
			}
		}

		std::sort(q, q+q_c);
		fprintf(fout, "%d", q[0]);
		for(int j = 1;j < q_c;j ++) fprintf(fout, " %d", q[j]);
		fprintf(fout, "\n");
	}

	memset(computed, 0, sizeof(int)*n);

	printf("Number of connected components: %d\n", cnt);
}

/*void Graph::output_k_edge_connected_subgraphs_refine(int o) {
	std::ostringstream os;
	os<<dir<<"/decomposition_refine_"<<K<<"_"<<o<<".txt";
	//os<<dir<<"/my.txt";
	FILE *fout = open_file(os.str().c_str(), "w");

	int cnt = 0, max_size = 0;

	memset(computed, 0, sizeof(int)*n);
	for(int i = 0;i < n;i ++) if(!computed[i]) {
		int q_c = 1;
		q[0] = i;
		computed[i] = 1;

		for(int j = 0;j < q_c;j ++) {
			int s = q[j];
			for(Edge *e = nodes[s].first;e != NULL&&!e->deleted;e = e->next) if(!computed[e->node_id]) {
				computed[e->node_id] = 1;
				q[q_c] = e->node_id;
				++ q_c;
			}
		}

		if(q_c == 1) continue;

		std::sort(q, q+q_c);
		fprintf(fout, "%d", q[0]);
		for(int j = 1;j < q_c;j ++) fprintf(fout, " %d", q[j]);
		fprintf(fout, "\n\n");

		++ cnt;
		if(q_c > max_size) max_size = q_c;
	}

	printf("%d %d\n", cnt, max_size);

	fclose(fout);
}*/