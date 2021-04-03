#include "local_search.h"

int compare_word(const void *w1, const void *w2) {
	Word *a = *(Word**)w1;
	Word *b = *(Word**)w2;
	return a->st.compare( b->st );
}

int compare_word_freq(const void *w1, const void *w2) {
	Word *a = *(Word**)w1;
	Word *b = *(Word**)w2;
	return (int)(a->v_node.size() - b->v_node.size());
}

int *S::n = new int[1<<KEY_MAX];
vector<int> *S::v = new vector<int>[1<<KEY_MAX];
int *S::ns = new int[1<<KEY_MAX];
vector<int> *S::vs = new vector<int>[1<<KEY_MAX];

void S::init(){
	memset( n, 0, sizeof(int) * (1<<KEY_MAX) );
	memset( ns, 0, sizeof(int) * (1<<KEY_MAX) );

	for( int i = 1; i < (1<<KEY_MAX); ++i ) {
		for( int j = 0; j < KEY_MAX; ++j )
			if( i & (1<<j) ) {
				++n[i];
				v[i].push_back(j);
			}
		for( int j = 1; j <= i; ++j )
			if( (i & j) == j ) {
				++ns[i];
				vs[i].push_back(j);
			}
	}
}

void S::clear() {
	delete[] n;
	delete[] v;
}

Word::Word(string st) {
	this->st = st;
	pos = 0;
	tlen = 0;
}

void Application::create_node_index( string path ) {
	printf( "Creating node index\n" );
	string st_node = path + "node.txt";
	string st_str = path + "node_dat.bin";
	string st_ind = path + "node_ind.bin";
	string st_inf = path + "node_inf.txt";
	FILE *fin = fopen( st_node.c_str(), "r" );
	FILE *f_str = fopen( st_str.c_str(), "wb" );
	FILE *f_ind = fopen( st_ind.c_str(), "wb" );
	FILE *f_inf = fopen( st_inf.c_str(), "w" );

	char st1[MAXST], st2[MAXST], st3[MAXST];

	int pos = 0;
	int i;
	int cnt = 0;
	while( 1 ) {
		if( fgets( st1, MAXST, fin )==NULL )
			break;
		fgets( st2, MAXST, fin );
		i = (int)strlen( st2 );
		if( st2[i-1] == '\n' ) {
			st2[i-1] = '\0';
			--i;
		}
		fgets( st3, MAXST, fin );
		fwrite( &pos, sizeof(int), 1, f_ind );
		fwrite( &i, sizeof(int), 1, f_ind );
		fwrite( st2, sizeof(char), i, f_str );
		pos += i * sizeof(char);
		++cnt;
	}
	fprintf( f_inf, "%d\n", cnt );
	printf( "##%d\n", cnt );

	fclose( f_ind );
	fclose( f_str );
	fclose( fin );
	fclose( f_inf );
}

int Application::get_n_node( string path ) {
	int p;
	string st_inf = path + "node_inf.txt";
	FILE *fin = fopen( st_inf.c_str(), "r" );
	fscanf( fin, "%d", &p );
	fclose( fin );
	return p;
}

int Application::get_n_edge( string path ) {
	int p;
	string st_inf = path + "edge_inf.txt";
	FILE *fin = fopen( st_inf.c_str(), "r" );
	fscanf( fin, "%d", &p );
	fclose( fin );
	return p;
}

void Application::create_edge_index( string path ) {
	int cnt = 0;
	printf( "Creating edge index\n" );
	string st_dat = path + "edge_dat.bin";
	string st_inf = path + "edge_inf.txt";
	string st_edge = path + "edge.txt";
	FILE *fin = fopen( st_edge.c_str(), "r" );
	FILE *f_dat = fopen( st_dat.c_str(), "wb" );
	FILE *f_inf = fopen( st_inf.c_str(), "w" );

	int n = get_n_node( path );
	vector<int>* e = new vector<int>[n];
	int u, v;

	while( fscanf( fin, "%d %d", &u, &v ) != EOF ) {
		e[u].push_back( v );
		e[v].push_back( u );
	}

	for( int i = 0; i < n; ++i )
		for( int j = 0; j < (int) e[i].size(); ++j ) {
			int v = e[i][j];
			fwrite( &v, sizeof(int), 1, f_dat );
			++cnt;
		}

	fprintf( f_inf, "%d\n", cnt/2 );
	fclose( fin );
	fclose( f_dat );
	fclose( f_inf );

	string st_ind = path + "edge_ind.bin";
	FILE *f_ind = fopen( st_ind.c_str(), "wb" );

	for( int i = 0; i < n; ++i ) {
		int len = (int) e[i].size();
		fwrite( &len, sizeof(int), 1, f_ind );
	}
	fclose( f_ind );

	delete[] e;

	printf( "##%d\n", cnt/2 );
}

void Application::create_word_index( string path ) {
	FILE *fnode = fopen( (path + "node.txt").c_str(), "r" );
	printf( "Creating Inverted Index...\n" );

	char st1[MAXST], st2[MAXST], st3[MAXST];
	int cnt = 0;

	vector<Word*> v_word;
	map<string, Word*> m_word;

	while( 1 ) {
		if( fgets( st1, MAXST, fnode ) == NULL )
			break;
		fgets( st2, MAXST, fnode );
		fgets( st3, MAXST, fnode );
		int id;
		sscanf( st1, "%d", &id );

		int len = (int)strlen( st2 );
		if( st2[len-1] == '\n' )
			st2[--len] = '\0';

		add_st( st2, id, v_word, m_word );
		if( ++cnt % 100000 == 0 )
			printf( "%d\n", cnt );

	}
	fclose( fnode );


	int n_word = (int) v_word.size();
	printf( "Sorting %d Words...\n", n_word );

	Word** l_word = new Word*[n_word];
	for( int i = 0; i < n_word; ++i )
		l_word[i] = v_word[i];
	qsort( l_word, n_word, sizeof(Word*), compare_word );

	printf( "Creating word_dat.bin...\n" );
	FILE *f_word_node = fopen( (path + "word_dat.bin").c_str(), "wb" );

	int pos = 0;
	int tlen = 0;
	for( int i = 0; i < n_word; ++i ) {
		Word* w = l_word[i];
		w->pos = pos;
		w->tlen = tlen;
		int len = (int) w->v_node.size();
		fwrite( &len, sizeof(int), 1, f_word_node );
		for( int j = 0; j < len; ++j )
			fwrite( &w->v_node[j], sizeof(int), 1, f_word_node );
		++len;
		tlen += w->st.size();
		pos += len * sizeof(int);
	}

	fclose( f_word_node );

	printf( "Creating word_ind.bin...\n" );
	FILE *f_word_ind = fopen( (path + "word_ind.bin").c_str(), "wb" );
	int now_pos = 0;
	save_word_ind_bin( l_word, f_word_ind, now_pos, 0, n_word-1 );
	fclose( f_word_ind );

	printf( "Sorting word freq...\n" );
	qsort( l_word, n_word, sizeof(Word*), compare_word_freq );

	printf( "Creating word_freq.txt...\n" );
	FILE *f_word_freq = fopen( (path + "word_freq.txt").c_str(), "w" );
	for( int i = 0; i < n_word; ++i ) {
		Word *w = l_word[i];
		fprintf( f_word_freq, "%d %s %d\n", i+1, w->st.c_str(), (int)w->v_node.size() );
	}
	fclose( f_word_freq );

	for( int i = 0; i < n_word; ++i )
		delete l_word[i];
	delete[] l_word;
}

void Application::find_word_node( string path, string w, vector<int> &v_node ) {
	int pos = -1, l, r, len, now = 0;
	char st[MAXST];
	bool succ = false;
	FILE *fin = fopen( (path+"word_ind.bin").c_str(), "rb" );
	while( true ) {
		fseek( fin, now, SEEK_SET );
		fread( &len, sizeof(int), 1, fin );
		fread( st, sizeof(char), len, fin );
		fread( &pos, sizeof(int), 1, fin );
		fread( &l, sizeof(int), 1, fin );
		fread( &r, sizeof(int), 1, fin );

		st[len] = '\0';
		int cmp = strcmp( w.c_str(), st );
		if( cmp == 0 ) {
			succ = true;
			break;
		} else
			now = cmp < 0 ? l : r;
		if( now == 0 )
			break;
	}
	fclose( fin );

	v_node.clear();
	if( !succ )
		return;

	fin = fopen( (path+"word_dat.bin").c_str(), "rb" );
	fseek( fin, pos, SEEK_SET );
	fread( &len, sizeof(int), 1, fin );
	for( int i = 0; i < len; ++i ) {
		fread( &now, sizeof(int), 1, fin );
		v_node.push_back( now );
	}
	fclose( fin );
}

