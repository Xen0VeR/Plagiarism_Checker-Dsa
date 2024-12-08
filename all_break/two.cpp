//{ Driver Code Starts
#include <bits/stdc++.h>
using namespace std;

class Solution {
  public:
    // Function to detect cycle in a directed graph using a recursive DFS approach.
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

    // Helper function to detect cycle using DFS recursively
    bool isCyclicUtil(int node, vector<vector<int>>& adj, vector<bool>& visited, vector<bool>& recStack) {
        visited[node] = true;
        recStack[node] = true;

        // Recur for all the vertices adjacent to this node
        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                if (isCyclicUtil(neighbor, adj, visited, recStack)) {
                    return true;  // Cycle detected
                }
            } else if (recStack[neighbor]) {
                return true; // Cycle detected
            }
        }

        recStack[node] = false; // Backtrack
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
