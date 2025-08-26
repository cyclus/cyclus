#ifndef CYCLUS_SRC_PROGRESS_BAR_H_
#define CYCLUS_SRC_PROGRESS_BAR_H_

#include <string>
#include <iostream>
#include <iomanip>

namespace cyclus {

/// @brief A progress bar class that displays progress at the bottom of the
/// terminal while allowing other output to print above it.
///
/// The ProgressBar provides visual feedback on simulation progress with
/// features like:
/// - Visual progress bar with percentage and fraction display
/// - Smart update frequency based on simulation duration
/// - Automatic disabling when verbose logging is enabled
/// - Configurable via environment variables
/// - Non-intrusive operation alongside existing logging output
///
/// @section progress_bar_usage Usage
///
/// The progress bar is automatically integrated into the Cyclus simulation loop
/// and will appear during simulation runs. It can be controlled via:
///
/// @subsection env_vars Environment Variables
/// - @c CYCLUS_PROGRESS_BAR: Control whether the progress bar is shown
///   - Set to @c 0, @c false, @c no, or @c off to disable
///   - Any other value (including unset) enables the progress bar (default)
///
/// @subsection auto_disable Automatic Disabling
/// The progress bar is automatically disabled when:
/// - Verbose logging is enabled (e.g., @c -v 2 or higher verbosity levels)
/// - This prevents interference with verbose debug output
///
/// @subsection update_freq Update Frequency
/// The progress bar automatically adjusts its update frequency:
/// - Short simulations (≤20 timesteps): Updates every timestep
/// - Medium simulations (21-100 timesteps): Updates every 5 timesteps
/// - Long simulations (>100 timesteps): Updates every 10 timesteps
///
/// @section progress_bar_example Example
/// @code
/// // Progress bar will be automatically disabled with verbose logging
/// cyclus input.xml -v 2 -o test.sqlite
///
/// // Normal run with progress bar
/// cyclus input.xml -o test.sqlite
/// @endcode
///
/// @section progress_bar_impl Implementation Details
/// The progress bar uses ANSI escape codes to update in place and works with
/// the existing logging system. It integrates with the Timer class to provide
/// real-time progress updates during simulation execution.
class ProgressBar {
 public:
  /// @brief Constructor for ProgressBar
  /// @param total The total number of steps (simulation duration)
  /// @param width The width of the progress bar in characters (default: 50)
  /// @param show_percentage Whether to show percentage (default: true)
  /// @param show_fraction Whether to show fraction (default: true)
  ///
  /// Creates a new progress bar with the specified parameters. The progress bar
  /// will automatically check environment variables and verbose logging
  /// settings to determine if it should be enabled.
  ProgressBar(int total, int width = 50, bool show_percentage = true,
              bool show_fraction = true);

  /// @brief Destructor - ensures the progress bar is cleared from the terminal
  ///
  /// Automatically clears the progress bar from the terminal when the object
  /// is destroyed, preventing leftover progress bar text.
  ~ProgressBar();

  /// @brief Update the progress bar to the current step
  /// @param current The current step (0 to total)
  ///
  /// Updates the progress bar display to show the current progress. The update
  /// frequency is controlled by the internal frequency counter to reduce output
  /// noise. The progress bar will always update when reaching the final step.
  void Update(int current);

  /// @brief Set the total number of steps
  /// @param total The new total number of steps
  ///
  /// Updates the total number of steps and redraws the progress bar if enabled.
  /// This is useful if the simulation duration changes during execution.
  void SetTotal(int total);

  /// @brief Clear the progress bar from the terminal
  ///
  /// Removes the progress bar from the terminal display and moves the cursor
  /// to the next line. This is automatically called by the destructor.
  void Clear();

  /// @brief Get the current progress as a percentage
  /// @return Progress percentage (0.0 to 100.0)
  ///
  /// Calculates and returns the current progress as a percentage of completion.
  /// Returns 0.0 if the total is invalid (≤0).
  double GetPercentage() const;

  /// @brief Check if progress bar is enabled globally
  /// @return true if progress bar should be shown
  ///
  /// Returns the global enabled state of progress bars. This is affected by
  /// environment variables and verbose logging settings.
  static bool IsEnabled();

  /// @brief Enable or disable progress bar globally
  /// @param enabled Whether to enable progress bars
  ///
  /// Sets the global enabled state for all progress bars. This can be used
  /// to programmatically control progress bar visibility.
  static void SetEnabled(bool enabled);

  /// @brief Set the update frequency (only update every N calls)
  /// @param frequency Update frequency (1 = every call, 10 = every 10th call,
  /// etc.)
  ///
  /// Controls how often the progress bar updates its display. Higher values
  /// reduce output noise but make the progress bar less responsive. The Timer
  /// class automatically sets appropriate values based on simulation duration.
  void SetUpdateFrequency(int frequency);

  /// @brief Get the current update frequency
  /// @return Current update frequency
  ///
  /// Returns the current update frequency setting. This can be used to query
  /// the current update behavior of the progress bar.
  int GetUpdateFrequency() const;

 private:
  /// @brief Draw the progress bar to the terminal
  ///
  /// Internal method that renders the progress bar to the terminal using ANSI
  /// escape codes. Only redraws if the progress bar content has changed.
  void Draw();

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
