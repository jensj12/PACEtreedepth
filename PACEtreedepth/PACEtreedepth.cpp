#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <csignal>
//#include <unistd.h>

using namespace std;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;
typedef vector<vi> vvi;

int N, M;
int currentRoot = 0;
vi currentTree;
vi bestTree;
vii order;

volatile sig_atomic_t tle = 0;

void term(int signum)
{
	tle = 1;
}

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

void match_parents(Graph &g) {
	for (int i = 1; i <= N; i++)
	{
		if (g.v_status[i] == STATUS_ORPHAN) {
			for (size_t j = 0; j < g.adj_list[i].size(); j++)
			{
				if (g.v_status[g.adj_list[i][j]] == STATUS_REMOVED || g.v_status[g.adj_list[i][j]] == STATUS_ORPHAN) {
					g.v_status[i] = STATUS_REMOVED;
					currentTree[i] = g.adj_list[i][j];
					break;
				}
			}
		}
	}
}

void TreeDepth(Graph &g, int depth) {
	while (true)
	{
		depth++;
		if (depth >= bestTree[0] || tle) return;
		int n = 0, maxDeg = -1, v = 0, minDeg = N;
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
				// Vertices without edges can be purged immediately
				if (deg == 0) {
					currentTree[i] = currentRoot;
					g.v_status[i] = STATUS_REMOVED;
					rematch = true;
					continue;
				}
				n++;
				if (deg < minDeg) minDeg = deg;
			}
		}
		if (rematch) match_parents(g);

		// Shrinking complete, log solution
		if (n == 0) {
			if (depth < bestTree[0]) {
				for (int i = 1; i <= N; i++)
				{
					if (g.v_status[i] == STATUS_ORPHAN) currentTree[i] = currentRoot;
					bestTree[i] = currentTree[i];
				}
				bestTree[0] = depth;
			}
			return;
		}
		if (tle) return;

		// If the graph is complete, build the remaining tree in any order
		if (minDeg == n - 1) {
			if (depth + n >= bestTree[0]) return;
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
			return;
		}

		// Remove a vertex that is connected to all other vertices
		if (maxDeg == n - 1) {
			currentTree[v] = currentRoot;
			currentRoot = v;
			g.v_status[v] = STATUS_ROOT;
			int deg = g.adj_list[v].size();
			for (int i = deg - 1; i >= 0; i--)
			{
				int other = g.adj_list[v][i];
				remove_value(g.adj_list[other], v);
			}
			continue;
		}

		// Find a maximal independent set
		for (size_t i = 0; i < order.size() && !tle; i++)
		{
			v = order[i].second;
			if (g.v_status[v] == STATUS_ACTIVE)
				contract_vertex(g, v);
		}
	}
}

int main()
{
	signal(SIGTERM, term);

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
			currentTree = vi(N + 1, 0);
			bestTree = vi(N + 1, 0);
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
	for (int i = 1; i <= N; i++)
	{
		order.emplace_back(original_graph.adj_list[i].size(), i);
	}
	sort(order.begin(), order.end());
	Graph g2 = original_graph;
	while (!tle)
	{
		currentRoot = 0;
		TreeDepth(g2, 0);
		if (tle) break;
		ii tmp;
		for (int swp, i = 0; i < N; i++)
		{
			swp = rand() % N;
			tmp = order[swp];
			order[swp] = order[i];
			order[i] = tmp;
		}
		g2 = original_graph;
	}
	//cout << TreeDepth(original_graph, 0) << endl;
	cout << bestTree[0] + 1 << endl;
	for (int i = 1; i <= N; i++)
	{
		//cerr << i << ": ";
		cout << bestTree[i] << endl;
	}
}