void Application::find_term_node( string path, string term, vector<int> &v_node ) {
	char st[MAXST];
	strcpy( st, term.c_str() );
	int len = (int)term.size();
	for( int i = 0; i < len; ++i ) {
		st[i] = tolower( st[i] );
		if( isspace(st[i]) )
			st[i] = '\0';
	}

	vector<string> v_word;
	for( int i = 0; i < len; ++i ) {
		if( st[i] != '\0' )
			if( i == 0 || st[i-1] == '\0' )
				v_word.push_back( st + i );
	}

	v_node.clear();
	if( v_word.size() > 0 ) {
		find_word_node( path, v_word[0], v_node );
		for( int i = 1; i < (int) v_word.size(); ++i ) {
			vector<int> v_tmp;
			find_word_node( path, v_word[i], v_tmp );
			union_node_set( v_node, v_tmp );
		}
	}
}

string Application::find_node_st( string path, int id ) {
	FILE* fin = fopen( (path + "node_ind.bin").c_str(), "rb" );
	int pos = 0, len = 0;
	fseek( fin, id*sizeof(int)*2, SEEK_SET );
	fread( &pos, sizeof(int), 1, fin );
	fread( &len, sizeof(int), 1, fin );
	fclose(fin);

	len = min(len, MAXST-1);
	char* st = new char[MAXST];
	fin = fopen( (path+"node_dat.bin").c_str(), "rb" );
	fseek( fin, pos, SEEK_SET );
	fread( st, sizeof(char), len, fin );
	st[len] = '\0';
	fclose( fin );

	string rst = string(st);
	delete[] st;
	return rst;
}

void Application::save_word_ind_bin( Word** l_word, FILE* f_word_ind, int &now_pos, int s, int t ) {
	int mid = (s+t)/2;
	int tmp = (int)l_word[mid]->st.size();
	fwrite( &tmp, sizeof(int), 1, f_word_ind );
	fwrite( l_word[mid]->st.c_str(), sizeof(char), tmp, f_word_ind );
	fwrite( &l_word[mid]->pos, sizeof(int), 1, f_word_ind );
	now_pos += sizeof(int) + sizeof(char) * tmp + sizeof(int) + sizeof(int) * 2;

	tmp = (mid==s) ? 0 : now_pos;
	fwrite( &tmp, sizeof(int), 1, f_word_ind );

	tmp = (mid==t) ? 0 : (now_pos + (l_word[mid]->tlen-l_word[s]->tlen) + sizeof(int) * (mid-s) * 4 );
	fwrite( &tmp, sizeof(int), 1, f_word_ind );

	if( mid != s )
		save_word_ind_bin( l_word, f_word_ind, now_pos, s, mid - 1 );
	if( mid != t )
		save_word_ind_bin( l_word, f_word_ind, now_pos, mid + 1, t );
}

void Application::union_node_set( vector<int> &v_rst, vector<int> &v_node ) {
	int p1 = 0, p2 = 0, p = 0;
	while( p1 < (int) v_rst.size() && p2 < (int) v_node.size() ) {
		if( v_rst[p1] < v_node[p2] )
			++p1;
		else if( v_rst[p1] > v_node[p2] )
			++p2;
		else {
			v_rst[p++] = v_rst[p1];
			++p1;
			++p2;
		}
	}
	v_rst.resize(p);
}

void Application::add_st( char *st, int id, vector<Word*> &v_word, map<string, Word*> &m_word ) {
	int len = (int) strlen( st );
	for( int i = 0; i < len; ++i )
		if( !(st[i]>='a' && st[i]<='z') && !(st[i]>='A' && st[i]<='Z') && st[i]!= '-' && !(st[i]>='0' && st[i] <= '9'))
			st[i] = '\0';
		else if( st[i] >= 'A' && st[i] <= 'Z' )
			st[i] = st[i] - 'A' + 'a';

	for( int i = 0; i < len; ++i )
		if( st[i] && (i == 0 || st[i-1] == '\0') ) {
			string str( st + i );
			Word *w = m_word[str];
			if( w == NULL ) {
				w = new Word(str);
				v_word.push_back(w);
				m_word[str] = w;
			}
			int last = -1;
			if( w->v_node.size() > 0 )
				last = w->v_node[w->v_node.size()-1];
			if( id != last )
				w->v_node.push_back( id );
		}
}


//======PEntry======
PEntry::PEntry() {
	a = 0;
	b = 0;
	set = 0;
	idx = 0;
	cost = 0;
	info = 0;
}

//======KeyInfo======
KeyInfo::KeyInfo(string key) {
	this->key = key;
	n_node = 0;
}

//======PEntryHeap======
PEntryHeap::PEntryHeap( DisOracle *d ) {
	this->l = (int) d->n_key;
	tab = new PEntry***[l];
	tab_a = new PEntry**[l];
	for( int i = 0; i < l; ++i ) {
		tab[i] = new PEntry**[l];
		tab_a[i] = new PEntry*[1<<l];
		memset( tab_a[i], 0, sizeof(PEntry*) * (1<<l) );

		for( int j = 0; j < l; ++j ) {
			tab[i][j] = new PEntry*[1<<l];
			memset( tab[i][j], 0, sizeof(PEntry*) * (1<<l) );
		}
	}

	heap.push_back( NULL );
	tot = 0;

	v_block.push_back( new PEntry[n_block_pentry] );
	v_pos.push_back( 0 );

	init( d );
}

PEntryHeap::~PEntryHeap() {
	for( int i = 0; i < l; ++i ) {
		delete[] tab_a[i];
		for( int j = 0; j < l; ++j )
			delete[] tab[i][j];
		delete[] tab[i];
	}
	delete[] tab;
	delete[] tab_a;

	for( int i = 0; i < (int)v_block.size(); ++i )
		delete[] v_block[i];
}

PEntry* PEntryHeap::get_entry() {
	int t = (int) v_block.size() - 1;
	if( v_pos[t] == n_block_pentry ) {
		v_block.push_back( new PEntry[n_block_pentry] );
		v_pos.push_back( 0 );
		++t;
	}
	return v_block[t] + (v_pos[t]++);
}

int PEntryHeap::get_dis( int a, int b, int set ) {
	if( tab[a][b][set] == NULL )
		return INTINF;
	return tab[a][b][set]->cost;
}

int PEntryHeap::get_dis( int a, int set ) {
	if( tab_a[a][set] == NULL )
		return INTINF;
	return tab_a[a][set]->cost;
}

void PEntryHeap::up( int p ) {
	PEntry* tmp = heap[p];
	for( int i = p >> 1; i >= 1; i >>= 1 ) {
		if( heap[i]->cost < tmp->cost )
		  break;
		heap[p] = heap[i];
		heap[p]->idx = p;
		p = i;
	}

	heap[p] = tmp;
	heap[p]->idx = p;
}

void PEntryHeap::down( int p ) {
	PEntry* tmp = heap[p];
	for( int i = p << 1; i <= tot; i <<= 1 ) {
		if( i + 1 <= tot && heap[i]->cost > heap[i + 1]->cost )
		  ++i;
		if( heap[i]->cost > tmp->cost )
		  break;
		heap[p] = heap[i];
		heap[p]->idx = p;
		p = i;
	}
	heap[p] = tmp;
	heap[p]->idx = p;
}

void PEntryHeap::enheap( int a, int b, int set, int cost ) {
	PEntry *e = get_entry();
	e->a = a;
	e->b = b;
	e->set = set;
	e->cost = cost;

	++tot;
	if( tot == heap.size() )
		heap.push_back( e );
	else
		heap[tot] = e;
	e->idx = tot;
	up(tot);

	tab[a][b][set] = e;
}

void PEntryHeap::update( PEntry *e ) {
	up( e->idx );
}

