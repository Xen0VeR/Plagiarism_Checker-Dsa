#include <vector>
#include <unordered_set>
using namespace std;

class Solution {
private:
    // Initialize the possible candidates for each cell
    vector<vector<unordered_set<char>>> initializeCandidates(vector<vector<char>>& board) {
        vector<vector<unordered_set<char>>> candidates(9, vector<unordered_set<char>>(9));

        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                if (board[row][col] == '.') {
                    unordered_set<char> possibleValues;
                    for (char num = '1'; num <= '9'; num++) {
                        if (isValid(board, row, col, num)) {
                            possibleValues.insert(num);
                        }
                    }
                    candidates[row][col] = possibleValues;
                }
            }
        }
        return candidates;
    }

    // Check if placing 'num' in board[row][col] is valid
    bool isValid(vector<vector<char>>& board, int row, int col, char num) {
        for (int i = 0; i < 9; i++) {
            // Check the row and column
            if (board[row][i] == num || board[i][col] == num) {
                return false;
            }
            // Check the 3x3 box
            int boxRow = 3 * (row / 3) + i / 3;
            int boxCol = 3 * (col / 3) + i % 3;
            if (board[boxRow][boxCol] == num) {
                return false;
            }
        }
        return true;
    }

    // Solve the Sudoku using forward-checking and backtracking
    bool forwardChecking(vector<vector<char>>& board, vector<vector<unordered_set<char>>>& candidates) {
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                if (board[row][col] == '.') { // Found an empty cell
                    for (char num : candidates[row][col]) {
                        board[row][col] = num; // Place the candidate
                        
                        // Update candidates after placement
                        vector<vector<unordered_set<char>>> updatedCandidates = candidates;
                        if (updateCandidates(board, updatedCandidates, row, col, num) &&
                            forwardChecking(board, updatedCandidates)) {
                            return true; // Continue solving if successful
                        }
                        
                        board[row][col] = '.'; // Undo placement
                    }
                    return false; // No valid placements possible
                }
            }
        }
        return true; // Solved the board
    }

    // Update the candidates after placing a number
    bool updateCandidates(vector<vector<char>>& board, vector<vector<unordered_set<char>>>& candidates, int row, int col, char num) {
        for (int i = 0; i < 9; i++) {
            // Remove 'num' from the row and column candidates
            candidates[row][i].erase(num);
            candidates[i][col].erase(num);

            // Remove 'num' from the 3x3 box candidates
            int boxRow = 3 * (row / 3) + i / 3;
            int boxCol = 3 * (col / 3) + i % 3;
            candidates[boxRow][boxCol].erase(num);

            // If any cell has no valid candidates left, return false
            if (board[row][i] == '.' && candidates[row][i].empty()) return false;
            if (board[i][col] == '.' && candidates[i][col].empty()) return false;
            if (board[boxRow][boxCol] == '.' && candidates[boxRow][boxCol].empty()) return false;
        }
        return true;
    }

public:
    void solveSudoku(vector<vector<char>>& board) {
        vector<vector<unordered_set<char>>> candidates = initializeCandidates(board); // Initialize the candidates
        forwardChecking(board, candidates); // Start solving using forward-checking
    }
};