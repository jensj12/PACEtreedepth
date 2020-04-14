#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;
typedef vector<vi> vvi;

int N, M;
int currentRoot = 0;
int currentTree[501];
int bestTree[501];
stack<vii> order;

// Maximal independent set algorithm:
// Take a vertex v of minimum degree
// Try all vertices of N(v) and recurse
// O(1.4432^n)

// Treedepth algorithm:
// Try all maximal independent sets
// Contract and recurse
// T(n) = O(1.4432^n) * T(n-2)
// T(n) = O(1.4432^(n*n/2))
// Which means we can handle graphs with 13 vertices
// What to expect with O(c^O(n^2))
// But it will hopefully do better

enum VSTATUS
{
	STATUS_ROOT,
	STATUS_REMOVED,
	STATUS_ORPHAN,
	STATUS_INACTIVE,
	STATUS_ACTIVE,
};

struct Graph {
	//int n;
	vvi adj_list;
	vector<VSTATUS> v_status;
} original_graph;

bool contains(vi &c, int f) {
	for (int v : c) if (v == f) return true;
	return false;
}

void remove_value(vi &c, int v) {
	for (size_t i = 0; i < c.size(); i++)
	{
		if (c[i] == v) {
			c[i] = c[c.size() - 1];
			c.pop_back();
			return;
		}
	}
}

void add_edge(Graph &g, int v1, int v2) {
	if (!contains(g.adj_list[v1], v2)) {
		g.adj_list[v1].push_back(v2);
		g.adj_list[v2].push_back(v1);
	}
}

void remove_edge(Graph &g, int v1, int v2) {
	remove_value(g.adj_list[v1], v2);
	remove_value(g.adj_list[v2], v1);
}

void contract_vertex(Graph &g, int v) {
	int size = g.adj_list[v].size();
	for (int i = size - 1; i >= 0; i--)
	{
		if (g.v_status[g.adj_list[v][i]] == STATUS_ACTIVE)
			g.v_status[g.adj_list[v][i]] = STATUS_INACTIVE;
		for (int j = 0; j < i; j++)
		{
			add_edge(g, g.adj_list[v][i], g.adj_list[v][j]);
		}
		//remove_edge(g, v, g.adj_list[v][i]);
		remove_value(g.adj_list[g.adj_list[v][i]], v);
	}
	g.v_status[v] = STATUS_ORPHAN;
	currentTree[v] = 0;
}

int TreeDepth(Graph&, int);

int maximal_independent_set(Graph &g, int depth, int status) {
	int deg = N, v = 0, n = 0;
	while (true)
	{
		if (status == order.top().size()) return TreeDepth(g, depth + 1);
		v = order.top()[status].second;
		if (g.v_status[v] == STATUS_ACTIVE) break;
		status++;
	}
	deg = g.adj_list[v].size();
	//if (v == 0) cerr << "ERROR, v=0" << endl;
	Graph g2 = g;
	contract_vertex(g2, v);
	int val, best = maximal_independent_set(g2, depth, status + 1);
	if (deg == 1 || (deg == 2 && contains(g.adj_list[g.adj_list[v][0]], g.adj_list[v][1]))) return best;
	for (int i = 0; i < deg; i++) {
		if (g.v_status[g.adj_list[v][i]] != STATUS_ACTIVE) continue;
		g2 = g;
		contract_vertex(g2, g.adj_list[v][i]);
		val = maximal_independent_set(g2, depth, status + 1);
		if (val < best) best = val;
	}
	return best;
}

int make_root(Graph &g, int v, int depth) {
	currentTree[v] = currentRoot;
	int tmpRoot = currentRoot;
	currentRoot = v;
	g.v_status[v] = STATUS_ROOT;
	int deg = g.adj_list[v].size();
	for (int i = deg - 1; i >= 0; i--)
	{
		int other = g.adj_list[v][i];
		remove_value(g.adj_list[other], v);
	}
	int val = TreeDepth(g, depth + 1);
	currentRoot = tmpRoot;
	return val;
}

