/* C++ World Clock: 2025 Edition 
By Ken Burchfiel
Released under the MIT License
*/

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

std::string
get_tz_time(const std::string &tz_string,
            const std::chrono::time_point<std::chrono::system_clock,
                              std::chrono::seconds> &current_time) {
  // Note: I found the actual type of system_clock::now() from
  // https://en.cppreference.com/w/cpp/chrono/system_clock/now .

  // The following code was based on
  // https://en.cppreference.com/w/cpp/chrono/zoned_time.html .

  std::chrono::zoned_time tz_time{tz_string, current_time};

  // The following line was based on
  // https://stackoverflow.com/a/68754043/13097194
  // and https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html .
  // ISO8601 approach: (Probably overkill for this use case, though)
  // return std::format("{:%FT%T%z}", tz_time);
  return std::format("{:%T (%F) (%z)}", tz_time);

  // return tz_time;
}

void print_tzs(const std::vector<std::vector<std::string>> &tz_vec) {

  // Retrieving the current time, then rounding it down to the
  // nearest second in order to simplify the output:
  // This code was based on
  // https://en.cppreference.com/w/cpp/chrono/system_clock/now.html .
  // I found that adding std::chrono::seconds as my duration argument
  // helped ensure that the times printed by this function would
  // indeed show seconds as integers rather than floats.
  // std::chrono::time_point<std::chrono::system_clock>
  std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
      current_time = std::chrono::floor<std::chrono::seconds>(
          std::chrono::system_clock::now());

  // The following line was based on
  // https://en.cppreference.com/w/cpp/chrono/time_point.html and
  // https://en.cppreference.com/w/cpp/chrono/time_point/time_since_epoch .
  auto unix_time_s = std::chrono::duration_cast<std::chrono::seconds>(
                         current_time.time_since_epoch())
                         .count();
  // Printing out all time zone data:
  // Creating a single string that can store all time zones:
  // This approach *should* reduce the likelihood that the 
  // terminal's cursor will 'flicker' from quickly printing
  // out time zones line by line.
  std::string tz_display = ""; 

  tz_display += "Unix Time: " + std::to_string(unix_time_s) + "\033[K\n";
  // \033[K clears out any lingering additional text to the right
  // of each line.


  for (const std::vector<std::string> &tz_vec_entry : tz_vec) {

    std::string tz_time_str = get_tz_time(
      tz_vec_entry[0], current_time);
    // To make daytime and nighttime hours easier to distinguish,
    // we'll color times from 8:00:00 to 19:59:59 green,
    // and all othe rtimes magenta. We'll do this by (1) retrieving
    // the hours component of the timestamp in integer form;
    // (2) checking whether this integer is less than 8 or
    // greater than/equal to 20; and then specifying the ANSI escape
    // code that corresponds to the desired color.
    // (These escape codes were retrieved from
    // https://en.wikipedia.org/wiki/ANSI_escape_code#Colors ).
    int tz_time_hours = std::stoi(tz_time_str.substr(
      0, 2));
    std::string time_color = "\033[32m";
    if ((tz_time_hours < 8) || (tz_time_hours >= 20)) {
      time_color = "\033[36m";
    }

    
    tz_display += tz_vec_entry[1] + ": " + time_color + tz_time_str
              + "\033[0m\033[K\n";
  }

    // Clearing out the rest of the screen (which may be necessary
    // if the window had been resized), then returning to the 
    // top of the terminal:
    tz_display += "\033[J\033[1;1H";

  std::cout << tz_display;

}

int main() {
  // Clearing the screen:
  // (For reference, see https://en.wikipedia.org/wiki/ANSI_escape_code)
  std::cout << "\033[1;1H";
  std::cout << "\033[J";

  std::vector<std::vector<std::string>> tz_vec;

  // Reading time zone database codes and user-specified titles
  // for each time zone from tz_list.csv:
  // Note: much of this input code was based on the examples
  // found at https://en.cppreference.com/w/cpp/string/basic_string/getline .
  std::ifstream tz_list_file{"../tz_list.csv"};
  // Retrieving each line within the .csv file:
  int line_count = 0;
  for (std::string tz_list_pair;
       std::getline(tz_list_file, tz_list_pair);) { 
    // Discarding the header
        if (line_count != 0) {
      std::istringstream tz_list_pair_inputs{tz_list_pair};
      // Creating a vector that can store each entry within the file:
      // (There's likely a way to skip this step and add items
      // directly to tz_vec, but this approach will be more
      // flexible and can thus be repurposed for other programs
      // that read in .csv data.
      std::vector<std::string> tz_pair_vector{};
      for (std::string item; std::getline(
        tz_list_pair_inputs, item, ',');) {
        tz_pair_vector.push_back(item);
      }
      tz_vec.push_back(tz_pair_vector);
    }

    line_count++;
  }

  while (true) {
    // We'll want to next run the script as close to the top of
    // the following second as possible--which can be accomplished
    // by first rounding a system clock down to get to the
    // current second; adding exactly one second to it; and then
    // pausing the script until that second.
    // This code was based on:
    // https://en.cppreference.com/w/cpp/chrono/time_point/round ;
    // Bames53's response at
    // https://stackoverflow.com/a/9747668/13097194 ;
    // https://cppreference.net/cpp/chrono/time_point.html;
    // https://en.cppreference.com/w/cpp/chrono/duration.html ;
    // and
    // https://en.cppreference.com/w/cpp/thread/sleep_until.html .
    // See also: https://stackoverflow.com/a/79802383/13097194

    auto next_second = std::chrono::floor<std::chrono::seconds>(
                           std::chrono::system_clock::now()) +
                       std::chrono::seconds(1);
    std::this_thread::sleep_until(next_second);
    // The following timing code, which I've since commented out, 
    // allowed me to check how long it took to render a new set of
    // times. On my computer, I tended to get render times
    // around 500-600 microseconds (e.g. 0.5-0.6 milliseconds),
    // but this time might be substantially slower or faster on your
    // own machine.
    // auto start_time = std::chrono::high_resolution_clock::now();
    print_tzs(tz_vec);
    // auto end_time = std::chrono::high_resolution_clock::now();
    // The following runtime calculation is based on p. 587 of
    // Programming: Principles and Practice Using C++ (3rd Edition)
    // by Bjarne Stroustrup.
    // auto run_time = std::chrono::duration_cast<
    // std::chrono::microseconds>(end_time - start_time).count();
    // std::cout << "Render time: " << run_time 
    // << " microseconds\t";

  }
}