PEntry* PEntryHeap::deheap() {
	if( tot == 0 )
		return NULL;
	PEntry* p = heap[1];
	heap[1] = heap[tot--];
	if( tot > 0 ) {
		heap[1]->idx = 1;
		down(1);
	}
	p->info = 1;
	return p;
}

bool PEntryHeap::empty() {
	return tot == 0;
}

void PEntryHeap::update( int a, int b, int set, int cost ) {
	PEntry *e = tab[a][b][set];
	if( e == NULL )
		enheap( a, b, set, cost );
	else if( e->info == 1 )
		return;
	else if( cost < e->cost ) {
		e->cost = cost;
		update( e );
	}
}

void PEntryHeap::init( DisOracle *d ) {
	for( int i = 0; i < l; ++i )
		enheap( i, i, 1<<i, 0 );

	while( !empty() ) {
		PEntry *e = deheap();
		//printf( "%d %d {%d} %d\n", e->a, e->b, e->set, e->cost );
		for( int i = 0; i < l; ++i )
			if( (!(e->set & (1<<i))) || i == e->a ) {
			//if(!(e->set & (1<<i))) {
				//int cost = e->cost + g->nodes[vu[e->b]].dij->get_dis(vu[i]);
				//int cost = e->cost + v_dij[e->b]->get_dis_key(i);
				int cost = e->cost + d->get_dis_key(e->b, i);
				update( e->a, i, e->set|(1<<i), cost );
			}
	}

	for( int i = 0; i < l; ++i )
		for( int s = 0; s < (1<<l); ++s )
			if( s & (1<<i) )
				for( int j = 0; j < l; ++j )
					if( (s&(1<<j)) && tab[i][j][s] != NULL ) {
					if( tab_a[i][s] == NULL )
						tab_a[i][s] = tab[i][j][s];
					else if( tab[i][j][s]->cost < tab_a[i][s]->cost)
						tab_a[i][s] = tab[i][j][s];
				}
}

//======Entry======
Entry::Entry() {
	set = 0;
	p = 0;
	idx = 0;
	cost = 0;
	val = 0;
	info = 0;
	next = NULL;
}

//=====EntryHeap=====
EntryHeap::EntryHeap() {
	v_block.push_back( new Entry[n_block_entry] );
	v_pos.push_back( 0 );
	tot = 0;
	len_hash = init_hash_len;
	n_hash = 0;
	hash = new Entry*[len_hash];
	memset( hash, 0, sizeof(Entry*) * len_hash );
	heap.push_back( NULL );
}

EntryHeap::~EntryHeap() {
	for( int i = 0; i < (int)v_block.size(); ++i )
		delete[] v_block[i];
	if( hash != NULL )
		delete[] hash;
}

Entry* EntryHeap::get_entry() {
	int t = (int) v_block.size() - 1;
	if( v_pos[t] == n_block_entry ) {
		v_block.push_back( new Entry[n_block_entry] );
		v_pos.push_back( 0 );
		++t;
	}

	return v_block[t] + (v_pos[t]++);
}

int EntryHeap::h( int p, int set, int label ) {
	unsigned u = label;
	u = u * 37 + p;
	u = u * 71 + set;
	return u % len_hash;
}

int EntryHeap::h( Entry* e ) {
	return h(e->p, e->set, e->info & ((1<<N_LABEL_BIT)-1) );
}

void EntryHeap::add( Entry *e ) {
	if( n_hash == len_hash ) {
		int old_len = len_hash;
		len_hash = len_hash * 2 + 1;
		Entry **new_hash = new Entry*[len_hash];
		memset( new_hash, 0, sizeof(Entry*) * len_hash );
		for( int i = 0; i < old_len; ++i ) {
			for( Entry *nowe = hash[i]; nowe != NULL; ) {
				Entry *nexte = nowe->next;
				int nowh = h(nowe);
				nowe->next = new_hash[nowh];
				new_hash[nowh] = nowe;
				nowe = nexte;
			}
		}
		delete[] hash;
		hash = new_hash;
	}

	++n_hash;
	int nowh = h(e);
	e->next = hash[nowh];
	hash[nowh] = e;
}

Entry *EntryHeap::find( int p, int set, int label ) {
	int nowh = h(p, set, label);
	for( Entry *e = hash[nowh]; e != NULL; e = e->next )
		if( e->p == p && e->set == set && (e->info & ((1<<N_LABEL_BIT)-1)) == label )
			return e;
	return NULL;
}

int EntryHeap::len() {
	return n_hash;
}

void EntryHeap::up( int p ) {
	Entry* tmp = heap[p];
	for( int i = p >> 1; i >= 1; i >>= 1 ) {
		if( heap[i]->cost < tmp->cost )
		  break;
		heap[p] = heap[i];
		heap[p]->idx = p;
		p = i;
	}

	heap[p] = tmp;
	heap[p]->idx = p;
}

void EntryHeap::down( int p ) {
	Entry* tmp = heap[p];
	for( int i = p << 1; i <= tot; i <<= 1 ) {
		if( i + 1 <= tot && heap[i]->cost > heap[i + 1]->cost )
		  ++i;
		if( heap[i]->cost > tmp->cost )
		  break;
		heap[p] = heap[i];
		heap[p]->idx = p;
		p = i;
	}
	heap[p] = tmp;
	heap[p]->idx = p;
}

Entry* EntryHeap::enheap( int p, int set, int label, int val, int cost ) {
	Entry *e = get_entry();
	e->p = p;
	e->set = set;
	e->info = label;
	e->val = val;
	e->cost = cost;

	++tot;
	if( tot == heap.size() )
		heap.push_back( e );
	else
		heap[tot] = e;
	e->idx = tot;
	up(tot);

	add( e );
	return e;
}

void EntryHeap::update( Entry *e ) {
	up( e->idx );
}

Entry* EntryHeap::deheap() {
	if( tot == 0 )
		return NULL;
	Entry* p = heap[1];
	heap[1] = heap[tot--];
	if( tot > 0 ) {
		heap[1]->idx = 1;
		down(1);
	}
	p->info |= (1<<(USED_BIT-1));
	return p;
}

bool EntryHeap::empty() {
	return tot == 0;
}

//=====Node=====
Node::Node(){
	len = 0;
	adj = NULL;
	dis = NULL;
}

//=====Graph=====

Graph::Graph() {
	n = 0;
	m = 0;
	adj = NULL;
	dis = NULL;
	nodes = NULL;
}

Graph::~Graph() {
	if( adj != NULL )
		delete[] adj;
	if( dis != NULL )
		delete[] dis;
	if( nodes != NULL )
		delete[] nodes;
}

void Graph::print() {
	for( int i = 0; i < n; ++i ) {
		printf( "%d:", i );
		for( int j = 0; j < nodes[i].len; ++j )
			printf( "(%d,%d)", nodes[i].adj[j], nodes[i].dis[j] );
		printf( "\n" );
	}
}

void Graph::trans_graph(PUndirNet& net){
	n = net->GetMxNId();
	m = net->GetEdges() * 2;

	//printf("n=%d,m=%d\n", n, m);

	int *len = new int[n];
	adj = new int[m];
	dis = new int[m];
	nodes = new Node[n];

	for(PUndirNet::TObj::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++){
		int t = NI.GetId();
		len[t] = NI.GetDeg();
	}

	int y = 0;
	for(PUndirNet::TObj::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++){
		int tt = NI.GetDeg();
		for(int ttt = 0; ttt < tt; ttt++){
			adj[y] = NI.GetNbrNId(ttt);
			y++;
		}
	}

	int pos = 0;
	for(PUndirNet::TObj::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++ ) {
		int i = NI.GetId();
		nodes[i].len = len[i];
		nodes[i].adj = adj + pos;
		nodes[i].dis = dis + pos;
		pos += len[i];
	}

	for(PUndirNet::TObj::TNodeI NI = net->BegNI(); NI < net->EndNI(); NI++ ){
		int u = NI.GetId();
		for( int i = 0; i < len[u]; ++i )  {
			//int v = nodes[u].adj[i];
			//nodes[u].dis[i] = max((int)(log(nodes[u].len) + log(nodes[v].len)),1);
			nodes[u].dis[i] = 1;
		}
	}
	delete[] len;

	//printf( "finished loading graph\n" );
}

