/* C++ World Clock: 2025 Edition [Still a work in progress!]
By Ken Burchfiel
Released under the MIT License 

// To-dos:
1. Allow users to specify time zones via a config file that the
script will read in
3. Allow users to choose whether or not to show the Unix time entry

(For your config file, you could place the time zone list at the 
bottom and all other entries before it. Each entry could be preceded
by an explanation, and you could hard-code the row numbers
in your script for easier parsing. For instance, row 1 could be 
"Show unix time", row 2 could be "true" or "false", 
rows 3 and 4 could be an explanation/entry pair for some other
configuration, row 5 could be "Time Zones", and rows 6 onward could
be all the time zones. (The script would then loop from 5 to the end
of all lines within the file to gather users' time zones.)

*/


#include <chrono>
#include <format>
#include <iostream>
#include <vector>
#include <thread>
#include <string>

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


std::string get_tz_time(
    std::string tz_string,
    std::chrono::time_point<std::chrono::system_clock, 
    std::chrono::seconds> current_time) {
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


  //return tz_time;
}

void print_tzs(std::vector<std::string> tz_string_list) {

  // Retrieving the current time, then rounding it down to the 
  // nearest second in order to simplify the output:
  // This code was based on
  // https://en.cppreference.com/w/cpp/chrono/system_clock/now.html .
  // I found that adding std::chrono::seconds as my duration argument
  // helped ensure that the times printed by this function would
  // indeed show seconds as integers rather than floats.
  // std::chrono::time_point<std::chrono::system_clock>
  std::chrono::time_point<std::chrono::system_clock, 
  std::chrono::seconds> current_time =
  std::chrono::floor<std::chrono::seconds>(
  std::chrono::system_clock::now());

  // For debugging:
  // std::cout << "Current UTC time: " << std::chrono::system_clock::now() << "\n";
  //std::cout << ty

  // The following line was based on
  // https://en.cppreference.com/w/cpp/chrono/time_point.html and
  // https://en.cppreference.com/w/cpp/chrono/time_point/time_since_epoch .
  auto unix_time_s = std::chrono::duration_cast<std::chrono::seconds>(
    current_time.time_since_epoch()).count();
  std::cout << "Unix Time: " <<  unix_time_s << "\033[K\n";
    // \033[K clears out any lingering additional text to the right
    // of each line.
  // Printing out all time zone data:
  for (int i = 0; i < tz_string_list.size(); i++) {
    std::string tz_name_str = tz_string_list[i];
    std::string tz_time_str = get_tz_time(tz_name_str, current_time);
    // To make daytime and nighttime hours easier to distinguish, 
    // we'll color times from 8:00:00 to 19:59:59 green,
    // and all othe rtimes magenta. We'll do this by (1) retrieving
    // the hours component of the timestamp in integer form;
    // (2) checking whether this integer is less than 8 or 
    // greater than/equal to 20; and then specifying the ANSI escape
    // code that corresponds to the desired color.
    // (These escape codes were retrieved from
    // https://en.wikipedia.org/wiki/ANSI_escape_code#Colors ).
    int tz_time_hours = std::stoi(tz_time_str.substr(0, 2));
    std::string time_color = "\033[32m";
    if ((tz_time_hours < 8) || (tz_time_hours >= 20)) 
      {time_color = "\033[36m";}
    std::cout << tz_string_list[i] << ": " << time_color
    << tz_time_str << "\033[0m" << "\033[K\n";
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
    std::chrono::system_clock::now()) + std::chrono::seconds(1);
    std::this_thread::sleep_until(next_second);
    
    print_tzs(tz_string_list); 
    // Clearing out the rest of the screen (which may be necessary
    // if the window had been resized):
    std::cout << "\033[J";
    // Returning to the top of the terminal:
    std::cout << "\033[1;1H";

  }
}