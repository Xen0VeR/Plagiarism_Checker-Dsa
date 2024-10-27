#include <array>
#include <iostream>
#include <vector>
#include <cmath>

// Helper function to check if two sequences are approximately equal
bool is_approximate_match(const std::vector<int>& seq1, size_t start1,
                          const std::vector<int>& seq2, size_t start2,
                          size_t length, double threshold = 0.85) {
    if (start1 + length > seq1.size() || start2 + length > seq2.size()) {
        return false;
    }
    
    int matches = 0;
    for (size_t i = 0; i < length; i++) {
        if (seq1[start1 + i] == seq2[start2 + i]) {
            matches++;
        }
    }
    // Return whether the match ratio exceeds the threshold
    return (static_cast<double>(matches) / length) >= threshold;
}

// Helper function to find exact match length
size_t get_exact_match_length(const std::vector<int>& seq1, size_t start1,
                              const std::vector<int>& seq2, size_t start2) {
    size_t length = 0;
    while (start1 + length < seq1.size() && 
           start2 + length < seq2.size() && 
           seq1[start1 + length] == seq2[start2 + length]) {
        length++;
    }
    return length;
}

std::array<int, 5> match_submissions(std::vector<int>& submission1,
                                     std::vector<int>& submission2) {
    std::array<int, 5> result = {0, 0, 0, 0, 0};
    const size_t MIN_PATTERN_LENGTH = 10;
    const size_t SHORT_PATTERN_THRESHOLD = 20;
    const double APPROXIMATE_MATCH_THRESHOLD = 0.85;

    // Variables to track the longest approximate match
    size_t longest_approx_match = 0;
    size_t best_start1 = 0;
    size_t best_start2 = 0;
    
    // Total length of all matches
    size_t total_match_length = 0;
    std::vector<bool> matched1(submission1.size(), false);
    std::vector<bool> matched2(submission2.size(), false);
    
    // First pass: Find all exact matches
    for (size_t i = 0; i < submission1.size(); i++) {
        if (matched1[i]) continue;
        
        for (size_t j = 0; j < submission2.size(); j++) {
            if (matched2[j]) continue;
            
            if (submission1[i] == submission2[j]) {
                size_t match_length = get_exact_match_length(submission1, i, submission2, j);
                
                if (match_length >= MIN_PATTERN_LENGTH) {
                    // Mark these tokens as matched
                    for (size_t k = 0; k < match_length; k++) {
                        matched1[i + k] = true;
                        matched2[j + k] = true;
                    }
                    
                    total_match_length += match_length;
                    
                    // Check for approximate matches for longer sequences
                    if (match_length > SHORT_PATTERN_THRESHOLD) {
                        size_t extended_length = match_length;
                        size_t window_size = 20;
                        
                        // Try to extend with approximate matching
                        while (i + extended_length < submission1.size() &&
                               j + extended_length < submission2.size()) {
                            if (!is_approximate_match(submission1, i + extended_length - window_size,
                                                      submission2, j + extended_length - window_size,
                                                      window_size, APPROXIMATE_MATCH_THRESHOLD)) {
                                break;
                            }
                            extended_length++;
                        }
                        
                        if (extended_length > longest_approx_match) {
                            longest_approx_match = extended_length;
                            best_start1 = i;
                            best_start2 = j;
                        }
                    }
                }
                
                // Skip to end of current match
                if (match_length > 1) {
                    i += match_length - 1;
                    break;
                }
            }
        }
    }
    
    // Second pass: Look for approximate matches that might have been missed
    for (size_t i = 0; i < submission1.size(); i++) {
        for (size_t j = 0; j < submission2.size(); j++) {
            if (i + SHORT_PATTERN_THRESHOLD <= submission1.size() &&
                j + SHORT_PATTERN_THRESHOLD <= submission2.size()) {
                
                if (is_approximate_match(submission1, i, submission2, j, SHORT_PATTERN_THRESHOLD, APPROXIMATE_MATCH_THRESHOLD)) {
                    size_t extended_length = SHORT_PATTERN_THRESHOLD;
                    
                    while (i + extended_length < submission1.size() &&
                           j + extended_length < submission2.size() &&
                           is_approximate_match(submission1, i + extended_length - 20,
                                                submission2, j + extended_length - 20,
                                                20, APPROXIMATE_MATCH_THRESHOLD)) {
                        extended_length++;
                    }
                    
                    if (extended_length > longest_approx_match) {
                        longest_approx_match = extended_length;
                        best_start1 = i;
                        best_start2 = j;
                    }
                }
            }
        }
    }
    
    // Determine plagiarism flag based on total and longest approximate matches
    bool has_significant_matches = (total_match_length >= MIN_PATTERN_LENGTH * 2) || 
                                   (longest_approx_match >= SHORT_PATTERN_THRESHOLD * 1.5);
    
    // Set results
    result[0] = has_significant_matches ? 1 : 0;
    result[1] = static_cast<int>(total_match_length);
    result[2] = static_cast<int>(longest_approx_match);
    result[3] = static_cast<int>(best_start1);
    result[4] = static_cast<int>(best_start2);
    
    return result;
}