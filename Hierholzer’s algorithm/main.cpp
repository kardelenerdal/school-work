#include <iostream>
#include <queue>
#include <vector>
#include <list>
#include <fstream>
using namespace std;

struct Vertex {
    
    int id;
    queue<int> edges;
    int outdegree;
    int indegree;
    
    Vertex(const int& id) {
        this->id = id;
        this->indegree = 0;
    }

};

// checks if graph is eulerian
bool isEulerian(const vector<Vertex>& vertices) {
    for(int i=0; i<vertices.size(); i++){
        if(vertices[i].outdegree != vertices[i].indegree){
            return false;
        }
    }
    return true;
}

int main(int argc, char const *argv[]) {

    FILE* infile = fopen(argv[1], "r");
    FILE* outfile = fopen(argv[2], "w");
    
    int numberOfVertices;
    int numberOfEdges = 0;
    fscanf(infile, "%d", &numberOfVertices);

    vector<Vertex> vertices(numberOfVertices,0);
    
    //input part
    for(int i=0; i<numberOfVertices; i++) {
        int id;
        int outdegree;
        fscanf(infile, "%d %d", &id, &outdegree);
        vertices[id].id = id;
        vertices[id].outdegree = outdegree;
        if(outdegree < vertices[id].indegree) {
            fprintf(outfile, "%s", "no path");
            return 0;
        }
        for(int j=0; j<outdegree; j++){
            numberOfEdges++;
            int edge;
            fscanf(infile, "%d", &edge);
            vertices[id].edges.push(edge);
            vertices[edge].indegree++;
        }
    }
  
    int startingIndex;
    fscanf(infile, "%d", &startingIndex);

    // the algorithm
    list<int> eulerianCircuit;
    int v = startingIndex;
    int idx = 1;
    
    if(!isEulerian(vertices)) {
        fprintf(outfile, "%s", "no path");
    } else {

        eulerianCircuit.push_back(v);
        Vertex* currentVertex = &vertices[v];
        list <int> :: iterator mem;
        mem = eulerianCircuit.begin(); 
        
        while(eulerianCircuit.size() <= numberOfEdges){
            vector<int> tour;
            while(!currentVertex->edges.empty()){
                int nonUsedEdge = currentVertex->edges.front();
                currentVertex->edges.pop();
                v = nonUsedEdge;
                tour.push_back(v);
                currentVertex = &vertices[v];
            }
            // merge
            list <int>::iterator it;
            it = next(mem);
            
            eulerianCircuit.insert(it, tour.begin(), tour.end());
            
            //update
            for(; mem!=eulerianCircuit.end(); ++mem){
                if(!vertices[*mem].edges.empty()) {
                    currentVertex = &vertices[*mem];
                    v = *mem;
                    break;
                }
            }
        }

        //output
        for(auto pr:eulerianCircuit){
            fprintf(outfile, "%d ", pr);
        }
        
        fclose(outfile);
        fclose(infile);
     }

    return 0;
}