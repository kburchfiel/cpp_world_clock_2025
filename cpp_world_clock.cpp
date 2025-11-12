/* C++ World Clock: 2025 Edition
Version 2.1.0
By Ken Burchfiel
Released under the MIT License
*/

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>


// Storing ANSI escape codes for a set of colors:
// (I retrieved these codes from
// https://github.com/agauniyal/rang/blob/master/include/rang.hpp . ).
// The rang repository is released under the Unlicense.
// For the use of 'bright' to differentiate the final eight colors
// from the first eight, see: 
// https://github.com/agauniyal/rang/tree/master
std::map<std::string,std::string> ansi_color_codes{
  {"black","30"},{"red","31"}, 
  {"green","32"}, {"yellow","33"},
  {"blue","34"},{"magenta","35"},
  {"cyan","36"},{"gray","37"},
  {"bright black", "90"}, {"bright red", "91"},
{"bright green", "92"}, {"bright yellow", "93"},
 {"bright blue", "94"}, {"bright magenta", "95"},
{"bright cyan", "96"}, {"bright gray", "97"}};


std::map<std::string, std::string> csv_to_map(std::string csv_file_path) {

  // This function converts a CSV file to a map with string-based
  // keys and values.
  // It assumes that the CSV file has only two columns; that
  // it contains a header; that values are separated by commas;
  // and that no items themselves contain commas.

  // Note: much of this code was based on the examples
  // found at https://en.cppreference.com/w/cpp/string/basic_string/getline .

  std::ifstream csv_ifstream{csv_file_path};

  std::map<std::string, std::string> csv_map;

  // Retrieving each line within the .csv file:

  int line_count = 0;
  for (std::string row_string; std::getline(csv_ifstream, row_string);) {

    // Parsing each individual line (other than the header, which
    // will get skipped):
    if (line_count != 0) {
      std::istringstream row_pair{row_string};

      int field_index = 0; // This value will help us differentiate
      // between configuration variables (whose field_index value
      // will be 0) and values (whose field_index value will be 1).
      // Defining variables that will store the configuration
      // info within the current line:
      std::string map_key = "";
      std::string map_value = "";
      for (std::string item; std::getline(row_pair, item, ',');) {
        if (field_index == 0) {
          map_key = item;
        }
        if (field_index == 1) {
          map_value = item;
        }

        if (field_index > 1) // We don't need to scan any additional
        // rows within the CSV file, so we can break out of the loop
        // at this point.
        {
          break;
        }

        field_index++;
      }
      csv_map[map_key] = map_value;
    }
    line_count++;
  }

  return csv_map;
}

std::vector<std::vector<std::string>> csv_to_vector(std::string csv_file_path) {
  // This function converts a CSV file to a vector of vectors of
  // strings. This function will be a better fit than csv_to_map()
  // for CSV files that have more than two columns, *or* when
  // it's important that the order of the rows be maintained.
  // As with csv_to_map(), this function assumes that the .csv file
  // contains a header; that values are separated by commas;
  // and that no items themselves contain commas.

  // Note: much of this code was based on the examples
  // found at https://en.cppreference.com/w/cpp/string/basic_string/getline .

  std::vector<std::vector<std::string>> csv_vector;

  std::ifstream csv_ifstream{csv_file_path};

  int line_count = 0;
  for (std::string row_string; std::getline(csv_ifstream, row_string);) {

    if (line_count != 0) {
      std::istringstream row_values{row_string};

      std::vector<std::string> row_vector{};
      for (std::string item; std::getline(row_values, item, ',');) {
        row_vector.push_back(item);
      }
      csv_vector.push_back(row_vector);
    }

    line_count++;
  }

  return (csv_vector);
}