//=====DisOracle=====
DisOracle::DisOracle(Graph *g, vector<KeyInfo*> &v_key, int *key_set) {
	this->g = g;
	n_key = (int) v_key.size();
	empty = false;
	this->key_set = key_set;
	n = g->n;

	timestamp = new int[n];
	memset( timestamp, 0, sizeof(int) * n );
	nowtime = 0;

	visit_now = new int[n];
	n_now = 0;

	visit_nxt = new int[n];
	n_nxt = 0;

	src_now = new int[n];
	src_nxt = new int[n];

	seen = new int[n];
	memset( seen, 0, sizeof(int) * n );

	dis = new int*[n_key];
	for( int i = 0; i < n_key; ++i ) {
		dis[i] = new int[n+1];
		memset( dis[i], -1, sizeof(int) * (n+1) );
	}

	dis_key = new int*[n_key];
	for( int i = 0; i < n_key; ++i ) {
		dis_key[i] = new int[n_key];
		memset( dis_key[i], -1, sizeof(int) * n_key );
	}

	++nowtime;
	for( int i = 0; i < n_key; ++i )
		for( int j = 0; j < v_key[i]->n_node; ++j ) {
			int u = v_key[i]->v_node[j];
			if( timestamp[u] != nowtime ) {
				timestamp[u] = nowtime;
				visit_now[n_now++] = u;
				src_now[u] = 0;
			}

			seen[u] |= 1<<i;
			src_now[u] |= 1<<i;
			dis[i][u] = 0;
			for( int k = 0; k < S::n[key_set[u]]; ++k )
				dis_key[i][S::v[key_set[u]][k]] = 0;
		}
	nowd = 0;
	if( n_now == 0 )
		empty = true;
	can_key_dis = true;
}

void DisOracle::disable_key_dis() {
	can_key_dis = false;
}

void DisOracle::bfs_one_round() {
	if( empty )
		return;
	++nowd;
	++nowtime;

	for( int i = 0; i < n_now; ++i ) {
		int u = visit_now[i];
		int b = src_now[u];

		for( int j = 0; j < g->nodes[u].len; ++j ) {
			int v = g->nodes[u].adj[j];
			int d = b & (~seen[v]);
			if( d ) {
				if( timestamp[v] != nowtime ) {
					timestamp[v] = nowtime;
					visit_nxt[n_nxt++] = v;
					src_nxt[v] = 0;
				}

				seen[v] |= d;
				src_nxt[v] |= d;
				for( int k = 0; k < n_key; ++k )
					if( d & (1<<k) ) {

						dis[k][v] = nowd;

						if( can_key_dis && key_set[v] ) {
							for( int l = 0; l < n_key; ++l )
								if( (key_set[v] & (1<<l)) && dis_key[k][l] == -1 )
									dis_key[k][l] = nowd;
						}
					}
			}
		}
	}

	int *tmp = visit_now;
	visit_now = visit_nxt;
	visit_nxt = tmp;

	tmp = src_now;
	src_now = src_nxt;
	src_nxt = tmp;

	n_now = n_nxt;
	n_nxt = 0;
	if( n_now == 0 )
		empty = true;
}

DisOracle::~DisOracle() {
	delete[] timestamp;
	delete[] visit_now;
	delete[] visit_nxt;
	delete[] seen;
	delete[] src_now;
	delete[] src_nxt;
	for( int i = 0; i < n_key; ++i )
		delete[] dis[i];
	delete[] dis;
	for( int i = 0; i < n_key; ++i )
		delete[] dis_key[i];
	delete[] dis_key;
}

int DisOracle::get_dis( int i_key, int u ) {
	while( !empty && dis[i_key][u] == -1 )
		bfs_one_round();
	return dis[i_key][u] == -1 ? INTINF : dis[i_key][u];
}

int DisOracle::get_dis_key( int i_key, int key ) {
	while( !empty && dis_key[i_key][key] == -1 )
		bfs_one_round();
	return dis_key[i_key][key] == -1 ? INTINF : dis_key[i_key][key];
}

bool DisOracle::get_path( int i_key, int u, vector<int> &v_path ) {
	v_path.push_back(u);
	if( dis[i_key][u] == 0 ) return true;
	if( dis[i_key][u] == -1 ) return false;
	for( int i = 0; i < g->nodes[u].len; ++i ) {
		int v = g->nodes[u].adj[i];
		if( dis[i_key][v] == dis[i_key][u] - 1 )
			return get_path( i_key, v, v_path );
	}
	return false;
}

//=====ResultNode=====
ResultNode::ResultNode( int id ) {
	this->id = id;
	used = false;
}

//=====ResultTree=====
ResultTree::ResultTree(string path) {
	this->path = path;
	n_edge = 0;
	is_tree = true;
}

ResultTree::~ResultTree() {
	for( int i = 0; i < (int)v_node.size(); ++i )
		delete v_node[i];
}

ResultNode* ResultTree::get_node( int id ) {
	if( m_node.find(id) == m_node.end() ) {
		ResultNode* tn = new ResultNode(id);
		v_node.push_back(tn);
		m_node[id] = tn;
		return tn;
	}
	return m_node[id];
}

void ResultTree::add_edge( int id1, int id2 ) {
	ResultNode* tn1 = get_node( id1 );
	ResultNode* tn2 = get_node( id2 );

	for( int i = 0; i < (int) tn1->con.size(); ++i ) {
		ResultNode *tmp = tn1->con[i];
		if( tmp->id == id2 )
			return;
	}

	++n_edge;
	tn1->con.push_back( tn2 );
	tn2->con.push_back( tn1 );
}

//=====Algorithm=====

void Algorithm::parse_query( std::vector<TIntV>& QInComV, Graph *g, vector<KeyInfo*> &v_key, int *&key_set ) {
	int n_key = (int) QInComV.size();//获得每个query 存入v_term 数量是n_key

	v_key.clear();

	key_set = new int[g->n];//此处不应该是图的大小 应该是component的大小
	memset( key_set, 0, sizeof(int) * g->n );

	for( int i = 0; i < n_key; ++i ) {
		stringstream ss;
		ss << i;
		string s = ss.str();
		KeyInfo *key = new KeyInfo(s);
		TIntV qincomv = QInComV[i];
		for(int ii = 0; ii < qincomv.Len(); ii++){
			key->v_node.push_back(qincomv[ii].Val);
		}
		key->n_node = (int)key->v_node.size();
		//key->n_node = qincomv.Len();
		//printf("%s:%d\n", s.c_str(), key->n_node);
		v_key.push_back(key);
		for(int j = 0; j < key->n_node; ++j){
			int p = key->v_node[j];
			key_set[p] |= (1<<i);
		}
	}
}

void Algorithm::parse_query( std::vector<TIntV>& QInComV, Graph *g, vector<KeyInfo*> &v_key,  DisOracle* &d, int *&key_set  ) {
	parse_query( QInComV, g, v_key, key_set );
	d = new DisOracle(g, v_key, key_set );
}

int Algorithm::get_lb_path( int u, int set, int val ) {
	int lb = val;

	if( (type & PARA_OPATH_MAX) || (type & PARA_PATH) ) {
		int val_max = 0;
		for( int i = 0; i < n_key; ++i )
			if( !(set & (1<<i)) ) {
				int tmpval = d->get_dis(i, u);
				val_max = max(val_max, tmpval);
			}
		lb = max(lb, val + val_max);
	}

	if( (type & PARA_OPATH_VPATH) || (type & PARA_PATH) ) {
		int val_vpath = INTINF;
		int _set = ((1<<n_key)-1)^set;

		if( _set == 0 )
			val_vpath = 0;
		else {
			for( int i = 0; i < n_key; ++i )
				if( _set & (1<<i) ) {
					int tmpval = d->get_dis(i,u) + pheap->get_dis( i, _set );
					val_vpath = min(val_vpath, tmpval);
				}
		}
		lb = max(lb, val + val_vpath);
	}

	if( type & PARA_PATH )
		lb = (int) (lb * PATH_MUL);

	return min( INTINF, lb );
}

