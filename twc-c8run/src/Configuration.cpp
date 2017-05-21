//
// Created by Laurent Fourrier on 2017-05-18.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include <cstring>

#include "Configuration.h"

const char *CONFIGURATION_MESSAGES[] = {
        "Input file is missing.",
        "Unknown or invalid argument."
};

ConfigurationException::ConfigurationException(ConfigurationError error):
_error(error)
{}

const char *ConfigurationException::what() const noexcept
{
    return CONFIGURATION_MESSAGES[_error];
}

ConfigurationError ConfigurationException::get_error() const
{
    return _error;
}

Configuration::Configuration():
_file("")
{}

void Configuration::parse_arguments(int argc, char **argv)
{
    bool got_file = false;

    // Argument parse loop
    for(int arg_it = 1; arg_it < argc; arg_it++)
    {
        char *arg_cur = argv[arg_it];
        if (strncmp(arg_cur, "--", 2) == 0)
            parse_word_argument(arg_cur);
        else if (strncmp(arg_cur, "-", 1) == 0)
            parse_letter_argument(arg_cur);
        else
        {
            got_file = true;
            _file = std::string(arg_cur);
        }
    }

    // Check if we actually got the file
    if (!got_file)
        throw ConfigurationException(ConfigurationError::MISSING_FILE);
}

void Configuration::parse_letter_argument(const char *arg)
{
    throw ConfigurationException(ConfigurationError::INVALID_ARGUMENT);
}

void Configuration::parse_word_argument(const char *arg)
{
    throw ConfigurationException(ConfigurationError::INVALID_ARGUMENT);
}

const std::string &Configuration::get_file() const
{
    return _file;
}