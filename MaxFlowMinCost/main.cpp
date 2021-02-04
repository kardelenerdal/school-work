#include <iostream>
#include <queue>
#include <utility>
#include <vector>
#include <list>
#include <fstream>
#include <unordered_map>
using namespace std;

struct Vertex {

	int id;
	unordered_map <int, int> edges;     // (vertex_id, voltage)

	Vertex(int id) {
		this->id = id;
	}

};

// https://konaeakira.github.io/posts/using-the-shortest-path-faster-algorithm-to-find-negative-cycles.html
bool detect_cycle(const int pre[], int nofVertices, vector<int>& negCycle){
	
	vector<int> vec;
	bool visited[nofVertices];
	bool on_stack[nofVertices];
	for(int i=0; i<nofVertices; i++){
		on_stack[i] = false;
		visited[i] = false;
	}

	for(int i=0; i<nofVertices; i++){
		if(!visited[i]){
			for(int j=i; j!=-1; j=pre[j]){
				if(!visited[j]){
					visited[j] = true;
					on_stack[j] = true;
					vec.push_back(j);
				} else {
					if(on_stack[j]){
						bool start = false;
						for(int k=0; k<vec.size(); k++){
							if(vec[k] == j && !start){
								negCycle.push_back(vec[k]);
								start = true;
							} else if(start){
								if(vec[k] == j){
									negCycle.push_back(vec[k]);
									break;
								}
								negCycle.push_back(vec[k]);
							}
						}
						return true;
					}
					break;
				}
			}
			for(int j : vec){
				on_stack[j] = false;
			}
			vec.clear();
		}
	}
	return false;
}

// https://konaeakira.github.io/posts/using-the-shortest-path-faster-algorithm-to-find-negative-cycles.html
bool negative(vector<Vertex>& vertices, vector<int>& negCycle) {

	int nofVertices = vertices.size();
	int dis[nofVertices];
	int pre[nofVertices];
	queue<int> que;
	bool in_queue[nofVertices];
	for(int i=0; i<nofVertices; i++){
		dis[i] = 0;
		pre[i] = -1;
		in_queue[i] = true;
		que.push(i);
	}
	int iter = 0;
	while(!que.empty()){
		int u = que.front();
		que.pop();
		in_queue[u] = false;
		for(pair<int,int> edge : vertices.at(u).edges) {
			int weight = edge.second;
			int toEdge = edge.first;
			if(dis[u] + weight < dis[toEdge]){
				pre[toEdge] = u;
				dis[toEdge] = weight + dis[u];
				iter++;
				if(iter == nofVertices){
					iter = 0;
					if(detect_cycle(pre, nofVertices, negCycle)){
						return true;
					} 
				}
				if(!in_queue[toEdge]){
					que.push(toEdge);
					in_queue[toEdge] = true;
				}
			}
		}
	}
	if(detect_cycle(pre, nofVertices, negCycle)){
		return true;
	}
	return false;

}

int main(int argc, char* argv[]) {

	ifstream infile;                            
	infile.open(argv[1]);
	ofstream outfile;
	outfile.open(argv[2]);
    
	int nofTestCases;
	infile >> nofTestCases;

	for(int i=0; i<nofTestCases; i++){

		// INPUT PART
		int n;
		infile >> n;

		int nofVertices = n*2;
		vector<Vertex> vertices(n*2, 0);


		for(int j=0; j<n; j++){
			Vertex v(j);
			for(int k=0; k<n; k++){
				int cost;
				infile >> cost;
				if(k==j){
					Vertex w(j+n);
					w.edges[j] = cost;
					vertices[j+n] = w;
				} else {
					v.edges[n+k] = -1*cost;
				}
			}
			vertices[j] = v;
		}
		// INPUT PART

		vector<int> negCycle;

		// while the graph contains negative cycle
		while(negative(vertices, negCycle)){
			
			for(int j=0; j<negCycle.size()-1; j++){
				int from_edge = negCycle[j+1];
				int to_edge = negCycle[j];
				int minus_cost = -1*(vertices[from_edge].edges[to_edge]);
				
				vertices[from_edge].edges.erase(to_edge);
				vertices[to_edge].edges[from_edge] = minus_cost;
			}
			int lfrom_edge = negCycle[0];
			int lto_edge = negCycle[negCycle.size()-1];
			int lminus_cost = -1*(vertices[lfrom_edge].edges[lto_edge]);
			
			vertices[lfrom_edge].edges.erase(lto_edge);
			vertices[lto_edge].edges[lfrom_edge] = lminus_cost; 
			negCycle.clear();
		}
		int result = 0;
		for(int j=n; j<nofVertices; j++){
			for(pair<int,int> e : vertices[j].edges){
				result += e.second;
			}
		}

		outfile << result << endl;

	}


    return 0;
}
