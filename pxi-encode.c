/*
 * (c) Copyright 2019, Pongsakorn Ritrugsa <poundxi@protonmail.com>
 * This project is licensed under the terms of the MIT license.
 */

#include <stdio.h> // fprintf()
#include <string.h> // strlen(), strcmp()
#include <unistd.h> // optarg, optopt, optind
#include <ctype.h> // isprint()
#include <stdlib.h> // exit()

typedef enum
{
	percent_encode,
	double_percent_encode,
	c_backslash
} encode_format_t;

char *_prog_name = NULL;

// https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
// https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt
int _option_force_encode_all_chars = 0; // 1 force encode all characters, 0 for partly.
char *_option_encode_format = NULL; // encoding format type
char *_input_str = NULL; // string to encode
int _is_input_str_from_pipe = 0;

int parse_options(int argc, char **argv)
{
	int index;
	int c;

	// getopt() for parse command options
	// <char> = just a flag (no param).
	// <char>: = param is required.
	// <char>:: = param is optional.
	while ((c = getopt(argc, argv, "fe:")) != -1) {
		switch (c) {
	   		case 'f':
				_option_force_encode_all_chars = 1;
				break;
	   		case 'e':
		 		_option_encode_format = optarg;
		 		break;
	   		case '?':
		 		if (optopt == 'e')
		   			fprintf(stderr, "Option -%c requires an argument.\n", optopt);
		 		else if (isprint(optopt))
					// other printable character
		   			fprintf(stderr, "Unknown option `-%c'.\n", optopt);
		 		else
					// non-printable character
		   			fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
		 		return 0;
	   		default:
		 		return 0;
	   	}
	}

	// if no stdin read from pipe
	if (_is_input_str_from_pipe == 0) {
		for (index = optind; index < argc; index++) {
			// Non-option argument
			_input_str = argv[index];
		}
	}

	return 1;
}

int is_valid_uri_char(char c)
{
	char *uri_valid_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
	size_t valid_chars_len = strlen(uri_valid_chars);

	for (size_t i = 0 ; i < valid_chars_len ; i++) {
		if (uri_valid_chars[i] == c)
			return 1;
	}

	return 0;
}

void encode_string(char *str, size_t str_len, encode_format_t encode_format)
{
	if (str != NULL) {
		for(int i = 0 ; i < str_len ; i++) {
			switch (encode_format) {
				case c_backslash: {
					printf("\\x%02X", str[i]);
					break;
				}
				case percent_encode: {
					if (!is_valid_uri_char(str[i]) || _option_force_encode_all_chars != 0)
						printf("%%%02X", str[i]);
					else
						printf("%c", str[i]);
					break;
				}
				case double_percent_encode: {
					if (!is_valid_uri_char(str[i]) || _option_force_encode_all_chars != 0)
						printf("%%25%02X", str[i]);
					else
						printf("%c", str[i]);
					break;
				}
			}
		}
		printf("\n");
	}
}

void print_usage()
{
	printf("PXI-ENCODE - This program is a simple string encoder\n");
	printf("(c) Copyright 2019, Pongsakorn Ritrugsa <poundxi@protonmail.com>\n");
	printf("This project is licensed under the terms of the MIT license.\n");
	printf("\n");

	printf("Usage:\n");
	printf("    $ %s input_string -e encode_format [-f]\n", _prog_name);
	printf(" Or\n");
	printf("    $ printf input_string | %s -e encode_format [-f]\n", _prog_name);
	printf("\n");

	printf("Options:\n");
	printf(" -e (required) = encode format.\n");
	printf("    - c_backslash => \\x48\\x65\\x6C\\x6C\\x6F\\x20\\x57\\x6F\\x72\\x6C\\x64\\x21\n");
	printf("    - percent_encode => Hello%%20World%%21\n");
	printf("    - double_percent_encode => Hello%%2520World%%2521\n");
	printf("\n");

	printf(" -f (optional) = force encode all characters.\n");
	printf("\n");

	printf("Examples:\n");
	printf("   $ %s \"Hello World\" -e c_backslash\n", _prog_name);
	printf("   $ %s \"Hello World\" -e percent_encode\n", _prog_name);
	printf("   $ %s \"Hello World\" -e percent_encode -f\n", _prog_name);
}

int main(int argc, char **argv)
{
	// check stdin pipe
	if (!isatty(fileno(stdin))) {
		int i = 0;
		char buf[10240];

		// read all data from stdin
		while ((buf[i++] = getchar()) != EOF);

		// end string with null byte
		buf[i-1] = '\0';

		// copy char array to char pointer
		_input_str = malloc(strlen(buf) + 1);
		strcpy(_input_str, buf);

		_is_input_str_from_pipe = 1;
	}

	_prog_name = argv[0];

	// parse options
	if (parse_options(argc, argv) == 0) {
		exit(1);
	}

	// input string
	if (_input_str == NULL) {
		print_usage();
		exit(2);
	}
	size_t input_str_len = strlen(_input_str);

	// encode format
	if (_option_encode_format == NULL) {
		print_usage();
		exit(3);
	}

	// https://www.gnu.org/software/libc/manual/html_node/String_002fArray-Comparison.html
	// strcmp return 0 if two strings are equal.
	if (strcmp(_option_encode_format, "percent_encode") == 0) {
		encode_string(_input_str, input_str_len, percent_encode);
	} else if (strcmp(_option_encode_format, "double_percent_encode") == 0) {
		encode_string(_input_str, input_str_len, double_percent_encode);
	} else if (strcmp(_option_encode_format, "c_backslash") == 0) {
		encode_string(_input_str, input_str_len, c_backslash);
	} else {
		print_usage();
	}

	return 0;
}
