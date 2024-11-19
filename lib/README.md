# ArgParser <!-- omit in toc -->
The ArgParser supports the POSIX program argument syntax [convention](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html).

Using the parser is that simple:
```cpp
#include <iostream>

// Simple program that only prints the number
// The usage may be: ./app --number=10

int main(int argc, char** argv) {
    ArgumentParser::ArgParser parser("Program name", "Program description");
    parser.AddArgument<int32_t>('n', "number", "Some number");

    if (!parser.Parse(argc, argv)) {
        std::cerr << "An error occured" << std::endl;
        return 1;
    }

    std::cout << parser.GetValue<int32_t>("number") << std::endl;
    return 0;
}
```

- [Argument configuration](#argument-configuration)
  - [Default value](#default-value)
  - [Multi value](#multi-value)
  - [Storage for values](#storage-for-values)
- [Options and positional arguments](#options-and-positional-arguments)
  - [Options](#options)
  - [Positional arguments](#positional-arguments)
- [Obtaining a value](#obtaining-a-value)
- [Error handling](#error-handling)
  - [What is a successful parse?](#what-is-a-successful-parse)
  - [Determining an error](#determining-an-error)
- [Help](#help)
  - [Type aliases](#type-aliases)
  - [Does user need help?](#does-user-need-help)
- [Registering your own types](#registering-your-own-types)


## Argument configuration
For each argument you create, you can specify the rules of it's validation, the "storage" for a value, etc.

### Default value
You can specify a default value for an argument. If the argument value was not specified by the user during parsing, it will be set to the default value.

```cpp
ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number")
      .Default(10);
```

### Multi value
If you want your argument to accept many values, mark the argument as multi value.
```cpp
ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number")
      .MultiValue();
```

To specify the minimum number of values, use:
```cpp
ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number")
      .MultiValue(5);
```

Possible usage: `app --number=10 --number=15 --number=-3`.

__NB__ If the argument has a default value, multivalue validation will not occur. If you want to get a value at an index that exceeds the number of values passed, you will get the default value. To find out the exact number of values passed, use `GetValuesSet(name)`.

### Storage for values
By default, each argument has it's own storage for passed values, and it stores all the values that a user passed (even if it's not a multi value argument), and - separatly - the last value passed.

To specify your own storage, use std::vector<arg_type> for all values:
```cpp
std::vector<int32_t> values;

ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number")
      .MultiValue()
      .StoreValues(values);
```

And any variable of the same type for the last value:
```cpp
int32_t value;

ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number")
      .MultiValue()
      .StoreValue(values);
```
__NB__ The storage gets __cleared__ every time a parsing performs.

## Options and positional arguments
There are 2 types of arguments: options and positional arguments. The type of an argument determines __the way it will be parsed__ and the way it will be printed in the [HelpDescription()](#does-user-need-help).

### Options
Arguments are options if they begin with a hyphen delimiter (`-`). By default, all arguments that are registered as options.
Let's say you have an argument like this:
```cpp
ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number");
```
To specify it's value, the *argv* must contain `--number=10` or `-n 10` (other combinations are possible, see the [convention](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html)).

### Positional arguments
Positional arguments don't start with a hyphen. All values that start with it must be passed after the special argument which indicates the end of options: `--`.

To register a positional argument, use:
```cpp
ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number")
      .Positional();
```

If an argument in the *argv* doesn't start with a hyphen and the `--` argument wasn't present, the argument is regarded as an option.
Positional argument are parsed in the same order they were registered.

__NB__ If a positional argument is also a multi value, __all__ values after it will be considered the values of this argument. Meaning that only 1 positional + multi value argument can be present (all positional arguments after it will be ignored). This befavior is similar to a function with a variable amount of parameters.

## Obtaining a value
Once the parsing is performed, you can get a value of the argument:
```cpp
ArgumentParser::ArgParser parser("Program name", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number");
parser.AddArgument<std::string>('f', "film", "Your favourite film")
      .MultiValue(2);

// argv: "app --number=10 --film=TheGodfather --film=Casablanca"
parser.Parse(argc, argv);

std::optional<int32_t> value_result = parser.GetValue<int32_t>("number");
int32_t value = value_result.value(); // value == 10

std::string film1 = *parser.GetValue<std::string>("film", 0); // TheGodfather
std::string film2 = *parser.GetValue<std::string>("film", 1); // Casablanca
```

Note that GetValue returns a std::optional, so you should check the value every time you use it. Alternatively, you can check the return value of Parse - if it's true, all values are set, and "direct" use of GetValue is safe.

## Error handling
Of course, users of your program may make mistakes when specifying the necessary arguments. To deal with them and produce a nice message rather than crashing the whole program, ArgParser provides you with several tools.

### What is a successful parse?
`Parse()` returns a bool value indicating the success or failure of the parsing. The success means:
* No syntax errors were detected: all argument names are defined, and the value parsing for all arguments was successful.
* All required (i.e. not having a default value) arguments are set.
* For all multi-valued arguments (except those with a default value), the number of values is not less than the minimum.

If at least one of these items is violated, parsing is considered unsuccessful.

### Determining an error
To find out if the parsing was successful, you can either check the return value of `Parse()` or use the `HasError()` function.

Once you know that the parsing failed, you can get the exact name of the parameter (if it could be determined), the string from *argv*, and the error type. To do so, use the `GetError()` function.

The error type is the following enum:
```cpp
enum class ParsingErrorType {
    kInsufficent,
    kInvalidArgument,
    kUnknownArgument,
    kNoArgument,
    kSuccess // default
};
```
And the return value of `GetError()` is:
```cpp
struct ParsingError {
    std::string_view argument_string;
    ParsingErrorType status = ParsingErrorType::kSuccess;
    std::string_view argument_name;
};
```

Usage example:
```cpp
if (!parser.Parse(argc, argv)) {
    auto error = parser.GetError();

    if (error.status == ParsingErrorType::kUnknownArgument) {
        std::cerr << "Unknown argument: " << error.argument_string << std::endl;
        return 1;
    }

    std::cerr << "An error occured while parsing the following argument: " 
        << (error.argument_name) << std::endl;

    if (error.status == ParsingErrorType::kInvalidArgument) {
        std::cerr << "Unable to parse: " << error.argument_string << std::endl;
    } else if (error.status == ParsingErrorType::kNoArgument) {
        std::cerr << "No value was specified" << std::endl;
    } else if (error.status == ParsingErrorType::kInsufficent) {
        std::cerr << "Not enough values were specified" << std::endl;
    }

    return 1;
}
```

## Help 
You can register a special argument, specifying which the user can get help about using your program.
The help includes the name of the program, its description, usage pattern and description of all arguments.

For the following configuration:

```cpp
ArgumentParser::ArgParser parser("Program", "Program accumulate arguments");
parser.AddArgument<int32_t>("B").Positional();
parser.AddArgument<int32_t>("N").MultiValue(2).Positional();
parser.AddArgument<bool>('s', "sum", "add args");
parser.AddArgument<bool>('m', "mult", "multiply args");
parser.AddArgument<std::string>('s', "str", "some string").MultiValue(3);
parser.AddHelp('h', "help", "display help and exit");
```

The help will look as:
```
Program
Program accumulate arguments
Usage: Program [OPTIONS] <B> <N>...
List of options:
-s, --sum           add args [default = false]
-m, --mult          multiply args [default = false]
-s, --str=<string>  some string [repeated, min values = 3]
-h, --help          display help and exit
```

As you may have noticed, you should use `AddHelp()` to register the help argument.
Use `HelpDescription()` to get the string containg this help message.

### Type aliases
Notice the `-s, --str=<string>` above. This line is printed because there's an alias registered for `std::string` - `string`.
To register (or change an existing one) the alias, use `SetTypeAlias<type>(alias)`.

Usage:

```cpp
ArgumentParser::ArgParser parser("Program", "Program description");
parser.AddArgument<int32_t>('n', "number", "Some number");
parser.AddArgument<std::string>('f', "film", "Your favourite film")
      .MultiValue(2);

parser.SetTypeAlias<std::string>("yoursuperstring");
```

The help message will be:
```
Program name
Program description
Usage: Program [OPTIONS]
List of options:
-n, --number=<int>            Some number
-f, --film=<yoursuperstring>  Your favourite film [repeated, min values = 2]
```

### Does user need help?
To answer this question, use the `Help()` function, which returns a bool value. If the help flag was mentioned, `Help()` will return `true`.
This is only true if there were no syntax errors before the flag was mentioned.
And, of course, if you have not registered a help argument, `Help()` will always return `false`.

Usage:
```cpp
ArgumentParser::ArgParser parser("Program");
parser.AddHelp('h', "help", "display help and exit");
// ...
parser.Parse(argc, argv);

if (parser.Help()) {
    std::cout << parser.HelpDescription() << std::endl;
    return 0;
}
```

## Registering your own types
To add support for your own type, all you need to do is define a function to parse its value from a string (string_view, to be exact).
In this function, you must check the value and parse it. If the value cannot be derived from the passed string, your function should return `std::nullopt`. Otherwise, return the actual value.

```cpp
template<>
std::optional<YourType> ArgumentParser::ParseValue<YourType>(std::string_view value_string) {
    if (!IsValid(value_string)) { // your own validation function
        return std::nullopt;
    }

    YourType value;

    // parsing...

    return value;
}
```