int Algorithm::get_lb( int u, int set, int val ) {
	int lb = val;
	if ( type & PARA_MAX ) {
		int val_max = 0;

		/*
		int _set = ((1<<n_key)-1)^set;
		for( int ii = 0; ii < S::n[_set]; ++ii ) {
			int i = S::v[_set][ii];
			int tmpval = d->get_dis(i,u);
			val_max = max(val_max, tmpval);
		}
		*/

		for( int i = 0; i < n_key; ++i )
			if( !(set & (1<<i)) ) {
				int tmpval = d->get_dis(i,u);
				val_max = max(val_max, tmpval);
			}

		lb = max(lb, val + val_max);
	}

	/*
	if( type & PARA_SUM ) {
		int val_sum = 0;
		int n_key_left = 0;
		for( int i = 0; i < n_key; ++i )
			if( !(set & (1<<i)) ) {
				int tmpval = d->get_dis(i,u);
				val_sum += tmpval;
				++n_key_left;
			}
		if( n_key_left <= 1 )
			n_key_left = 2;
		lb = max(lb, val + val_sum/(n_key_left/2));
	}
	*/

	if( type & PARA_VTOUR ) {
		//int val_vtour = INTINF;
		int _set = ((1<<n_key)-1)^set;

		int val_vtour2 = 0;
		int val_min = INTINF;
		//int v_min = -1;

		for( int ii = 0; ii < S::n[_set]; ++ii ) {
			int i = S::v[_set][ii];
			val_min = min(val_min, d->get_dis(i,u));
		}

		if( _set == 0 ) {
			//val_vtour = 0;
			val_vtour2 = 0;
		} else {
			for( int ii = 0; ii < S::n[_set]; ++ii ) {
				int i = S::v[_set][ii];
				/*
				for( int ji = ii; ji < S::n[_set]; ++ji ){
					int j = S::v[_set][ji];
					int tmpval = d->get_dis(i,u) + d->get_dis(j,u) + pheap->get_dis( i, j, _set );
					val_vtour = min(val_vtour, tmpval);
				}
				*/

				/*
				int now_min = val_min;
				if( i == v_min ) {
					now_min = INTINF;
					for( int ji = 0; ji < S::n[_set]; ++ji )
						if( ji != ii ) {
							int j = S::v[_set][ji];
							now_min = min(now_min, d->get_dis(j,u));
						}
					if( now_min == INTINF )
						now_min = d->get_dis(i,u);
				}
				*/

				int tmpval = d->get_dis(i,u) + pheap->get_dis(i, _set) + val_min;
				val_vtour2 = max(val_vtour2, tmpval);
			}
			/*
			for( int i = 0; i < n_key; ++i )
				if( _set & (1<<i) )
					for( int j = i; j < n_key; ++j )
						if( _set & (1<<j) ) {
							int tmpval = d->get_dis(i,u) + d->get_dis(j,u) + pheap->get_dis( i, j, _set );
							val_vtour = min(val_vtour, tmpval);
						}
			*/
		}
		//val_vtour = (val_vtour + 1) / 2;
		//lb = max(lb, val + val_vtour);

		val_vtour2 = (val_vtour2 + 1) / 2;
		lb = max(lb, val + val_vtour2);
	}

	if( type & PARA_SUBVTOUR ) {

		int v_vtour = 0;
		int _s = ((1<<n_key)-1)^set;

		if( _s == 0 )
			v_vtour = 0;
		else {
			for( int si = 0; si < S::ns[_s]; ++si) {
				int _set = S::vs[_s][si];
				int val_vtour = INTINF;
				for( int ii = 0; ii < S::n[_set]; ++ii ) {
					int i = S::v[_set][ii];
						for( int ji = ii; ji < S::n[_set]; ++ji ){
							int j = S::v[_set][ji];
							int tmpval = d->get_dis(i,u) + d->get_dis(j,u) + pheap->get_dis( i, j, _set );
							val_vtour = min(val_vtour, tmpval);
						}
				}

				v_vtour = max(v_vtour, val_vtour);
			}
		}
		v_vtour = (v_vtour + 1) / 2;
		lb = max(lb, val + v_vtour);
	}

	if( type & PARA_VPATH ) {
		int val_vpath = INTINF;
		int _set = ((1<<n_key)-1)^set;

		if( _set == 0 )
			val_vpath = 0;
		else {
			for( int i = 0; i < n_key; ++i )
				if( _set & (1<<i) ) {
					int tmpval = d->get_dis(i,u) + pheap->get_dis( i, _set );
					val_vpath = min(val_vpath, tmpval);
				}
		}

		val_vpath = (val_vpath + 1) / 2;
		lb = max(lb, val + val_vpath);
	}

	if( type & PARA_PATH ) {
		int _set = ((1<<n_key)-1)^set;

		if( _set != 0 ) {
			Entry *e = heap->find( u, _set, LABEL_PATH );
			if( e != NULL )
				if( e->info & (1<<(USED_BIT-1)) ) {
					int val_min = INTINF;
					if( _set == 0 )
						val_min = 0;
					else {
						for( int i = 0; i < n_key; ++i )
							if( _set & (1<<i) ) {
								int tmpval = d->get_dis(i, u);
								val_min = min(val_min, tmpval);
							}
					}

					lb = max(lb, val + (e->val+val_min+1)/2);
				}
		}
	}

	if( type & PARA_TREE ) {
		int _set = ((1<<n_key)-1)^set;

		if( _set != 0 ) {
			Entry *e = heap->find( u, _set, LABEL_TREE );
			if( e != NULL )
				if( e->info & (1<<(USED_BIT-1)) ) {
					lb = max(lb, val + e->val);
					//printf( "[%d:%d]", _set, e->val );
					best = min( best, lb );
				}
		}
	}

	return min(INTINF, lb);
}

void Algorithm::update( int u, int set, int val ) {
	int pre_best = best;
	int lb = get_lb( u, set, val );
	lb = max(min_cost, lb); //here better change min_cost to be the lb of the father status of u

	if( lb > best )
		return;

	if( set + 1 == 1<<n_key )
		best = min( best, val );

	int val_sum = val;
	for( int i = 0; i < n_key; ++i )
		if( (set & (1<<i)) == 0 )
			val_sum += d->get_dis(i,u);

	val_sum = min( val_sum, INTINF );

	//printf( "u=%d,set=%d,val=%d,lb=%d,val_sum=%d\n", u, set, val, lb, val_sum );

	best = min( best, val + val_sum );

	Entry *e = heap->find( u, set, LABEL_TREE );

	if( e == NULL )
		e = heap->enheap( u, set, LABEL_TREE, val, lb );
	else if( e->info & (1<<(USED_BIT-1)) )
		return;
	else if( lb < e->cost ) {
		e->val = val;
		e->cost = lb;
		heap->update( e );
	}

	if( best < pre_best ) {
		if( best < pre_best && show_progress_result )
			print_progress_result( e );
	}
}

void Algorithm::update_path( int u, int set, int val ) {
	int lb = get_lb_path( u, set, val );
	lb = max(min_cost, lb);

	if( type & PARA_OPATH ) {
		if( set + 1 == 1<<n_key )
			best = min( best, val );
		int val_sum = val;
		int val_max = 0;
		for( int i = 0; i < n_key; ++i )
			if( (set & (1<<i)) == 0 ) {
				int nowd = d->get_dis(i,u);
				val_sum += nowd * 2;
				val_max = max( val_max, nowd );
			}
		val_sum -= val_max;
		best = min( best, val_sum );
	}

	Entry *e = heap->find( u, set, LABEL_PATH );
	if( e == NULL )
		heap->enheap( u, set, LABEL_PATH, val, lb );
	else if( e->info & (1<<(USED_BIT-1)) )
		return;
	else if( lb < e->cost ) {
		e->val = val;
		e->cost = lb;
		heap->update( e );
	}
}

void Algorithm::print_progress( int cnt, int cnt_tree, int cnt_path ) {
	long now_t = clock() - tm_start;
	if( best >= INTINF || min_cost <= 0 ) {
		printf( "nowt=%0.0lf, cnt_tree=%0.2lfM, cnt_path=%0.2lfM, best=%d, min_cost=%d\n",
				now_t/CLOCK_SECOND, cnt_tree/1000000.0, cnt_path/1000000.0, best, min_cost );
		return;
	}
	printf( "nowt=%0.0lf, cnt_tree=%0.2lfM, cnt_path=%0.2lfM, best=%d, min_cost=%d, approx=%0.3lf, heaplen=%d\n",
			now_t/CLOCK_SECOND,  cnt_tree/1000000.0, cnt_path/1000000.0, best, min_cost, best*1.0/min_cost, heap->len() );
}

