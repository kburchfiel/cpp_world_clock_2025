/* C++ World Clock: 2025 Edition
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

std::string
get_tz_time(const std::string &tz_string,
            const std::chrono::time_point<std::chrono::system_clock,
                                          std::chrono::seconds> &current_time,
            std::string &format_code) {

  // This function returns the time and date at the
  // time zone specified by tz_string in the format specified
  // by format_code.

  // Note: I found the actual type of system_clock::now() from
  // https://en.cppreference.com/w/cpp/chrono/system_clock/now .

  // The following code was based on
  // https://en.cppreference.com/w/cpp/chrono/zoned_time.html .

  std::chrono::zoned_time tz_time{tz_string, current_time};

  // Determining how to show dates and time zones:
  // The following line was based on
  // https://stackoverflow.com/a/68754043/13097194
  // and https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html .
  // ISO8601 approach: (Probably overkill for this use case, though)
  // return std::format("{:%FT%T%z}", tz_time);

  // Note: It would be much more ideal to specify a format string
  // before the program's main while loop; however, I had trouble
  // getting this approach to work--so I'm going with this crude
  // and ugly, but functional, option instead.

  // The four pipe-separated true/false values specify whether
  // to show seconds, years, dates, and offsets, respectively.

  if (format_code == "s_false|y_false|d_false|o_false") {
    return std::format("{:%R}", tz_time);
  }

  else if (format_code == "s_false|y_false|d_false|o_true") {
    return std::format("{:%R (%z)}", tz_time);
  }

  else if (format_code == "s_false|y_false|d_true|o_false") {
    return std::format("{:%R (%m-%d)}", tz_time);
  }

  else if (format_code == "s_false|y_false|d_true|o_true") {
    return std::format("{:%R (%m-%d) (%z)}", tz_time);
  }

  else if (format_code == "s_false|y_true|d_false|o_false") {
    return std::format("{:%R (%Y)}", tz_time);
  }

  else if (format_code == "s_false|y_true|d_false|o_true") {
    return std::format("{:%R (%Y) (%z)}", tz_time);
  }

  else if (format_code == "s_false|y_true|d_true|o_false") {
    return std::format("{:%R (%F)}", tz_time);
  }

  else if (format_code == "s_false|y_true|d_true|o_true") {
    return std::format("{:%R (%F) (%z)}", tz_time);
  }

  else if (format_code == "s_true|y_false|d_false|o_false") {
    return std::format("{:%T}", tz_time);
  }

  else if (format_code == "s_true|y_false|d_false|o_true") {
    return std::format("{:%T (%z)}", tz_time);
  }

  else if (format_code == "s_true|y_false|d_true|o_false") {
    return std::format("{:%T (%m-%d)}", tz_time);
  }

  else if (format_code == "s_true|y_false|d_true|o_true") {
    return std::format("{:%T (%m-%d) (%z)}", tz_time);
  }

  if (format_code == "s_true|y_true|d_false|o_false") {
    return std::format("{:%T (%Y)}", tz_time);
  }

  else if (format_code == "s_true|y_true|d_false|o_true") {
    return std::format("{:%T (%Y) (%z)}", tz_time);
  }

  else if (format_code == "s_true|y_true|d_true|o_false") {
    return std::format("{:%T (%F)}", tz_time);
  }

  else // This will be both the 's_true|y_true|d_true|o_true'
  // condition and a catch-all for any unexpected/
  // incorrectly-entered format codes within the config file.
  {
    return std::format("{:%T (%F) (%z)}", tz_time);
  }

  // Note: the following code reflects my *attempt* to more
  // efficiently specify these format strings. (The code was
  // originally placed before the main loop within main() so that
  // it would only need to be called once within the entire program.)
  // Hopefully I can find a way to get this to work!

  // std::string format_string = "{:";
  // if (config_map["show_seconds"] == "true")
  //   {format_string += "%T";}
  // else
  // {format_string += "%R";}

  // if ((config_map["show_year"] == "true") && (
  //   config_map["show_date"] == "true"))
  // {format_string += "(%F)";}
  // else if (config_map["show_year"] == "true") 
  // // This would be an unusual
  // condition,
  // // but I'll accommodate it nevertheless!
  //   {format_string += "(%Y)";}
  // else if (config_map["show_date"] == "true")
  //   {format_string += "(%m-%d)";}

  // if (config_map["show_offset"] == "true")
  // {format_string += "(%z)";}

  // format_string += "}";

  // std::format_string format_code = "{:%T (%F) (%z)}";

  // return tz_time;
}

void print_tzs(const std::vector<std::vector<std::string>> &tz_vec,
               std::map<std::string, std::string> &config_map) {
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
  std::chrono::time_point<std::chrono::system_clock, 
  std::chrono::seconds>
      current_time = std::chrono::floor<std::chrono::seconds>(
          std::chrono::system_clock::now());

  // The following line was based on
  // https://en.cppreference.com/w/cpp/chrono/time_point.html and
  // https://en.cppreference.com/w/cpp/chrono/time_point/time_since_epoch .
  auto unix_time_s = std::chrono::duration_cast<
  std::chrono::seconds>(
                         current_time.time_since_epoch())
                         .count();
  // Printing out all time zone data:
  // Creating a single string that can store all time zones:
  // This approach *should* reduce the likelihood that the
  // terminal's cursor will 'flicker' from quickly printing
  // out time zones line by line.
  std::string tz_display = "";

  if (config_map["show_unix_time"] == "true") {
    tz_display += "\033[" + config_map["unix_time_name_color"] + "m" +
                  "Unix Time: " + "\033[" + config_map[
                    "unix_time_color"] +
                  "m" + std::to_string(unix_time_s) 
                  + "\033[0m\033[K\n";
  }
  // \033[K clears out any lingering additional text to the right
  // of each line.

  for (const std::vector<std::string> &tz_vec_entry : tz_vec) {

    // Specifying which format code to pass to get_tz_time():

    std::string format_code =
        ("s_" + config_map["show_seconds"] + "|" + "y_" +
         config_map["show_year"] + "|" + "d_" + config_map[
          "show_date"] + "|" +
         "o_" + config_map["show_offset"]);

    std::string tz_time_str =
        get_tz_time(tz_vec_entry[0], 
          current_time, format_code);
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
    std::string time_color = "\033[" + config_map[
      "daytime_color"] + "m";
    if ((tz_time_hours < 8) || (tz_time_hours >= 20)) {
      time_color = "\033[" + config_map["nighttime_color"] + "m";
    }

    std::string entry_name_color =
        "\033[" + config_map["entry_name_color"] + "m";

    tz_display += entry_name_color + tz_vec_entry[1] + ": " + 
    time_color + tz_time_str + "\033[0m\033[K\n";
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

  // Reading in configuration filenames:
  // (This approach makes it easier to swap out different
  // time zone and config files--and to try out different
  // config options for the same set of time zones, or vice versa.)

  // After reading in the time zone and config filenames,
  // the script will then parse the data contained within those two
  // files.

  // Note: much of this input code was based on the examples
  // found at https://en.cppreference.com/w/cpp/string/basic_string/getline .

  std::ifstream config_files{"../config/cwc_config.csv"};

  // The following map will store two filenames, one for our
  // time zone list, and another for our configuration files.
  std::map<std::string, std::string> config_file_map;

  // Retrieving each line within the .csv file:

  int line_count = 0;
  for (std::string config_list_pair;
       std::getline(config_files, config_list_pair);) {

    // Discarding the header:

    if (line_count != 0) {
      std::istringstream config_list_pair_inputs{config_list_pair};

      int field_index = 0; // This value will help us differentiate
      // between configuration variables (whose field_index value
      // will be 0) and values (whose field_index value will be 1).
      // Defining variables that will store the configuration
      // info within the current line:
      std::string map_key = "";
      std::string map_value = "";
      for (std::string item;
           std::getline(config_list_pair_inputs, 
            item, ',');) {
        if (field_index == 0) {
          map_key = item;
        }
        if (field_index == 1) {
          map_value = item;
        }
        field_index++;
      }
      config_file_map[map_key] = map_value;
    }
    line_count++;
  }

  std::ifstream config_list_file{"../config/" 
    + config_file_map["config_list"]};

  // Parsing the active configuration file:

  // (Note: this script could be refactored to avoid duplicating the
  // CSV parsing code, but it's quite late at night right now
  // so I might try that another time. :)

  std::map<std::string, std::string> config_map;

  // Retrieving each line within the .csv file:

  line_count = 0;
  for (std::string config_list_pair;
       std::getline(config_list_file, config_list_pair);) {

    // Discarding the header:

    if (line_count != 0) {
      std::istringstream config_list_pair_inputs{config_list_pair};

      int field_index = 0; // This value will help us differentiate
      // between configuration variables (whose field_index value
      // will be 0) and values (whose field_index value will be 1).
      // Defining variables that will store the configuration
      // info within the current line:
      std::string map_key = "";
      std::string map_value = "";
      for (std::string item;
           std::getline(config_list_pair_inputs, 
            item, ',');) {
        if (field_index == 0) {
          map_key = item;
        }
        if (field_index == 1) {
          map_value = item;
        }
        field_index++;
      }
      config_map[map_key] = map_value;
    }
    line_count++;
  }

  // Parsing the active time zone file:

  std::vector<std::vector<std::string>> tz_vec;

  // Reading time zone database codes and user-specified titles
  // for each time zone from tz_list:
  std::ifstream tz_list_file{"../config/" + config_file_map[
    "tz_list"]};

  line_count = 0;
  for (std::string tz_list_pair; std::getline(
    tz_list_file, tz_list_pair);) {

    if (line_count != 0) {
      std::istringstream tz_list_pair_inputs{tz_list_pair};

      std::vector<std::string> tz_pair_vector{};
      for (std::string item; std::getline(
        tz_list_pair_inputs, item, ',');) {
        tz_pair_vector.push_back(item);
      }
      tz_vec.push_back(tz_pair_vector);
    }

    line_count++;
  }

  // Determining whether or not to show Unix time:
  bool show_unix_time = true;
  if (config_map["show_unix_time"] != "true") {
    show_unix_time = false;
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
    // Before adding the unwieldy 16-level if/else statement to
    // specify which time format to use,
    // and before adding other custom configuration settings,
    // I tended to get render times
    // around 500-600 microseconds (e.g. 0.5-0.6 milliseconds).
    // After adding in these extra features, my render times
    // increased to around 800-900 microseconds, and sometimes
    // went above 1000 microseconds (e.g. 1 millisecond).
    // These times might be substantially slower or faster on your
    // own machine.
    // auto start_time = std::chrono::high_resolution_clock::now();
    print_tzs(tz_vec, config_map);
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
