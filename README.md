# PXI-ENCODE
This program is a simple string encoder.

## Supported Encode styles
- c_backslash => \x48\x65\x6C\x6C\x6F\x20\x57\x6F\x72\x6C\x64\x21
- percent_encode => Hello%20World%21
- double_percent_encode => Hello%2520World%2521

## How to use
### Usage
**./pxi-encode** input_string -e encode_format [ -f ]

### Options
- -e (required) = format to encode:

    * c_backslash => \x48\x65\x6C\x6C\x6F\x20\x57\x6F\x72\x6C\x64\x21
    * percent_encode => Hello%20World%21
    * double_percent_encode => Hello%2520World%2521

- -f (optional) = force encode all characters.

### Examples
- ./pxi-encode "Hello World" -e c_backslash

- ./pxi-encode "Hello World" -e percent_encode

- ./pxi-encode "Hello World" -e percent_encode -f
