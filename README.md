# Mafi

## Description

Mafi is an algorithm designed to create timetables for teachers and students. It reads files in a specific grammar (currently `mr`) to construct data structures for user data and employs an A* approach to generate optimal timetables and then writes the output data to an output file (grammar `mw`). Future enhancements may include supporting additional file formats like JSON and implementing alternative optimization techniques.

## Requirements

- GCC compiler
  To install `gcc` on Ubuntu/Debian:
  ```bash
  sudo apt install gcc
  ```
  If you do not have the GCC compiler but you have your own, then when installing type:
  ```bash
  make CC=...
  ```

- Make build system
  To install `make` on Ubuntu/Debian:
  ```bash
  sudo apt install make
  ```

## Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/UShouldRun/mafi.git
   cd mafi
   ```

2. **Compile the Project**: 
   To compile the source files and generate the binaries, run:
   ```bash
   make
   ```
   or
   ```bash
   make build
   ```
   These commands will compile both the `mafi` and `display` binaries and place them in the `build/` directory.

3. **Install the Binaries**:
   To install the binaries into the default directory (./bin), use:
   ```bash
   make install
   ```
   To install them into a different directory, set the `PREFIX` variable:
   ```bash
   make PREFIX=/desired/path install
   ```

4. **Uninstall the Binaries**:
   To remove the installed binaries, use:
   ```bash
   make uninstall
   ```
   This will remove the binaries from the installation directory specified by `PREFIX`.

5. **Clean Up Compiled Binaries**:
   To remove the compiled binaries from the build/ directory, use:
   ```bash
   make clean
   ```

## Usage

### Mafi

After installation, you can run Mafi using:

```bash
mafi input_file.mr
```

Replace `input_file.mr` with your input file in the `.mr` format.

### Display

The `display` tool can be used to visualize `.mw` files:

```bash
display file_to_display.mw
```

Replace `file_to_display` with the file you want to visualize.

## Development

The project uses the following compiler flags:
- `-Wall`: Enable all warnings
- `-Werror`: Treat warnings as errors
- `-Wpedantic`: Issue warnings for strict ISO C and ISO C++ compliance

Include directories:
- `./algo/include`
- `./libs/logger/include`
- `./libs/tree/include`
- `./include`

Libraries:
- `algo`
- `logger`
- `tree`

To modify or extend the project, ensure you adhere to these compilation settings and link against the required libraries.

## Documentation

- Detailed instructions for executing the Mafi program are available in the `docs` directory.
- Documentation for the `display` CLI app, which assists file visualization, are also included in the `docs` directory.

## Contributing

Mafi is an open-source project.
Submit pull requests or report issues, but ensure all changes are well-documented and tested.

## Author

Created by Henrique Teixeira

- [GitHub](https://github.com/UShouldRun)
- [LinkedIn](https://www.linkedin.com/in/henrique-teixeira-52461b294/)

## License

 Copyright (c) 2024 Henrique Teixeira
 
 This file is part of Mafi, licensed under the MIT License.
 
 See the LICENSE file in the project root for full license information.