/* C++ World Clock: 2025 Edition
By Ken Burchfiel
Released under the MIT License */

#include <chrono>
#include <format>
#include <iostream>
#include <vector>
#include <thread>

// Initializing a list of time zone strings:
// These can be found at:
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones .
std::vector<std::string> tz_string_list{
    "US/Samoa",  
    "Pacific/Honolulu",
    "US/Pacific",
    "US/Eastern",
    "America/Puerto_Rico",
    "UTC",        
    "Europe/London",    
    "Europe/Rome",     
    "Asia/Jerusalem",
    "Asia/Calcutta",
    "Asia/Tokyo",
    "Pacific/Guam",
    "Australia/Sydney",
    "Pacific/Kiritimati",};

// Note: I wasn't able to use
std::string get_tz_time(
    std::string tz_string,
    std::chrono::time_point<std::chrono::system_clock> current_time) {
  // Note: I found the actual type of system_clock::now() from
  // https://en.cppreference.com/w/cpp/chrono/system_clock/now .

  // The following code was based on
  // https://en.cppreference.com/w/cpp/chrono/zoned_time.html .

  std::chrono::zoned_time tz_time{tz_string, current_time};

  // The following line was based on
  // https://stackoverflow.com/a/68754043/13097194
  // and https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html .
  return std::format("{:%FT%T%z}", tz_time);
  //return tz_time;
}

void print_tzs(std::vector<std::string> tz_string_list) {

  // Determining the current time:
  // This code was based on
  // https://en.cppreference.com/w/cpp/chrono/system_clock/now.html .
  std::chrono::time_point<std::chrono::system_clock> current_time =
      std::chrono::system_clock::now();
  // The following line was based on https://en.cppreference.com/w/cpp/chrono/time_point.html
  auto unix_time_ns = current_time.time_since_epoch();
  // The following line was based on https://en.cppreference.com/w/cpp/chrono/time_point/time_since_epoch
  auto unix_time_s = std::chrono::duration_cast<std::chrono::seconds>(
    unix_time_ns).count();
  std::cout << current_time.time_since_epoch() << "\n";
  std::cout << unix_time_s << "\n";
  // Printing out all time zone data:
  for (int i = 0; i < tz_string_list.size(); i++) {
    std::cout << tz_string_list[i] << ": "
              << get_tz_time(tz_string_list[i], current_time) << "\n";
  }
}

int main() { 
  // Clearing the screen:
  // (For reference, see https://en.wikipedia.org/wiki/ANSI_escape_code)

  std::cout << "\033[1;1H";
  std::cout << "\033[J";
  while (true)
  {

    // We'll want to next run the script as close to the top of
    // the following second as possible--which can be accomplished
    // by first rounding up a system clock to get to the next second,
    // then pausing the script until that second.
    // This code was based on:
    // https://en.cppreference.com/w/cpp/chrono/time_point/round ,
    // Bames53's response at
    // https://stackoverflow.com/a/9747668/13097194 ,
    // and 
    // https://en.cppreference.com/w/cpp/thread/sleep_until.html .

    auto next_second = std::chrono::ceil<std::chrono::seconds>(
    std::chrono::system_clock::now());
    std::this_thread::sleep_until(next_second);
    // Returning to the top of the console:
    std::cout << "\033[1;1H";
    print_tzs(tz_string_list); 
    //std::cout << "\033[J";


    // std::this_thread::sleep_until(
    // std::chrono::system_clock::now() + std::chrono::seconds(1));
  }
}