std::string
get_tz_time(const std::string &tz_string,
            const std::chrono::time_point<std::chrono::system_clock,
                                          std::chrono::seconds> &current_time,
            std::map<std::string, std::string> &config_map,
            std::string &format_string) {

  // This function returns the time and date at the
  // time zone specified by tz_string in the format specified
  // by format_code.

  // Note: I found the actual type of system_clock::now() from
  // https://en.cppreference.com/w/cpp/chrono/system_clock/now .

  // The following code was based on
  // https://en.cppreference.com/w/cpp/chrono/zoned_time.html .

  std::chrono::zoned_time tz_time{tz_string, current_time};

  // Retrieving the hour of the day for this time zone:
  // (This will involve finding the difference between the time point
  // corresponding to this code and the start of the most recent
  // day, then using duration_cast() to convert that difference
  // to an integer.
  // (Previously, I had extracted the hour of day from the
  // string-formatted version of this time; however, this approach
  // will fail if the hour isn't at the point of the string that
  // we expect (which can occur when a custom format code is
  // being used).
  // This code was based in part on Howard Hinnant's excellent answer at
  // https://stackoverflow.com/a/15958113/13097194
  // and

  auto zoned_tp = tz_time.get_local_time();
  auto days = std::chrono::floor<std::chrono::days>(zoned_tp);
  int tz_time_hours =
      std::chrono::duration_cast<std::chrono::hours>(zoned_tp - days).count();

  // Making daytime and nighttime hours easier to distinguish:
  std::string time_color = "\033[" + config_map["daytime_color"] + "m";
  if ((tz_time_hours < std::stoi(config_map["daytime_start"])) ||
      (tz_time_hours >= std::stoi(config_map["daytime_end"]))) {
    time_color = "\033[" + config_map["nighttime_color"] + "m";
  }

  // Determining how to show dates and time zones:
  // The following line was based on
  // https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html
  // and https://en.cppreference.com/w/cpp/utility/format/runtime_format.html .

  return (time_color +
          std::format(std::runtime_format(format_string), tz_time));
}

void print_tzs(const std::vector<std::vector<std::string>> &tz_vec,
               std::map<std::string, std::string> &config_map,
               std::string format_string) {
  // This function will print a series of time zones and their
  // corresponding times.

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

  // Specifying separate post-time strings for horizontal and
  // vertical display settings:
  // (These will be displayed after each individual time.
  // We should use four spaces following each time when displaying
  // values horizontally and newlines otherwise. Other tweaks
  // will need to be made here and there as well.)

  std::string post_time_string = "\033[0m\033[K\n";
  // \033 allows us to pass ANSI escape sequences to the terminal,
  // thus letting us control colors, line/display clearing, and
  // cursor position. [0m resets colors to their default value,
  // and [K clears out any lingering additional text to the right
  // of each line. These were based on the ANSI escape code
  // documentation
  // at https://en.wikipedia.org/wiki/ANSI_escape_code .

  std::string tz_display_closure = "\033[J\033[1;1H";
  // [J clears out any remaining text in the terminal, which may
  // be helpful following changes to its dimensions.
  // 1;1H returns the cursor to the top left of the terminal so that
  // we'll be in the right position to display the next set
  // of times.

  if (config_map["horizontal_display"] == "true") {
    post_time_string = "\033[0m    ";

    tz_display_closure = "\033[K\033[J\n\033[1;1H";
  }

  // Showing Unix Time (the number of seconds, not including
  // leap seconds, since 1970-01-01 00:00:00) if requested:
  if (config_map["show_unix_time"] == "true") {
    tz_display += "\033[" + config_map["unix_time_name_color"] + "m" +
                  "Unix Time: " + "\033[" + config_map["unix_time_color"] +
                  "m" + std::to_string(unix_time_s) + post_time_string;
  }

  for (const std::vector<std::string> &tz_vec_entry : tz_vec) {

    // Specifying which format code to pass to get_tz_time():

    std::string tz_time_str =
        get_tz_time(tz_vec_entry[1], current_time, config_map, format_string);

    std::string entry_name_color =
        "\033[" + config_map["entry_name_color"] + "m";

    tz_display += entry_name_color + tz_vec_entry[0] + " " + tz_time_str +
                  post_time_string;
  }

  // Clearing out the rest of the screen (which may be necessary
  // if the window had been resized), then returning to the
  // top of the terminal:
  tz_display += tz_display_closure;

  std::cout << tz_display;
}

