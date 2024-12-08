#pragma once
#include "../tokenizer.hpp"
#include "structures.hpp"
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <memory>
#include <vector>

struct SubmissionInfo {
    std::shared_ptr<submission_t> submission;
    std::chrono::system_clock::time_point timestamp;
    std::vector<int> tokens;
};

class plagiarism_checker_t {
public:
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    mutable std::shared_mutex submissions_mutex;  // For all_submissions
    std::mutex queue_mutex;                       // For submission_queue
    static std::mutex file_mutex;                 // Global mutex for file operations
    std::thread worker;
    std::atomic<bool> running;
    std::queue<SubmissionInfo> submission_queue;
    std::vector<SubmissionInfo> all_submissions;
    
    void worker_thread();
    bool check_single_plagiarism(const std::vector<int>& tokens1, 
                               const std::vector<int>& tokens2,
                               int& match_count);
    void process_submission(SubmissionInfo& new_submission);
    bool check_patchwork_plagiarism(const SubmissionInfo& submission);
    std::vector<int> get_submission_tokens(const std::string& filename);
};