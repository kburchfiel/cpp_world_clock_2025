# Console World Clock (2025 Edition)

By Kenneth Burchfiel

Released under the MIT License

![](cwc_2025_screenshot.png)

*Note: This program, like all of my other programs, was created *without* the use of generative AI tools.*

This simple C++ command-line-interface program displays the current time and date for a list of user-specified time zones. Times will appear in green if they're later than or equal to 8:00:00 and earlier than 20:00:00; all other times will appear in cyan.

The source code makes extensive use of [ANSI escape codes to control](https://en.wikipedia.org/wiki/ANSI_escape_code) the color and display of each time zone.


## Setup instructions

1. Go to [the Releases page](https://github.com/kburchfiel/cpp_world_clock_2025/releases) and download, then unzip, the latest release for your operating system (if available). [Note: I encountered issues with compiling this program for MacOS, as the `zoned_time` function wasn't recognized by the laptop's version of Clang/G++.]

2. Update the tz_list.csv file as needed in order to specify which time zones you would like to display. (You can choose any number of time zones as long as you specify at least one; however, the output may not display correctly if there's not enough space on your monitor to show all of them.)

    Within this CSV file, the first column shows time zone database entries ([available here](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones), and the second column shows the labels you'd like to assign to those entries. (These labels can include spaces, but they must not include commas.) World times will be displayed in the order that they're entered within this database. 

    [This map](https://upload.wikimedia.org/wikipedia/commons/8/88/World_Time_Zones_Map.png) can help you identify which time zones you might want to add to your program.

3. Run the program by (1) opening your terminal; (2) navigating to the /build subfolder folder within the unzipped project folder; and then executing the program (e.g. by entering './cwc25' on Linux).

## Compilation instructions

To compile the program, you can use CMake with the included CMakeLists.txt file. Simply create a build folder within your project's root folder; navigate to that folder within your terminal; and run:

`cmake ..`

`cmake --build .` (Don't forget the period at the end!)

Alternatively, you can create an executable using a compiler of your choice. For instance, to compile the program with g++, navigate to the project's root folder, make a build directory, and then run: 

`g++ cpp_world_clock.cpp -std=c++23 -o build/cwc25`

The Clang command is almost identical:

`clang++ cpp_world_clock.cpp -std=c++23 -o build/cwc25`

(Note: the code expects the executable to be located in a /build subfolder and **not** the project's root folder.)

As noted earlier, I was able to compile it on Linux and Windows, but not MacOS. I might try using CMake to cross-compile it for MacOS within Linux at some point.

    
