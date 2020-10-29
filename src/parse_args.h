/**
 * File parse_args.h
 *
 * Provides basic services to process the main arguments. Error codes are also
 * given in the case of invalid input from the user.
 *
 * @author   Alexandre Blondin Massé
 * @version  1.0
 * @date     June 18th, 2017
 */
#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <stdbool.h>

#define FORMAT_LENGTH 5
#define FILENAME_LENGTH 200
#define COLOR_LENGTH 15
#define NUM_ROWS_DEFAULT 5
#define NUM_COLS_DEFAULT 5
#define USAGE "\
Usage: %s [--help] [--start L,R,C] [--end L,R,C] [--with-solution]\n\
    --input-filename FILENAME [--output-format STRING]\n\
    [--output-filename FILENAME]\n\
\n\
Generates an isometric map from a JSON file.\n\
\n\
Mandatory argument:\n\
  --input-filename STRING  The name of the JSON input file.\n\
                           The file must respect the right format.\n\
                           See README for more details.\n\
Optional arguments:\n\
  --help                   Shows this help message and exit\n\
  --start L,R,C            The start point (L,R,C) in the map,\n\
                           where L is the layer, R the row and\n\
                           C the column.\n\
                           Default value is (1,0,0)\n\
  --end L,R,C              The end point (L,R,C) in the map,\n\
                           where L is the layer, R the row and\n\
                           C the column.\n\
                           Default value is (1,1,1)\n\
  --with-solution          Also displays the solution in the map.\n\
  --output-format STRING   Selects the ouput format (either \"text\",\n\
                           \"dot\" or \"png\").\n\
                           The default format is \"text\".\n\
  --output-filename STRING The name of the output file.\n\
                           Mandatory for png format.\n\
                           If not specified, displays on stdout.\n\
"

// Parsing errors
enum Error {
    TP2_OK                                = 0,
    TP2_ERROR_FORMAT_NOT_SUPPORTED        = 1,
    TP2_ERROR_COORDINATES                 = 2,
    TP2_ERROR_PNG_FORMAT_WITHOUT_FILENAME = 3,
    TP2_ERROR_BAD_OPTION                  = 4,
    TP2_ERROR_INPUT_FILENAME_MANDATORY    = 5,
    TP2_ERROR_JSON_FORMAT                 = 6,
};

// Arguments
struct Arguments {
    bool showHelp;                        // Shows help?
    bool withSolution;                    // Displays solution?
    int startLayer;                       // The start layer
    int startRow;                         // The start row
    int startColumn;                      // The start column
    int endLayer;                         // The end layer
    int endRow;                           // The end row
    int endColumn;                        // The end column
    char inputFilename[FILENAME_LENGTH];  // The input filename
    char outputFormat[FORMAT_LENGTH];     // The output format
    char outputFilename[FILENAME_LENGTH]; // The output filename
    enum Error status;                    // The status of the parsing
};

/**
 * Prints how to use the program.
 *
 * @param argv  The arguments retrieved by the main
 */
void printUsage(char **argv);

/**
 * Parses and returns the arguments.
 *
 * @param argc  The argc argument retrieved by the main
 * @param argv  The argv argument retrieved by the main
 */
struct Arguments parseArguments(int argc, char **argv);

#endif
