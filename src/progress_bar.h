#ifndef CYCLUS_SRC_PROGRESS_BAR_H_
#define CYCLUS_SRC_PROGRESS_BAR_H_

#include <string>
#include <iostream>
#include <iomanip>

namespace cyclus {

/// A progress bar class that displays progress at the bottom of the terminal
/// while allowing other output to print above it.
class ProgressBar {
 public:
  /// Constructor
  /// @param total The total number of steps
  /// @param width The width of the progress bar in characters (default: 50)
  /// @param show_percentage Whether to show percentage (default: true)
  /// @param show_fraction Whether to show fraction (default: true)
  ProgressBar(int total, int width = 50, bool show_percentage = true, 
              bool show_fraction = true);

  /// Destructor - ensures the progress bar is cleared
  ~ProgressBar();

  /// Update the progress bar to the current step
  /// @param current The current step (0 to total)
  void Update(int current);

  /// Update the progress bar by incrementing by one step
  void Increment();

  /// Set the total number of steps
  /// @param total The new total
  void SetTotal(int total);

  /// Set the current step
  /// @param current The current step
  void SetCurrent(int current);

  /// Clear the progress bar from the terminal
  void Clear();

  /// Get the current progress as a percentage
  /// @return Progress percentage (0.0 to 100.0)
  double GetPercentage() const;

  /// Check if progress bar is enabled
  /// @return true if progress bar should be shown
  static bool IsEnabled();

  /// Enable or disable progress bar globally
  /// @param enabled Whether to enable progress bars
  static void SetEnabled(bool enabled);

  /// Set the update frequency (only update every N calls)
  /// @param frequency Update frequency (1 = every call, 10 = every 10th call, etc.)
  void SetUpdateFrequency(int frequency);

  /// Get the current update frequency
  /// @return Current update frequency
  int GetUpdateFrequency() const;

 private:
  /// Draw the progress bar to the terminal
  void Draw();

  /// Move cursor up one line
  void MoveUp();

  /// Move cursor down one line
  void MoveDown();

  /// Clear the current line
  void ClearLine();

  int total_;
  int current_;
  int width_;
  bool show_percentage_;
  bool show_fraction_;
  bool is_drawn_;
  std::string last_bar_;
  int update_frequency_;
  int update_counter_;

  static bool enabled_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PROGRESS_BAR_H_