void Algorithm::record_progress() {
	if( best >= INTINF )
		return;
	if( min_cost <= 0 )
		return;

	if( best != pre_best || min_cost != pre_min_cost ) {
		double tm_now = (clock() - tm_start)/CLOCK_SECOND;
		double qlty = best * 1.0 / min_cost;
		diagram_time.push_back( tm_now );
		diagram_qlty.push_back( qlty );
		diagram_best.push_back( best );
		diagram_minc.push_back( min_cost );
		diagram_memy.push_back( get_memory() );
		pre_best = best;
		pre_min_cost = min_cost;
	}
}

void Algorithm::create_result_path_tree( Entry *e ) {
	if( e->val == 0 )
		return;
	int u = e->p;
	int s = e->set;

	//for bidirected search
	if( e->val == min_cost ) {
		for( int i = 1; i < s; ++i )
			if( (s & i) == i ) {
				Entry *ei = heap->find( u, i, LABEL_PATH );
				if( ei ) {
					int _i = s&(((1<<n_key)-1)^i);
					Entry *_ei = heap->find( u, _i, LABEL_PATH );
					if( _ei )
						if( ei->val + _ei->val == e->val ) {
							create_result_path_tree( ei );
							create_result_path_tree( _ei );
							return;
						}
				}
			}
	}

	for( int i = 0; i < n_key; ++i )
		if( (s & (1<<i)) && (key_set[u] & (1<<i)) ) {
			int _s = s&(~(1<<i));
			Entry *_e = heap->find( u, _s, LABEL_PATH );
			if( _e )
				if( e->val == _e->val ) {
					create_result_path_tree( _e );
					return;
				}
		}

	for( int i = 0; i < g->nodes[u].len; ++i ) {
		int v = g->nodes[u].adj[i];
		int w = g->nodes[u].dis[i];
		Entry *ei = heap->find( v, s, LABEL_PATH );
		if( ei )
			if( ei->val + w == e->val ) {
				rt->add_edge( u, v );
				//printf( "[%d(%d,%d),%d(%d,%d)]\n", u, e->val, e->set, v, ei->val, ei->set);
				create_result_path_tree( ei );
				return;
			}
	}
}

void Algorithm::create_result_tree( Entry *e ) {
	if( e->val == 0 )
		return;
	int u = e->p;
	int s = e->set;
	for( int i = 1; i < s; ++i )
		if( (s & i) == i ) {
			Entry *ei = heap->find( u, i, LABEL_TREE );
			if( ei ) {
				int _i = s&(((1<<n_key)-1)^i);
				Entry *_ei = heap->find( u, _i, LABEL_TREE );
				if( _ei )
					if( ei->val + _ei->val == e->val ) {
						create_result_tree( ei );
						create_result_tree( _ei );
						return;
					}
			}
		}

	for( int i = 0; i < g->nodes[u].len; ++i ) {
		int v = g->nodes[u].adj[i];
		int w = g->nodes[u].dis[i];
		Entry *ei = heap->find( v, s, LABEL_TREE );
		if( ei )
			if( ei->val + w == e->val ) {
				rt->add_edge( u, v );
				create_result_tree( ei );
				return;
			}
	}
}

void Algorithm::print_diagram() {
	printf( "Progress Diagram:\n" );
	for( int i = 0; i < (int)diagram_time.size(); ++i )
		printf( "%0.3lf:%0.3lf:%d:%d\n", diagram_time[i], diagram_qlty[i], diagram_best[i], diagram_minc[i] );
}

void Algorithm::print_progress_result( Entry* e ) {
	if( type & PARA_OPATH ) return;

	printf( "======progress result======\n" );
	printf( "best=%d, lb=%d\n", best, e->cost );
	rt = new ResultTree(g->path);
	rt->get_node( e->p );

	create_result_tree( e );

	for( int i = 0; i < n_key; ++i )
		if( (e->set & (1<<i)) == 0 ) {
			vector<int> v_path;
			if( !d->get_path( i, e->p, v_path ) ) printf( "Keyword %d: path not been found!\n", i );
			//printf( "len=%d\n", (int) v_path.size() - 1 );
			for( int j = 1; j < (int) v_path.size(); ++j )
				rt->add_edge( v_path[j-1], v_path[j] );
		}

	print_result(rt);
	printf( "n_edge=%d\n", rt->n_edge );

	best = min(best,rt->n_edge);
	delete rt;
}

void Algorithm::print_result( Entry* e ) {
	printf( "Print Result:\n" );
	rt = new ResultTree(g->path);
	rt->get_node( e->p );

	if( type & PARA_OPATH )
		create_result_path_tree( e );
	else
		create_result_tree( e );

	//for( int i = 0; i < rt->v_node.size(); ++i ) {
	//	int u = rt->v_node[i]->id;
	//	string st = Application::find_node_st( g->path, u );
	//	printf( "%d:%s\n", i+1, st.c_str() );
	//}

	print_result(rt);
	//string node_st = Application::find_node_st( g->path, rt->v_node[0]->id );
	//printf( "%s (%d)\n", node_st.c_str(), rt->v_node[0]->id );

	//is_tree = true;
	//print_result( rt, "", NULL, rt->v_node[0] );
	//printf( "is_tree=%s\n", rt->is_tree?"true":"false" );
	delete rt;
}

void Algorithm::print_result( ResultTree *rt ) {
	string node_st = Application::find_node_st( rt->path, rt->v_node[0]->id );
	printf( "%s (%d)\n", node_st.c_str(), rt->v_node[0]->id );
	print_result( rt, "", NULL, rt->v_node[0] );
	printf( "is_tree=%s\n", rt->is_tree?"true":"false" );
}

void Algorithm::print_result( ResultTree *rt, string st, ResultNode *pre, ResultNode *now ) {
	if( now->used ) {
		rt->is_tree = false;
		return;
	}
	now->used = true;
	vector<ResultNode*> v_node;
	for( int i = 0; i < (int) now->con.size(); ++i ) {
		ResultNode* tn = now->con[i];
		if( tn != pre )
			v_node.push_back( tn );
	}

	string st_new;
	for( int i = 0; i < (int) v_node.size(); ++i ) {
		ResultNode* tn = v_node[i];
		//printf( "id=%d\n", tn->id );
		string node_st = Application::find_node_st( rt->path, tn->id );
		printf( "%s|-%s (%d)\n", st.c_str(), node_st.c_str(), tn->id );
		if( i == (int) v_node.size() - 1 )
			st_new = st + "  ";
		else
			st_new = st + "| ";
		print_result( rt, st_new, now, tn );
	}
}

double Algorithm::get_memory() {
	double t_mem = heap->len() * 1.0 * sizeof(Entry);
	//t_mem += ( 12.0 + n_key * 4) * g->n;
	return t_mem / (1024.0*1024.0);
}

