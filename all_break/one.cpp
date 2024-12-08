//{ Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

class Solution {
  public:
    // Function to detect cycle in a directed graph using an iterative approach.
    bool isCyclic(int V, vector<vector<int>> adj) {
        vector<bool> visited(V, false), recStack(V, false);
        
        // Iterating over all vertices
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                if (isCyclicUtil(i, adj, visited, recStack)) {
                    return true;  // Cycle found
                }
            }
        }
        return false;  // No cycle found
    }

    // Helper function to detect cycle using DFS iteratively
    bool isCyclicUtil(int node, vector<vector<int>>& adj, vector<bool>& visited, vector<bool>& recStack) {
        stack<pair<int, vector<int>::iterator>> nodeStack;
        nodeStack.push({node, adj[node].begin()});
        recStack[node] = true;

        while (!nodeStack.empty()) {
            int curr = nodeStack.top().first;
            auto& it = nodeStack.top().second;

            if (it != adj[curr].end()) {
                int neighbor = *it++;
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    recStack[neighbor] = true;
                    nodeStack.push({neighbor, adj[neighbor].begin()});
                } else if (recStack[neighbor]) {
                    return true; // Cycle detected
                }
            } else {
                recStack[curr] = false;
                nodeStack.pop();
            }
        }
        return false;
    }
};

//{ Driver Code Starts.

int main() {

    int t;
    cin >> t;
    while (t--) {
        int V, E;
        cin >> V >> E;

        vector<vector<int>> adj(V);

        for (int i = 0; i < E; i++) {
            int u, v;
            cin >> u >> v;
            adj[u].push_back(v);
        }

        Solution obj;
        cout << obj.isCyclic(V, adj) << "\n";

        cout << "~"
             << "\n";
    }

    return 0;
}

// } Driver Code Ends
