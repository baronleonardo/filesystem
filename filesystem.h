#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdbool.h>

#define FILE_MAX_MODE_LEN       4
#define FILE_MAX_ENCODING_LEN   11

typedef enum FS_Seek {
    FS_SEEK_BEGIN,
    FS_SEEK_CUR,
    FS_SEEK_END,
} FS_Seek;

typedef struct File {
    FILE* fd;
    char mode[FILE_MAX_MODE_LEN];
    char encoding[FILE_MAX_ENCODING_LEN];
} File;

/// @param mode:
///     r ---- Opens an existing text file for reading purpose.
///     w ---- Opens a text file for writing. If it does not exist, then a new file is created. Here your program will start writing content from the beginning of the file.
///     a ---- Opens a text file for writing in appending mode. If it does not exist, then a new file is created. Here your program will start appending content in the existing file content.
///     r+ --- Opens a text file for both reading and writing.
///     w+ --- Opens a text file for both reading and writing. It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
///     a+ --- Opens a text file for both reading and writing. It creates the file if it does not exist. The reading will start from the beginning but writing can only be appended.
///     binary_mode --- "rb", "wb", "ab", "rb+", "r+b", "wb+", "w+b", "ab+", "a+b"
File
fs_open(const char* path, const char* mode, const char* encoding);

int
fs_close(File* file);

size_t
fs_read(File* file, char* buf, size_t buf_size);

size_t
fs_readline(File* file, char* buf, size_t buf_size, const char* newline);

/// @param max_lines_num: if `-1` it will read to the EOF or until the buf is fulled
size_t
fs_readlines(File* file, char** buf, size_t max_line_size, size_t lines_num, const char* newline);

bool
fs_readable(File* file);

size_t
fs_write(File* file, char* buf, size_t buf_size);

/// @param max_lines_num: if `-1` it will read to the EOF or until the buf is fulled
size_t
fs_writelines(File* file, char** buf, size_t lines_num, const char* newline);

bool
fs_writable(File* file);

bool
fs_flush(File* file);

bool
fs_seek(File* file, size_t offset, FS_Seek whence);

bool
fs_seekable(File* file);

/// @descript: return current stream position
long
fs_tell(File* file);

#endif // FILESYSTEM_H