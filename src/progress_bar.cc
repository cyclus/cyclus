#include "progress_bar.h"
#include "logger.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <algorithm>

namespace cyclus {

bool ProgressBar::enabled_ = true;

// Check environment variable for progress bar control
namespace {
  bool CheckProgressBarEnabled() {
    const char* env_var = std::getenv("CYCLUS_PROGRESS_BAR");
    if (env_var != NULL) {
      std::string val(env_var);
      return (val == "1" || val == "true" || val == "yes" || val == "on");
    }
    
    // Check if verbose logging is enabled - disable progress bar if so
    // LEV_INFO1 is the default level, so if we're above that, we're in verbose mode
    if (cyclus::Logger::ReportLevel() > cyclus::LEV_WARN) {
      return false;
    }
    
    return true;  // Default to enabled
  }
}

ProgressBar::ProgressBar(int total, int width, bool show_percentage, bool show_fraction)
    : total_(total),
      current_(0),
      width_(width),
      show_percentage_(show_percentage),
      show_fraction_(show_fraction),
      is_drawn_(false),
      update_frequency_(1),
      update_counter_(0) {
  if (total_ <= 0) {
    total_ = 1;  // Prevent division by zero
  }
  
  // Check environment variable
  enabled_ = CheckProgressBarEnabled();
}

ProgressBar::~ProgressBar() {
  Clear();
}

void ProgressBar::Update(int current) {
  if (!IsEnabled()) {
    return;
  }

  current_ = std::max(0, std::min(current, total_));
  
  // Check if we should update based on frequency
  update_counter_++;
  if (update_counter_ % update_frequency_ == 0 || current_ == total_) {
    Draw();
  }
}

void ProgressBar::Increment() {
  Update(current_ + 1);
}

void ProgressBar::SetTotal(int total) {
  if (total <= 0) {
    total = 1;  // Prevent division by zero
  }
  total_ = total;
  if (IsEnabled()) {
    Draw();
  }
}

void ProgressBar::SetCurrent(int current) {
  Update(current);
}

void ProgressBar::Clear() {
  if (!IsEnabled() || !is_drawn_) {
    return;
  }

  // Clear the line and move to next line
  std::cout << "\r\033[K" << std::endl;
  is_drawn_ = false;
}

double ProgressBar::GetPercentage() const {
  if (total_ <= 0) {
    return 0.0;
  }
  return (static_cast<double>(current_) / total_) * 100.0;
}

bool ProgressBar::IsEnabled() {
  return enabled_;
}

void ProgressBar::SetEnabled(bool enabled) {
  enabled_ = enabled;
}

void ProgressBar::SetUpdateFrequency(int frequency) {
  if (frequency > 0) {
    update_frequency_ = frequency;
  }
}

int ProgressBar::GetUpdateFrequency() const {
  return update_frequency_;
}

void ProgressBar::Draw() {
  if (!IsEnabled()) {
    return;
  }

  // Calculate progress
  double percentage = GetPercentage();
  int filled_width = static_cast<int>((percentage / 100.0) * width_);
  
  // Build the progress bar string
  std::stringstream ss;
  ss << "\r[";
  
  // Add filled portion
  for (int i = 0; i < filled_width; ++i) {
    ss << "=";
  }
  
  // Add current position indicator
  if (filled_width < width_) {
    ss << ">";
    // Add remaining empty space
    for (int i = filled_width + 1; i < width_; ++i) {
      ss << " ";
    }
  }
  
  ss << "]";
  
  // Add percentage and fraction if requested
  if (show_percentage_) {
    ss << " " << std::fixed << std::setprecision(1) << percentage << "%";
  }
  
  if (show_fraction_) {
    ss << " (" << current_ << "/" << total_ << ")";
  }
  
  std::string bar = ss.str();
  
  // Only redraw if the bar has changed
  if (bar != last_bar_) {
    // Print the new bar and flush immediately
    std::cout << bar << std::flush;
    is_drawn_ = true;
    last_bar_ = bar;
  }
}

void ProgressBar::MoveUp() {
  std::cout << "\033[A" << std::flush;
}

void ProgressBar::MoveDown() {
  std::cout << "\033[B" << std::flush;
}

void ProgressBar::ClearLine() {
  std::cout << "\r\033[K" << std::flush;
}

}  // namespace cyclus
