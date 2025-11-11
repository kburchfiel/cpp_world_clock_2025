# Console World Clock (2025 Edition)

Version 2.1.0 (Linux); Version 2.0.0 (Windows)

By Kenneth Burchfiel

Released under the MIT License

*Note: This program, like all of my other programs, was created \*without\* the use of generative AI tools.*

![](Images/default_output.png)

**Console World Clock 2025** (CWC25) is a simple C++ command-line-interface program that displays the current time and date for a list of time zones that you specify. You're also able to customize many aspects of the output, including what colors to use for different components and how much detail to display.

By default, times will appear in green if they're later than or equal to 8:00:00 and earlier than 20:00:00; all other times will appear in cyan. You can choose different colors and cutoff times than these if you wish, however.

The source code makes extensive use of [ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code) to control the color and display of each time zone.


## Setup instructions

1. If you'd like to download an executable for Windows or Linux, you can do so on Itch.io at https://kburchfiel.itch.io/console-world-clock. alternatively, you can compile the program yourself (see instructions below). 

    [Note: I encountered issues with compiling this program for MacOS, as the `zoned_time` function wasn't recognized by the laptop's version of Clang/G++. Also, the Windows executable is probably at least somewhat less efficient than the Linux executable, as the latter makes use of a C++26 function that doesn't yet appear to be supported by MSVC.]

2. Navigate to the build/ folder within your terminal, then launch the executable. On Linux, you can do so via the following command line entries: 

```
cd /home/your_username/Downloads/cpp_world_clock_2025/build
./cwc25
```

On Windows, you would use the following steps:

```
cd C:\\Users\your_username\Downloads\cpp_world_clock_2025\build
cwc25.exe
```

**Note**: If you try to launch the program from another folder, it likely won't work correctly, as it uses relative paths to locate certain configuration files. (More on these files below.)

(Make sure to replace the path to the program folder with your own path as needed.)

 The program will show, by default, the time, month, and date for 23 cities around the world along with the current Unix Time. However, there are many ways to customize its output, either via pre-existing configuration files or ones that you create yourself.

You can resize the terminal as needed to accommodate your own time zone list and configuration settings. **(If not all of the time zones can be displayed, the program may not operate correctly; you should be able to resolve this by increasing your terminal's height and/or using a smaller font size.)**

While it's most convenient to just have it on the side of your current screen, you could also dedicate an entire external monitor to it if you'd like!

![](Images/side_monitor_pic.jpg)


## Configuration instructions

Configuring CWC25 is very easy once you get the hang of it! When the program launches, it will load two filenames stored in /config/cwc_config.csv:

1. A .csv file containing time zones
2. A .csv file containing configuration settings

Thus, in order to update the program's output, you'll need to replace the existing filenames within this document with your preferred ones. A number of alternative time zone and config files are already present in the /config folder, but you can create your own also.

### Updating time zones

When creating a new .csv file for the time zones you wish to display, you can use one of the existing files (such as tz_list_default.csv) as a guide. You can choose any number of time zones as long as you specify at least one; however, the output may not display correctly if there's not enough space on your monitor to show all of them.

Within your CSV file, the first column should show time zone database entries ([available here](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones), and the second column should show the labels you'd like to assign to those entries. (These labels can include spaces, but they must not include commas.) World times will be displayed in the order that they're entered within this database. 

[This map](https://upload.wikimedia.org/wikipedia/commons/8/88/World_Time_Zones_Map.png) can help you identify which time zones you might want to add to your program.

For instance, an entry for Washington, DC could be entered as follows:

`America/New_York,Washington`

(If you're updating this .csv file within a spreadsheet editor, simply add `America/New York` to the first cell and `Washington` to the second one.)

Washington, like most cities, doesn't have its own time zone database entry, which is why we're using New York (another city within the same US Eastern time zone) instead.

You can replace 'Washington' with a value of your choice, such as an airport code:

`America/New_York,IAD`

Note that the first row of all configuration files, including time zone lists, will be skipped by the program, as it's expected to be a header row.

### Updating configuration settings

To update your configuration settings, I recommend making a copy
of the CSV file containing the program's default settings (config_list_default.csv), then adjusting the settings accordingly.

**Notes**: 

1. To adjust color values, you'll need to enter the ANSI escape code that corresponds to your desired foreground color. A list of these codes can be found at https://en.wikipedia.org/wiki/ANSI_escape_code#Colors . For instance, if you would like to make daytime colors yellow, set the daytime_color setting to 33 (the foreground color for yellow).

2. For boolean (yes/no) entries, enter 'true' for yes and 'false' for no--not True, FALSE, etc.

3. Make sure not to add any spaces before or after configuration variables or values--or the commas that separate them. (For example, you can enter `entry_name_color,37`, but *not* `entry_name_color, 37`.)

Here are the following configuration settings that you can specify, along with their values in config_list_default.csv:

1. `entry_name_color`: the color in which to display time zone names. Default: `37` (the ANSI escape color for white)
1. `daytime_start`: an integer corresponding to the first hour to which you would like to assign 'daytime' colors to times. Default: `8`
1. `daytime_end`: an integer one greater than the *last* hour to which you would like to assign 'daytime' colors to times. Default: `20`

    The default settings for daytime_start and daytime_end mean that all times greater than or equal to 8:00:00 (i.e. 8 AM) and *less* than 20:00:00 (i.e. 8 PM) will be assigned the daytime color that you specify; meanwhile, all other times (e.g. 20:00:00 to 7:59:59) will be assigned the nighttime color.

    If, for instance, you would like to limit daytime colors to the 9-5 range, use 9 and 17 as your daytime_start and daytime_end values, respectively.

1. `daytime_color`: the color to assign to daytime values (as specified by daytime_start and daytime_end). Default: `32` (green)
1. `nighttime_color`: the color to assign to nighttime values. Default: `36` (cyan)

    By the way, if you want all times to use the same color, simply use the same color code for both `daytime_color` and `nighttime_color`.

1. `unix_time_name_color`: the color to assign to the 'Unix Time' name. Default: `37` (white)
1. `unix_time_color`: the color to assign to the Unix Time value. Default: `37` (white)

    Note that `unix_time_color` is *not* affected by your daytime/nighttime color settings.

1. `show_unix_time`: whether or not to show the current Unix Time. Default: `true`
1. `show_seconds`: whether or not to show seconds in addition to hours and minutes. Default: `true`
1. `show_year`: whether or not to show the year, in YYYY format, for each timestamp. Default: `false`
1. `show_date`: whether or not to show the date (in MM-DD format). Default: `true`
1. `show_offset`: whether or not to show the time zone offset codes for each time zone. Default: `false`
1. `horizontal_display`: whether or not to display times horizontally rather than vertically. Default: `false`

### Examples

Here are examples of what the program will look like when different .csv files in the config/folder are passed to cwc_config.csv. By further customizing these files, though, you can create even more variations!

(These screenshots were taken on GNOME Terminal v 3.52.0; output on other terminals will likely differ at least somewhat.)

**Simplified output:**

![](Images/simple_output.png)

![](Images/simple_output_airports.png)

**More detailed output:**

![](Images/detailed_output.png)

![](Images/detailed_output_airports.png)

**All-red output:** (Useful for darkrooms, mission control centers, situation rooms, etc.)

![](Images/darkroom_output.png)

**Horizontal output:** (Useful if you'd like to place the output at the top of your monitor)

![](Images/horizontal_output.png)

**Simpler horizontal output:**

![](Images/horizontal_output_simple.png)



## Compilation instructions

### Linux

To compile the source code, simply navigate to the project's build/ folder within your terminal and run gpp_build_script.sh, which will create an executable version of cpp_world_clock.cpp using g++ with the C++26 standard (which is needed for the `runtime_format()` function to work).

(Notes: 

1. The code expects the executable to be located in a /build subfolder and **not** the project's root folder.
2. The Linux-specific CMakeLists.txt code isn't working properly; therefore, you should only use that file when compiling the program for Windows (see below).

### Windows
You can use CMake with the included CMakeLists.txt file to compile the source code for Windows. Simply create a build folder within your project's root folder (if one isn't already present); navigate to that folder within your terminal; and run:

`cmake ..`

`cmake --build .` (Don't forget the period at the end!)

As noted earlier, I was not able to compile this program on MacOS, as the compiler on the laptop I was using didn't appear to support the `zoned_time()` function.
    