void Algorithm::run( PUndirNet& net, std::vector<TIntV>& QInComV, int type, bool show_progress_result ) {
	printf( "size=%d\ntype=%d\n", net->GetNodes(), type );

	this->show_progress_result = show_progress_result;
	g = new Graph();
	g->trans_graph(net);

	g->path = "";

	long tm = clock();
	tm_start = clock();
	printf( "Parsing query...\n" );
	Algorithm::parse_query( QInComV, g, v_key, d, key_set );

	n_key = (int)v_key.size();
	best = INTINF;
	min_cost = -1;
	pre_best = best;
	pre_min_cost = min_cost;
	fail_reason = "Success";
	succ = false;

	diagram_time.clear();
	diagram_qlty.clear();
	diagram_best.clear();
	diagram_minc.clear();
	diagram_memy.clear();

	v_entry = new vector<Entry*>[g->n];
	Entry *rst = NULL;
	this->type = type;

	//printf( "Init Dij...\n" );
	//for( int i = 0; i < n_key; ++i ) {
	//	printf( "%d\n", i );
	//	v_dij[i]->get_dis( g->n );
	//}

	printf( "Initializing pheap...\n" );
	pheap = NULL;
	if( (type & PARA_VTOUR) || (type & PARA_PATH) || (type & PARA_VPATH)
			|| (type & PARA_OPATH_VPATH) || (type & PARA_SUBVTOUR))
		pheap = new PEntryHeap(d);

	d->disable_key_dis();

	printf( "Initializing heap...\n" );
	heap = new EntryHeap();
	for( int i = 0; i < n_key; ++i )
		for( int j = 0; j < v_key[i]->n_node; ++j ) {
			int u = v_key[i]->v_node[j];
			if( !(type & PARA_OPATH) )
				update( u, 1<<i, 0 );

			if( (type & PARA_PATH) || (type & PARA_OPATH) )
				update_path( u, 1<<i, 0 );
		}

	int cnt = 0, cnt_tree = 0, cnt_path = 0;

	printf( "Running Alg...\n" );
	while( !heap->empty() ) {
		if( heap->len() > MAXHEAP ) {
			fail_reason = "OutOfMemory";
			break;
		}
		Entry *e = heap->deheap();

		int u = e->p;
		int label = e->info & ((1<<N_LABEL_BIT)-1);
		min_cost = e->cost;
		if( label == LABEL_TREE )
			++cnt_tree;
		if( label == LABEL_PATH )
			++cnt_path;

		if( cnt == 0 && best >= INTINF )
			break;
		if( cnt % 1000000 == 0 )
			print_progress( cnt, cnt_tree, cnt_path );

		record_progress();
		++cnt;

		//printf( "u=%d,label=%d,set=%d,cost=%d,best=%d\n", u, label, e->set, min_cost, best);

		if( (label == LABEL_TREE && e->set+1==(1<<n_key)) ||
				((type & PARA_OPATH) && (label == LABEL_PATH) && (e->set+1==(1<<n_key)) )) {
			rst = e;
			succ = true;
			break;
		}

		if( label == LABEL_TREE ) {
			if(type & PARA_BIDIRECT) {
				int _set = ((1<<n_key)-1)^e->set;
				Entry *_e = heap->find( u, _set, LABEL_TREE );
				if( _e != NULL )
					if( _e->info & (1<<(USED_BIT-1)) ) {
						update( u, (1<<n_key)-1, e->val + _e->val );
						continue;
				}
			}

			if( (!(type & PARA_BIDIRECT)) || e->val <= (best-1)/2 ) {
				for( int i = 0; i < g->nodes[u].len; ++i ) {
					int v = g->nodes[u].adj[i];
					int w = g->nodes[u].dis[i];
					if( (!(type & PARA_BIDIRECT)) || e->val <= (best-1)/2 )
						update( v, e->set, e->val + w );
				}
			}

			if( (!(type & PARA_BIDIRECT)) || e->val <= (best-1)/2 ) {
				for( int i = 0; i < (int) v_entry[u].size(); ++i ) {
					Entry *_e = v_entry[u][i];
					if( (_e->set & e->set) == 0 )
						if( (!(type & PARA_BIDIRECT)) || (e->val <= (best-1)/2 && _e->val <= (best-1)/2 && e->val + _e->val <= best*2/3) )
							update( u, e->set|_e->set, e->val + _e->val );
				}
			}

			v_entry[u].push_back(e);
		} else if( (label == LABEL_PATH) || (type & PARA_OPATH) ){
			if((type & PARA_OPATH_BIDIRECT) || (type & PARA_PATH)) {
				int _set = ((1<<n_key)-1)^e->set;
				Entry *_e = heap->find( u, _set, LABEL_PATH );
				if( _e != NULL )
					if( _e->info & (1<<(USED_BIT-1)) ) {
						update_path( u, (1<<n_key)-1, e->val + _e->val );
						continue;
				}
			}

			if( (!((type & PARA_OPATH_BIDIRECT) || (type & PARA_PATH))) || e->val <= (best-1)/2 ) {
				for( int i = 0; i < g->nodes[u].len; ++i ) {
					int v = g->nodes[u].adj[i];
					int w = g->nodes[u].dis[i];
					if( (!((type & PARA_OPATH_BIDIRECT) || (type & PARA_PATH))) || e->val <= (best-1)/2 )
						update_path( v, e->set, e->val + w );
				}
			}

			for( int i = 0; i < n_key; ++i )
				if( (e->set & (1<<i)) == 0  && (key_set[u] & (1<<i)) )
					if( (!((type & PARA_OPATH_BIDIRECT) || (type & PARA_PATH))) || e->val <= best/2 )
						update_path( u, e->set | (1<<i), e->val );
		}
	}

	tm = clock() - tm;
	processing_time = tm/CLOCK_SECOND;
	if( diagram_time.size() != 0 )
		processing_time = diagram_time[diagram_time.size()-1];

	if( rst != NULL ) {
		print_result( rst );
		print_diagram();
	}

	printf( "cnt_tree=%d,cnt_path=%d\n", cnt_tree, cnt_path );
	printf( "Succ=%s,cnt=%d,len=%d,", succ ? "true" : "false", cnt, heap->len()  );
	printf( "time=%0.3lfsec,cost=%d,mem=%0.3lfM\n", processing_time, min_cost, get_memory() );
	if( rt != NULL && (type & PARA_OPATH) )
		printf( "tree_cost=%d\n", rt->n_edge );
	total_memory = get_memory();
	heap_len = heap->len();
	//succ = (rst == NULL ? "false" : "true");

	delete[] v_entry;
	if( pheap )
		delete pheap;
	delete heap;
	for( int i = 0; i < n_key; ++i )
		delete v_key[i];
	delete d;
	delete[] key_set;
	delete g;
}

//======BANKS Related======

bool BanksAlgorithm::is_complete(int u) {
	for( int i = 0; i < n_key; ++i )
		if( key_dis[u][i] == NULL )
			return false;
	return true;
}

int BanksAlgorithm::find_best() {
	int best = -1;
	for( int i = 0; i < n_key; ++i )
		if( !key_iter[i].empty() ) {
			if( best == -1 )
				best = i;
			else if ( key_iter[i].get_max_val() > key_iter[best].get_max_val() )
				best = i;
		}
	return best;
}

void BanksAlgorithm::output(int u) {
	ResultTree *rt = new ResultTree(g->path);
	rt->get_node( u );
	for( int i = 0; i < n_key; ++i )
		for(BanksNode *bn = key_dis[u][i]; bn->pre; bn = bn->pre)
			rt->add_edge( bn->id, bn->pre->id );
	printf( "n_edge=%d\n", (int) rt->v_node.size() - 1 );
	Algorithm::print_result( rt );
	delete rt;
}

void BanksAlgorithm::update_best(int u) {
	int now_cost = 0;
	for( int i = 0; i < n_key; ++i )
		now_cost += key_dis[u][i]->dis;
	if( best_cost < 0 || now_cost < best_cost ) {
		best_cost = now_cost;
		best_root = u;
	}
}

void BanksAlgorithm::run(PUndirNet& net, std::vector<TIntV>& QInComV, int type, bool show_progress_result) {
	g = new Graph();
	g->trans_graph(net);

	long tm = clock();

	total_memory = 0;
	int cnt = 0;
	best_cost = -1;
	best_root = -1;
	printf( "Parsing query...\n" );
	Algorithm::parse_query(  QInComV, g, v_key, key_set );
	n_key = (int)v_key.size();
	printf("n_key: %d\n", n_key);
	key_iter = new BanksKeyIter[n_key];
	key_dis = new BanksNode**[g->n];
	for( int i = 0; i < g->n; ++i ) {
		key_dis[i] = new BanksNode*[n_key];
		memset( key_dis[i], 0, sizeof(BanksNode*) * n_key );
	}
	total_memory += sizeof(BanksNode*) * g->n * n_key;
	printf("1 ");
	for( int i = 0; i < n_key; ++i ) {
		key_iter[i].g = g;
		key_iter[i].key = v_key[i];
		key_iter[i].key_dis = key_dis;
		key_iter[i].key_id = i;
		key_iter[i].init();
	}
	printf("2 ");
	total_memory += sizeof(BanksNode*) * g->n * n_key + sizeof(BanksNode) * g->n * n_key;

	for( int k = find_best(); k != -1; k = find_best() ) {
		int u = key_iter[k].expand();
		if( is_complete(u) ) {
			update_best(u);
			++cnt;
			if( cnt % 1000000 == 0 ) printf( "%d\n", cnt );
		}
	}

	if( best_root != -1 )
		output( best_root );

	for( int i = 0; i < n_key; ++i )
		key_iter[i].clear();
	for( int i = 0; i < g->n; ++i )
		delete[] key_dis[i];
	delete[] key_dis;
	delete[] key_iter;
	delete[] key_set;
	for( int i = 0; i < n_key; ++i )
		delete v_key[i];
	delete g;
	tm = clock() - tm;

	process_time = tm/CLOCK_SECOND;
	total_memory /= 1000000.0;
	printf( "processing time = %0.3lf sec\n", process_time );
	printf( "total memory = %0.3lf mb\n", total_memory );
}

