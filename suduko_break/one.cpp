#include <vector>
using namespace std;

class Solution {
private:
    // Helper function to check if placing 'placement' at board[row][col] is valid
    bool isThisPlacementValidAtThisIndex(vector<vector<char>>& board, int row, int col, char placement) {
        for (int i = 0; i < 9; i++) {
            // Check if 'placement' exists in the same column
            if (board[i][col] == placement) {
                return false;
            }
            // Check if 'placement' exists in the same row
            if (board[row][i] == placement) {
                return false;
            }
            // Check if 'placement' exists in the same 3x3 subgrid
            if (board[3 * (row / 3) + i / 3][3 * (col / 3) + i % 3] == placement) {
                return false;
            }
        }
        return true; // Valid placement
    }

    // Recursive function to solve the Sudoku board
    bool solve(vector<vector<char>>& board) {
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[0].size(); j++) {
                // Look for an empty cell
                if (board[i][j] == '.') {
                    // Try placing each number from '1' to '9'
                    for (char placement = '1'; placement <= '9'; placement++) {
                        // Check if placing 'placement' is valid
                        if (isThisPlacementValidAtThisIndex(board, i, j, placement)) {
                            board[i][j] = placement; // Place 'placement'
                            // Recursively attempt to solve with this placement
                            if (solve(board)) {
                                return true;
                            }
                            board[i][j] = '.'; // Undo placement if it leads to failure
                        }
                    }
                    return false; // If no number can be placed in a cell
                }
            }
        }
        return true; // If all the places are solved
    }

public:
    void solveSudoku(vector<vector<char>>& board) {
        solve(board); // Start backtracking
    }
};