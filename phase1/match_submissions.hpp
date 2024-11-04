#include <array>
#include <iostream>
#include <vector>
#include <span>
#include <cmath>

// Code Overview:
// The program checks for similar patterns in two code submissions represented as integer sequences.
// It has two main phases:
// 1. Find exact matches of patterns between the submissions.
// 2. Find approximate matches for longer patterns missed in the first pass, using a sliding window.

bool approximate_match(const std::vector<int>& seq1, size_t start1,
                       const std::vector<int>& seq2, size_t start2,
                       size_t length, double threshold = 0.80) {
    // Ensure we don't go out of bounds for the sequences
    if (start1 + length > seq1.size() || start2 + length > seq2.size()) {
        return false;
    }
    
    int matches = 0;
    // Check each position in the window for exact matches
    for (size_t i = 0; i < length; i++) {
        if (seq1[start1 + i] == seq2[start2 + i]) {
            matches++;
        }
    }
    // Return true if match percentage is above threshold
    return (static_cast<double>(matches) / length) >= threshold;
}

size_t exact_match_length(const std::vector<int>& seq1, size_t start1,
                          const std::vector<int>& seq2, size_t start2) {
    size_t length = 0;
    // Count consecutive exact matches from the starting point
    while (start1 + length < seq1.size() && 
           start2 + length < seq2.size() && 
           seq1[start1 + length] == seq2[start2 + length]) {
        length++;
    }
    return length;  // Length of exact matching pattern
}

// Main function to compare two submissions
std::array<int, 5> match_submissions(std::vector<int>& submission1,
                                     std::vector<int>& submission2) {
    // Result array: [0] match found?, [1] total match length, 
    // [2] longest approx match, [3] start index 1, [4] start index 2
    std::array<int, 5> result = {0, 0, 0, 0, 0};

    // Define parameters
    const size_t MIN_PATTERN_LENGTH = 10;
    const size_t SHORT_PATTERN_THRESHOLD = 20;
    const double APPROXIMATE_MATCH_THRESHOLD = 0.75;
    const size_t SECOND_PASS_WINDOW = 20;

    // Tracking values for longest matches and their positions
    size_t longest_approx_match = 0;
    size_t best_start1 = 0;
    size_t best_start2 = 0;
    size_t total_match_length = 0;
    std::vector<bool> matched1(submission1.size(), false);  // Tracks matched positions in submission1
    std::vector<bool> matched2(submission2.size(), false);  // Tracks matched positions in submission2
    
    // First pass: Find exact matches
    for (size_t i = 0; i < submission1.size(); i++) {
        if (matched1[i]) continue;  // Skip if already matched
        
        for (size_t j = 0; j < submission2.size(); j++) {
            if (matched2[j]) continue;  // Skip if already matched
            
            // Check if the sequences match starting from i and j
            if (submission1[i] == submission2[j]) {
                size_t match_length = exact_match_length(submission1, i, submission2, j);
                
                if (match_length >= MIN_PATTERN_LENGTH) {
                    // Mark tokens as matched to avoid redundant checks
                    for (size_t k = 0; k < match_length; k++) {
                        matched1[i + k] = true;
                        matched2[j + k] = true;
                    }
                    
                    total_match_length += match_length;  // Accumulate the match length
                    
                    // For long matches, extend them using approximate matching
                    if (match_length > SHORT_PATTERN_THRESHOLD) {
                        size_t extended_length = match_length;
                        size_t window_size = 20;
                        
                        while (i + extended_length < submission1.size() &&
                               j + extended_length < submission2.size()) {
                            if (!approximate_match(submission1, i + extended_length - window_size,
                                                   submission2, j + extended_length - window_size,
                                                   window_size, APPROXIMATE_MATCH_THRESHOLD)) {
                                break;
                            }
                            extended_length++;  // Increment extended match length
                        }
                        
                        // Track the longest approximate match found
                        if (extended_length > longest_approx_match) {
                            longest_approx_match = extended_length;
                            best_start1 = i;
                            best_start2 = j;
                        }
                    }
                }
                
                // Skip the matched section
                if (match_length > 1) {
                    i += match_length - 1;
                    break;
                }
            }
        }
    }
    
    // Second pass: Look for missed approximate matches
    for (size_t i = 0; i < submission1.size(); i++) {
        for (size_t j = 0; j < submission2.size(); j++) {
            if (i + SECOND_PASS_WINDOW <= submission1.size() &&
                j + SECOND_PASS_WINDOW <= submission2.size()) {
                
                // Check for approximate match in this window
                if (approximate_match(submission1, i, submission2, j, 
                                      SECOND_PASS_WINDOW, APPROXIMATE_MATCH_THRESHOLD)) {
                    size_t extended_length = SECOND_PASS_WINDOW;
                    
                    // Extend the match as much as possible
                    while (i + extended_length < submission1.size() &&
                           j + extended_length < submission2.size() &&
                           approximate_match(submission1, i + extended_length - SECOND_PASS_WINDOW,
                                             submission2, j + extended_length - SECOND_PASS_WINDOW,
                                             SECOND_PASS_WINDOW, APPROXIMATE_MATCH_THRESHOLD)) {
                        extended_length++;
                    }
                    
                    // Track the longest approximate match found
                    if (extended_length > longest_approx_match) {
                        longest_approx_match = extended_length;
                        best_start1 = i;
                        best_start2 = j;
                    }
                }
            }
        }
    }
    
    // Determine if there's a significant match based on thresholds
    bool has_significant_matches = (total_match_length >= MIN_PATTERN_LENGTH * 5) || 
                                   (longest_approx_match >= SHORT_PATTERN_THRESHOLD * 1.5);
    
    result[0] = has_significant_matches ? 1 : 0;
    result[1] = static_cast<int>(total_match_length);
    result[2] = (longest_approx_match >= 30) ? static_cast<int>(longest_approx_match) : 0;
    result[3] = static_cast<int>(best_start1);
    result[4] = static_cast<int>(best_start2);
    
    return result;
}