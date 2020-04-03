#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
typedef vector<int> vi;
typedef vector<vi> vvi;
#define rep(a,b) for(a=0;a<b;a++)

int N, M;
vvi edge_list;

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
			rep(i, N + 1) {
				edge_list.push_back(vi());
			}
			continue;
		}
		iss >> i >> j;
		edge_list[i].push_back(j);
		edge_list[j].push_back(i);
		if (++count == M) break;
	}
	cout << edge_list[0][0] << "\n";
}