void match_parents(Graph &g) {
	for (int i = 1; i <= N; i++)
	{
		if (g.v_status[i] == STATUS_ORPHAN) {
			for (size_t j = 0; j < g.adj_list[i].size(); j++)
			{
				if (g.v_status[g.adj_list[i][j]] == STATUS_REMOVED || g.v_status[g.adj_list[i][j]] == STATUS_ORPHAN) {
					g.v_status[i] = STATUS_REMOVED;
					currentTree[i] = g.adj_list[i][j];
					g.adj_list[i].clear();
					break;
				}
			}
		}
	}
}

int TreeDepth(Graph &g, int depth) {
	if (depth >= bestTree[0]) return N;
	int n = 0, maxDeg = -1, v = 0, minDeg = N;
	vii newOrder = vii();
	bool rematch = false;
	match_parents(g);
	for (int i = 1; i <= N; i++)
	{
		if (g.v_status[i] == STATUS_INACTIVE) g.v_status[i] = STATUS_ACTIVE;
		if (g.v_status[i] == STATUS_ACTIVE) {
			int deg = g.adj_list[i].size();
			if (deg > maxDeg) {
				maxDeg = deg;
				v = i;
			}
			if (deg == 0) {
				currentTree[i] = currentRoot;
				g.v_status[i] = STATUS_REMOVED;
				rematch = true;
				continue;
			}
			newOrder.emplace_back(deg, i);
			n++;
			if (deg < minDeg) minDeg = deg;
		}
	}
	if (rematch) match_parents(g);
	if (n == 0) {
		if (depth < bestTree[0]) {
			for (int i = 1; i <= N; i++)
			{
				if (g.v_status[i] == STATUS_ORPHAN) currentTree[i] = currentRoot;
				bestTree[i] = currentTree[i];
			}
			bestTree[0] = depth;
		}
		return 1;
	}
	if (minDeg == n - 1) {
		if (depth + n >= bestTree[0]) return N;
		// immediately complete the tree, we have a complete graph
		for (int i = 1; i <= N; i++)
		{
			if (g.v_status[i] == STATUS_ACTIVE) {
				currentTree[i] = currentRoot;
				currentRoot = i;
			}
		}
		for (int i = 1; i <= N; i++)
		{
			if (g.v_status[i] == STATUS_ORPHAN) {
				currentTree[i] = currentRoot;
			}
			bestTree[i] = currentTree[i];
		}
		bestTree[0] = depth + n - 1;
		return n;
	}
	if (maxDeg == n - 1) return make_root(g, v, depth) + 1;
	sort(newOrder.begin(), newOrder.end());
	order.push(newOrder);
	int val = maximal_independent_set(g, depth, 0) + 1;
	order.pop();
	return val;
}

int main()
{
	string str;
	int count = 0;
	while (true) {
		getline(cin, str);
		if (str[0] == 'c') continue;
		istringstream iss(str);
		int i, j;
		if (str[0] == 'p') {
			// read n and m
			string waste;
			iss >> waste;
			iss >> waste;
			iss >> N >> M;
			//original_graph.n = N;
			bestTree[0] = N + 1;
			for (i = 0; i < N + 1; i++) {
				original_graph.adj_list.push_back(vi());
				original_graph.v_status.push_back(i == 0 ? STATUS_REMOVED : STATUS_ACTIVE);
			}
			continue;
		}
		iss >> i >> j;
		original_graph.adj_list[i].push_back(j);
		original_graph.adj_list[j].push_back(i);
		if (++count == M) break;
	}
	cout << TreeDepth(original_graph, 0) << endl;
	//cerr << "CheckDepth: " << bestTree[0] << endl;
	for (int i = 1; i <= N; i++)
	{
		//cerr << i << ": ";
		cout << bestTree[i] << endl;
	}
}