int main() {
  // Clearing the screen:
  // (For reference, see https://en.wikipedia.org/wiki/ANSI_escape_code)
  std::cout << "\033[1;1H";
  std::cout << "\033[J";

  // Reading in configuration filenames:
  // (This approach makes it easier to swap out different
  // time zone and config files--and to try out different
  // config options for the same set of time zones, or vice versa.)

  // The following map will store two filenames, one for our
  // time zone list, and another for our configuration files.
  // After reading in the time zone and config filenames,
  // the script will then parse the data contained within those two
  // files.

  std::map<std::string, std::string> config_file_map =
      csv_to_map("../config/cwc_config.csv");

  // Parsing the active configuration file:
  std::map<std::string, std::string> config_map =
      csv_to_map("../config/" + config_file_map["config_list"]);

  // Reading time zone database codes and user-specified titles
  // for each time zone from tz_list:
  // (csv_to_vector() will be used here so that we can maintain
  // the original order in which time zones were entered.)

  std::vector<std::vector<std::string>> tz_vec =
      csv_to_vector("../config/" + config_file_map["tz_list"]);

  // Determining the ANSI escape codes that correspond to the 
  // display colors specified by the user:
  // Note: the escape code map only contains 16 standard colors;
  // however, the user may have chosen to submit other valid colors
  // also. Therefore, if a given color entry isn't found in
  // the dictionary, the script will assume it's a separate
  // ANSI escape code and thus pass it on, unconverted, to the 
  // main while() loop.

  std::vector<std::string> color_variables = {
    "entry_name_color", "daytime_color", "nighttime_color",
    "unix_time_name_color", "unix_time_color"};

    for (auto color_variable: color_variables)
    {
      if (ansi_color_codes.contains(
        config_map[color_variable]))
        // Replacing the color name with its corresponding color 
        // code:
        {config_map[color_variable] = ansi_color_codes[
          config_map[color_variable]];}
      }

  // Determining whether or not to show Unix time:
  bool show_unix_time = true;
  if (config_map["show_unix_time"] != "true") {
    show_unix_time = false;
  }

  // Specifying, based on the show_seconds, show_year, show_date,
  // and show_offset options within the configuration file,
  // how times should be formatted:
  // This specification will entail constructing a format string,
  // piece by piece (depending on the user's preferences),
  // that can then get passed to the get_tz_time().
  // The new runtime_format() function, available within
  // C++26, makes this approach possible.

  // This code was based on:
  // https://stackoverflow.com/a/68754043/13097194 ;
  // https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html ;
  // and https://en.cppreference.com/w/cpp/utility/format/runtime_format.html
  // .

  std::string format_string = "";

  if (config_map["use_custom_format"] == "true") {
    format_string = config_map["custom_format_code"];
  } else

  {

    format_string = "{:";
    // Specifying whether to show seconds: (the HH-MM component
    // will always be shown.)
    if (config_map["show_seconds"] == "true") {
      format_string += "%T";
    } else {
      format_string += "%R";
    }

    // Specifying which date format to use:
    // This section is a bit more complex, since there are five
    // possible combinations (governed by the show_year,
    // show_date and date_before_month configuration entries)
    // to take into account:
    // YYYY-MM-DD; MM-DD; DD-MM-YYYY; DD-MM; and YYYY.
    if (config_map["show_date"] == "true") {
      if (config_map["date_before_month"] == "true")

      {
        if (config_map["show_year"] == "true") {
          format_string += " (%d-%m-%Y)";
        } else // The year won't get displayed.
        {
          {
            format_string += " (%d-%m)";
          }
        }
      }

      else // Months will come before dates.

      {
        if (config_map["show_year"] == "true") {
          format_string += " (%F)";
        }    // %F is short for YYYY-MM-DD.
        else // The year won't get displayed.
        {
          {
            format_string += " (%m-%d)";
          }
        }
      }
    } // In this case, the user has chosen not to include the date.

    else if (config_map["show_year"] == "true")
    // This would be an unusual condition,
    // but I'll accommodate it nevertheless!
    {
      format_string += " (%Y)";
    }

    if (config_map["show_offset"] == "true") {
      format_string += " (%z)";
    }

    format_string += "}";

    // // For debugging
    // std::cout << "Format string:" << format_string << "\n";
    // std::this_thread::sleep_for(std::chrono::seconds(1));
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
    // times.
    // It took around 1200 microseconds (e.g. 1.2ms), on average,
    // for the script to render around 32 times; around 800-900
    // microseconds for it to render roughly 25 times;
    // and around 300-400 microseconds for it to render 10 different
    // times.
    // These times might be substantially slower or faster on your
    // own machine.

    // auto start_time = std::chrono::high_resolution_clock::now();
    print_tzs(tz_vec, config_map, format_string);
    // auto end_time = std::chrono::high_resolution_clock::now();
    // auto run_time = std::chrono::duration_cast<
    // std::chrono::microseconds>(end_time - start_time).count();
    // std::cout << "Render time: " << run_time
    // << " microseconds\t";
    // The previous runtime calculation is based on p. 587 of
    // Programming: Principles and Practice Using C++ (3rd Edition)
    // by Bjarne Stroustrup.
  }
}
