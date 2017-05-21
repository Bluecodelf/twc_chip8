//
// Created by Laurent Fourrier on 2017-05-18.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_CONFIGURATION_H
#define TWC_CHIP8_CONFIGURATION_H

#include <exception>
#include <string>

enum ConfigurationError
{
    MISSING_FILE = 0,
    INVALID_ARGUMENT = 1
};

class ConfigurationException: public std::exception
{
public:
    explicit ConfigurationException(ConfigurationError error);
    virtual ~ConfigurationException() {}

    virtual const char *what() const noexcept;
    ConfigurationError get_error() const;
private:
    ConfigurationError _error;
};

class Configuration
{
public:
    Configuration();

    void parse_arguments(int argc, char **argv);

    const std::string &get_file() const;
private:
    void parse_letter_argument(const char *arg);
    void parse_word_argument(const char *arg);
    std::string _file;
};

#endif //TWC_CHIP8_CONFIGURATION_H
