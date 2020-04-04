#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;
typedef vector<vi> vvi;

int N, M;

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
// But it will probably do better

enum VSTATUS
{
	STATUS_REMOVED,
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
	for (size_t i = size - 1; i >= 0; i--)
	{
		for (size_t j = 0; j < i; j++)
		{
			add_edge(g, g.adj_list[v][i], g.adj_list[v][j]);
		}
		remove_edge(g, v, g.adj_list[v][i]);
	}
}

int TreeDepth(Graph &g) {
	// TODO
	return 0;
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
	Graph g2 = original_graph;
	cout << TreeDepth(g2) << "\n";
}

