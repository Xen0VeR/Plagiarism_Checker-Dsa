#include "plagiarism_checker.hpp"

// Define the static member in the cpp file
std::mutex plagiarism_checker_t::file_mutex;

plagiarism_checker_t::plagiarism_checker_t(void) : running(true) {
    worker = std::thread(&plagiarism_checker_t::worker_thread, this);
}

plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions) 
    : running(true) {
    // Process initial submissions with timestamp 0
    std::lock_guard<std::mutex> file_lock(file_mutex); // Lock file operations for initial processing
    std::unique_lock<std::shared_mutex> lock(submissions_mutex);
    
    for (auto& submission : __submissions) {
        SubmissionInfo info{
            submission,
            std::chrono::system_clock::time_point(),
            get_submission_tokens(submission->codefile)
        };
        all_submissions.push_back(info);
    }
    lock.unlock();
    worker = std::thread(&plagiarism_checker_t::worker_thread, this);
}

plagiarism_checker_t::~plagiarism_checker_t(void) {
    running = false;
    if (worker.joinable()) {
        worker.join();
    }
}

void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    std::vector<int> tokens;
    {
        std::lock_guard<std::mutex> file_lock(file_mutex); // Protect file operations
        tokens = get_submission_tokens(__submission->codefile);
    }
    
    SubmissionInfo info{
        __submission,
        std::chrono::system_clock::now(),
        std::move(tokens)
    };
    
    std::lock_guard<std::mutex> queue_lock(queue_mutex);
    submission_queue.push(std::move(info));
}

std::vector<int> plagiarism_checker_t::get_submission_tokens(const std::string& filename) {
    // File mutex is already held by calling function
    tokenizer_t tokenizer(filename);
    return tokenizer.get_tokens();
}

bool plagiarism_checker_t::check_single_plagiarism(
    const std::vector<int>& tokens1,
    const std::vector<int>& tokens2,
    int& match_count) {
    
    match_count = 0;
    const int MIN_MATCH_LENGTH = 15;
    
    for (size_t i = 0; i < tokens1.size(); ++i) {
        for (size_t j = 0; j < tokens2.size(); ++j) {
            size_t k = 0;
            while (i + k < tokens1.size() && j + k < tokens2.size() && 
                   tokens1[i + k] == tokens2[j + k]) {
                k++;
            }
            if (k >= MIN_MATCH_LENGTH) {
                match_count++;
                if (k >= 75) return true;
            }
        }
    }
    
    return match_count >= 10;
}

bool plagiarism_checker_t::check_patchwork_plagiarism(const SubmissionInfo& submission) {
    int total_matches = 0;
    
    std::shared_lock<std::shared_mutex> lock(submissions_mutex);
    for (const auto& existing : all_submissions) {
        if (existing.submission->id == submission.submission->id) continue;
        
        if (existing.timestamp > submission.timestamp && 
            std::chrono::duration_cast<std::chrono::seconds>(
                existing.timestamp - submission.timestamp).count() > 1) {
            continue;
        }
        
        int match_count = 0;
        check_single_plagiarism(submission.tokens, existing.tokens, match_count);
        total_matches += match_count;
        
        if (total_matches >= 20) return true;
    }
    
    return false;
}

void plagiarism_checker_t::process_submission(SubmissionInfo& new_submission) {
    bool is_plagiarized = false;
    
    {
        std::shared_lock<std::shared_mutex> lock(submissions_mutex);
        for (const auto& existing : all_submissions) {
            if (existing.submission->id == new_submission.submission->id) continue;
            
            int match_count = 0;
            bool single_plagiarism = check_single_plagiarism(
                new_submission.tokens, existing.tokens, match_count);
                
            if (single_plagiarism) {
                auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(
                    new_submission.timestamp - existing.timestamp).count();
                    
                if (time_diff >= 1 || existing.timestamp == std::chrono::system_clock::time_point()) {
                    if (new_submission.submission->student)
                        new_submission.submission->student->flag_student(new_submission.submission);
                    if (new_submission.submission->professor)
                        new_submission.submission->professor->flag_professor(new_submission.submission);
                    is_plagiarized = true;
                    break;
                } else if (time_diff > -1) {
                    if (new_submission.submission->student)
                        new_submission.submission->student->flag_student(new_submission.submission);
                    if (new_submission.submission->professor)
                        new_submission.submission->professor->flag_professor(new_submission.submission);
                    if (existing.submission->student)
                        existing.submission->student->flag_student(existing.submission);
                    if (existing.submission->professor)
                        existing.submission->professor->flag_professor(existing.submission);
                    is_plagiarized = true;
                    break;
                }
            }
        }
    }
    
    if (!is_plagiarized && check_patchwork_plagiarism(new_submission)) {
        if (new_submission.submission->student)
            new_submission.submission->student->flag_student(new_submission.submission);
        if (new_submission.submission->professor)
            new_submission.submission->professor->flag_professor(new_submission.submission);
    }
    
    {
        std::unique_lock<std::shared_mutex> lock(submissions_mutex);
        all_submissions.push_back(new_submission);
    }
}

void plagiarism_checker_t::worker_thread() {
    while (running) {
        SubmissionInfo current_submission;
        bool has_submission = false;
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (!submission_queue.empty()) {
                current_submission = std::move(submission_queue.front());
                submission_queue.pop();
                has_submission = true;
            }
        }
        
        if (has_submission) {
            process_submission(current_submission);
        } else {
            std::this_thread::yield();
        }
    }
    
    // Process remaining submissions before shutting down
    while (true) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (submission_queue.empty()) break;
        
        SubmissionInfo current_submission = std::move(submission_queue.front());
        submission_queue.pop();
        process_submission(current_submission);
    }
}