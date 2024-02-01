# liballoc - Custom Memory Allocation Library

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://https://github.com/Melkor-1/liballoc/edit/main/LICENSE)

`liballoc` is a memory allocation library written in C that provides custom memory allocation and management functionality using a singly linked list data structure.

## Features

- Allocate and manage memory using a singly linked list.
- Support for `malloc`, `calloc`, `realloc`, and `free` functions.
- Thread-safe memory allocation using mutex locks.
- Built-in overflow and error handling.

## Usage

1. Include the `liballoc.h` header file in your source code:

```c
#include "liballoc.h"
```

Link your project with the compiled static or dynamic library.

## Building the Library

1. Clone this repository:
```bash
git clone https://github.com/your-username/liballoc.git
cd liballoc
```

2. Build the library:
```
make
```
This will compile the library and generate both static `(liballoc_$(ARCH)-$(OS).a)` and dynamic `(liballoc_$(ARCH)-$(OS).so)` versions in the `bin` directory.

## Contributing
Contributions are welcome! If you find any issues or want to add new features or improvements, feel free to open a pull request.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgements

The core concepts and code structure of this library were adapted from the tutorial by Arjun Sreedharan: [Memory Allocators 101 - Write a simple memory allocator](https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory).