//======BanksKeyIter======
void BanksKeyIter::init() {
	printf("tt ");
	try{
	nodes = new BanksNode[g->n];
	}
	catch(std::bad_alloc& ba){
		cout<<"failed:exception caught"<<endl;
	}
	printf("%d\n", sizeof(BanksNode));
	printf("20");
	heap = new BanksNode*[g->n+1];
	printf("21");
	for( int i = 0; i < g->n; ++i ) {
		nodes[i].dis = -1;
		nodes[i].id = i;
		nodes[i].pre = NULL;
		nodes[i].val = 0;
		nodes[i].idx = -1;
	}
	printf("22");
	memset( heap, 0, sizeof(BanksNode*) * (g->n+1) );

	tot = 0;
	printf("11 ");
	for( int i = 0; i < key->n_node; ++i ) {
		int id = key->v_node[i];
		printf("12 ");
		double val = 1.0/key->n_node;
		printf("13 ");
		BanksNode *bn = nodes + id;
		printf("14 ");
		bn->dis = 0;
		printf("15 ");
		bn->val = val;
		printf("16 ");
		enheap( bn );
		printf("17 ");
	}
}

void BanksKeyIter::clear() {
	delete[] nodes;
	delete[] heap;
}

void BanksKeyIter::up( int p ) {
	BanksNode* tmp = heap[p];
	for( int i = p >> 1; i >= 1; i >>= 1 ) {
		if( heap[i]->val > tmp->val )
		  break;
		heap[p] = heap[i];
		heap[p]->idx = p;
		p = i;
	}
	heap[p] = tmp;
	heap[p]->idx = p;
}

void BanksKeyIter::down( int p ) {
	BanksNode* tmp = heap[p];
	for( int i = p << 1; i <= tot; i <<= 1 ) {
		if( i + 1 <= tot && heap[i]->val < heap[i + 1]->val )
		  ++i;
		if( heap[i]->val < tmp->val )
		  break;
		heap[p] = heap[i];
		heap[p]->idx = p;
		p = i;
	}
	heap[p] = tmp;
	heap[p]->idx = p;
}

bool BanksKeyIter::empty() {
	return tot == 0;
}

double BanksKeyIter::get_max_val() {
	return tot == 0 ? -1 : heap[1]->val;
}

void BanksKeyIter::enheap( BanksNode *bn ) {
	bn->idx = ++tot;
	heap[tot] = bn;
	up(tot);
}

void BanksKeyIter::update( BanksNode *bn ) {
	up( bn->idx );
}

BanksNode* BanksKeyIter::deheap() {
	if( tot == 0 )
		return NULL;
	BanksNode* bn = heap[1];
	heap[1] = heap[tot--];
	if( tot > 0 ) {
		heap[1]->idx = 1;
		down(1);
	}
	bn->idx = -2;
	return bn;
}

void BanksKeyIter::update_dis( BanksNode *bn ) {
	vector<int> q;
	q.push_back(bn->id);
	unsigned int head = 0;
	while( head < q.size() ) {
		int u = q[head++];
		BanksNode *bu = nodes + u;
		for( int i = 0; i < g->nodes[u].len; ++i ) {
			int v = g->nodes[u].adj[i];
			BanksNode *bv = nodes + v;
			if( bv->idx > 0 && bu->dis + 1 < bv->dis ) {
				q.push_back(v);
				bv->dis = bu->dis + 1;
				bv->pre = bu;
			}
		}
	}
}

int BanksKeyIter::expand() {
	BanksNode* bn_pre = deheap();
	int u = bn_pre->id;
	for( int i = 0; i < g->nodes[u].len; ++i ) {
		int v = g->nodes[u].adj[i];
		BanksNode* bn = nodes+v;
		if( bn->idx == -2 )
			continue;
		double val = bn_pre->val * LMD / g->nodes[u].len;
		if( bn->idx == -1 ) {
			bn->val = val;
			bn->dis = bn_pre->dis + 1;
			bn->pre = bn_pre;
			enheap(bn);
		} else {
			if( bn->val < val ) {
				bn->val = val;
				update( bn );
			}
			if( bn_pre->dis + 1 < bn->dis ) {
				bn->dis = bn_pre->dis + 1;
				bn->pre = bn_pre;
				update_dis( bn );
			}
		}
	}

	key_dis[u][key_id] = bn_pre;
	return bn_pre->id;
}

//======exp======

const string Exp::in_path = "/Users/luqin/Documents/workspace/projects/MinGroupSteinerTree/exp/";
const string Exp::out_path = "/Users/luqin/Documents/collaboration/ronghua li/2015 progressive keyword search/exp/";

//const int Exp::n_knum = 10;
//const string Exp::v_knum[] = {"3","4","5","6","7","8", "9", "10", "11", "12"};

const int Exp::n_knum = 5;
const string Exp::v_knum[] = {"4","5","6","7","8"};
const string Exp::default_knum = "6";

//const int Exp::n_kwf = 6;
//const string Exp::v_kwf[] = {"100","200","400","800","1600","3200"};

const int Exp::n_kwf = 1;
const string Exp::v_kwf[] = {"400"};
const string Exp::default_kwf = "400";

const int Exp::n_dataset = 2;
//const string Exp::v_dataset[] = {"dblp", "imdb"};
const string Exp::v_dataset[] = {"usa-mgst", "lj-mgst"};

const int Exp::n_alg = 4;
const string Exp::v_alg[] = {"basic", "bidirect", "bidirect_max", "bidirect_max_vtour"};

const double Exp::max_apr = 8;

//const int Exp::n_apr = 9;
//const string Exp::v_apr[] = {4, 3.36, 2.83, 2.38, 2, 1.68, 1.41, 1.19, 1};


void Exp::exp_all() {
	for( int i_dataset = 0; i_dataset < n_dataset; ++i_dataset ) {
		string dataset = v_dataset[i_dataset];
		for( int i_alg = 0; i_alg < n_alg; ++i_alg ) {
			string alg = v_alg[i_alg];
			for( int i_knum = 0; i_knum < n_knum; ++i_knum ) {
				string knum = v_knum[i_knum];
				draw_diagram( dataset, alg, knum, default_kwf );
			}
			for( int i_kwf = 0; i_kwf < n_kwf; ++i_kwf ) {
				string kwf = v_kwf[i_kwf];
				draw_diagram( dataset, alg, default_knum, kwf );
			}
		}
	}
}

void Exp::draw_diagram(string dataset, string alg, string knum, string kwf) {
	string st = dataset + "." + alg + ".knum_" + knum + ".kwf_" + kwf + ".txt";
	string dir_in = in_path + st;
	string dir_out = out_path + st;

	FILE *fin = fopen( dir_in.c_str(), "r" );

	if( fin == NULL ) {
		printf( "file:%s not found!\n", st.c_str() );
		return;
	}

	FILE *fout = fopen( dir_out.c_str(), "w" );

	char line[1024];

	fgets(line, 1024, fin);
	while( strncmp( line, "<diagram>", 9 ) != 0 )
		fgets(line, 1024, fin);


	fgets(line, 1024, fin);
	double pret = 0, prea = 100;
	while(  strncmp( line, "</diagram>", 9 ) != 0 ) {
		double nowt, nowa, nowm;
		int ub, lb;
		sscanf( line, "%lf %lf %lf %d %d", &nowt, &nowa, &nowm, &ub, &lb );

		if( nowa < max_apr + 1e-5 && (nowa < prea * 0.93 || nowa < 1.0 + 1e-7) ) {
			fprintf( fout, "%0.3lf\t%0.3lf\t%0.3lf\t%d\t%d\n", nowa, nowt, nowm, ub, lb );
			pret = nowt;
			prea = nowa;
		}

		fgets(line, 1024, fin);
	}

	fclose( fin );
	fclose( fout );
}
