/**
 * File parse_args.c
 *
 * Implements parse_args.h, using the getopt library to simplify the
 * processing.
 *
 * @author Alexandre Blondin Masse
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "parse_args.h"
#include <jansson.h>

// -------------- //
// Private method //
// -------------- //

/**
 * Retrieves the (layer, row, column) coordinates from a string.
 *
 * @param s       The string containing the coordinates
 * @param layer   The given layer
 * @param row     The given row
 * @param column  The given column
 */
enum Error castCoordinates(char *s, int *layer, int *row, int *column) {
    char tail = '\0';
    int numParsed = sscanf(s, "%d,%d,%d%c", layer, row, column, &tail);
    return numParsed == 3 && tail == '\0' ? TP2_OK : TP2_ERROR_COORDINATES;
}


int adequateJsonFormat(char filename[]) {
    int noTile = 0;
    unsigned int numrows;
    unsigned int numcols;
    json_t* map = json_load_file(filename, 0, NULL);

    if (map == NULL) {
        return -1;
    } else {
        json_t* numrowsMap = json_object_get(map, "numrows");
        if (numrowsMap == NULL) {
            return -1;
        } else {
            numrows = (int) json_integer_value(numrowsMap); 
            if (numrows < 1) {
                return -1;
            }
        }
        
        json_t* numcolsMap = json_object_get(map, "numcols");
        if ( numcolsMap == NULL) {
            return -1;
        } else {
            numcols = (int) json_integer_value(numcolsMap);
            if (numcols < 1) {
                return -1;
            }
        }

        json_t* numtilewidthMap = json_object_get(map, "tilewidth");
        if ( numtilewidthMap == NULL) {
            return -1;
        } else {
            int numtilewidth = (int) json_integer_value(numtilewidthMap);
            if (numtilewidth < 1) {
                return -1;
            }
        }

        json_t* numlayeroffset = json_object_get(map, "layeryoffset");
        if (numlayeroffset == NULL) {
            return -1;
        }

        json_t* tiles = json_object_get(map, "tiles");
        if(tiles == NULL){
            return -1;
        } else {
            bool exist = true;
            for (noTile = 0; exist; noTile++) {
                json_t* tileType = json_array_get(tiles, noTile);
                json_t* numTileMap = json_object_get(tileType, "id");
                if (numTileMap == NULL) {
                    exist = false;
                } else {
                    if (json_object_get(tileType, "filename") == NULL || json_object_get(tileType, "directions") == NULL) {
                        return -1;
                    }
                }
            }
        }
        json_t* layers = json_object_get(map, "layers");
        bool dataExist = true;
        for (unsigned int i = 0; dataExist; i++) {
            json_t* numLayer = json_array_get(layers, i);
            json_t* data = json_object_get(numLayer, "data");
            if (data != NULL) {
                for (unsigned int j = 0; j < (numcols * numrows); j++) {
                    int tile = (int) json_integer_value(json_array_get(data, j));
                    if ( tile > noTile ) {
                        return -1;
                    }
                }
            } else {
                dataExist = false;
            }
        }
    }
    
    return 0;
}

// Public methods //
// -------------- //

void printUsage(char **argv) {
    printf(USAGE, argv[0]);
}

struct Arguments parseArguments(int argc, char **argv) {
    struct Arguments arguments;

    // Default argument
    strcpy(arguments.inputFilename, "");
    strcpy(arguments.outputFormat, "text");
    strcpy(arguments.outputFilename, "stdout");
    arguments.startLayer  = 1;
    arguments.startRow    = 0;
    arguments.startColumn = 0;
    arguments.endLayer    = 1;
    arguments.endRow      = 1;
    arguments.endColumn   = 1;
    arguments.withSolution = false;
    arguments.showHelp = false;
    arguments.status = TP2_OK;

    struct option longOpts[] = {
        // Set flag
        {"help",            no_argument,       0, 'h'},
        {"with-solution",   no_argument,       0, 's'},
        // Don't set flag
        {"start",           required_argument, 0, 't'},
        {"end",             required_argument, 0, 'e'},
        {"input-filename",  required_argument, 0, 'i'},
        {"output-format",   required_argument, 0, 'f'},
        {"output-filename", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    // Parse options
    while (true) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "htesifo", longOpts, &option_index);
        if (c == -1) break;
        switch (c) {
            case 'h': arguments.showHelp = true;
                      break;
            case 's': arguments.withSolution = true;
                      break;
            case 't': arguments.status = castCoordinates(optarg,
                                                         &arguments.startLayer,
                                                         &arguments.startRow,
                                                         &arguments.startColumn);
                      break;
            case 'e': arguments.status = castCoordinates(optarg,
                                                         &arguments.endLayer,
                                                         &arguments.endRow,
                                                         &arguments.endColumn);
                      break;
            case 'i': strncpy(arguments.inputFilename, optarg, FILENAME_LENGTH);
                      break;
            case 'f': strncpy(arguments.outputFormat, optarg, FORMAT_LENGTH);
                      break;
            case 'o': strncpy(arguments.outputFilename, optarg, FILENAME_LENGTH);
                      break;
            case '?': arguments.status = TP2_ERROR_BAD_OPTION;
                      break;
        }
    }

    if (optind < argc) {
        printf("Error: option not recognized\n");
        arguments.status = TP2_ERROR_BAD_OPTION;
    } else if (arguments.showHelp) {
        printUsage(argv);
        arguments.status = TP2_OK;
    } else if (arguments.status == TP2_ERROR_COORDINATES) {
        printf("Error: the coordinates must be integers separated by commas\n");
    } else if (strcmp(arguments.outputFormat, "text") != 0
            && strcmp(arguments.outputFormat, "dot") != 0
            && strcmp(arguments.outputFormat, "png") != 0) {
        printf("Error: format %s not supported\n", arguments.outputFormat);
        arguments.status = TP2_ERROR_FORMAT_NOT_SUPPORTED;
    } else if (strcmp(arguments.outputFormat, "png") == 0
            && strcmp(arguments.outputFilename, "stdout") == 0) {
        printf("Error: output filename is mandatory with png format\n");
        arguments.status = TP2_ERROR_PNG_FORMAT_WITHOUT_FILENAME;
    } else if (strcmp(arguments.inputFilename, "") == 0) {
        printf("Error: input filename is mandatory\n");
        arguments.status = TP2_ERROR_INPUT_FILENAME_MANDATORY;
    } else if (adequateJsonFormat(arguments.inputFilename) != 0) {
        printf("Error: Invalid JSON file\n");
        arguments.status = TP2_ERROR_JSON_FORMAT;
    }
    return arguments;
}
