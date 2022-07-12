#ifndef _WIN32
#include "../filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define CHECK_ASSERT(file) \
    if(!file || !file->fd) \
    { \
        fprintf(stderr, "Error in '%s:%d'\nfile or file fd is NULL\n", __FILE__, __LINE__); \
        abort(); \
    }

#define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })

static char*
__set_locale__(const char* encoding);

File
fs_open(const char* path, const char* mode, const char* encoding)
{
    File file;

    if(!path || !mode || !encoding)
    {
        return file;
    }

    FILE* fd = fopen(path, mode);

    size_t model_len = min(strlen(mode), FILE_MAX_MODE_LEN);
    size_t encoding_len = min(strlen(encoding), FILE_MAX_ENCODING_LEN);

    file.fd = fd;
    memcpy(file.mode, mode, model_len + 1);
    memcpy(file.encoding, encoding, encoding_len + 1);

    return file;
}

int
fs_close(File* file)
{
    CHECK_ASSERT(file);

    return fclose(file->fd);
}

size_t
fs_read(File* file, char* buf, size_t buf_size)
{
    CHECK_ASSERT(file);

    char* cur_locale = __set_locale__(file->encoding);

    size_t read_size = 0;

    if(buf)
    {
        read_size = fread(buf, buf_size, 1, file->fd);
    }

    cur_locale = __set_locale__(cur_locale);

    return read_size;
}

size_t
fs_readline(File* file, char* buf, size_t buf_size, const char* newline)
{
    CHECK_ASSERT(file);

    long read_size = 0;

    if(buf)
    {
        char* cur_locale = __set_locale__(file->encoding);

        if(strchr(file->mode, 'b'))
        {
            buf[0] = '\0';
            return 0;
        }

        read_size = getdelim(&buf, &buf_size, newline[0], file->fd);

        cur_locale = __set_locale__(cur_locale);
    }

    return read_size > 0 ? read_size : 0;
}

size_t
fs_readlines(File* file, char** buf, size_t max_line_size, size_t lines_num, const char* newline)
{
    CHECK_ASSERT(file);

    int cur_lines_num = 0;

    if(buf)
    {
        char* cur_locale = __set_locale__(file->encoding);

        for(; cur_lines_num < lines_num - 1; ++cur_lines_num)
        {
            if(getdelim(&buf[cur_lines_num], &max_line_size, newline[0], file->fd) == -1)
            {
                break;
            }
        }

        buf[cur_lines_num] = '\0';

        cur_locale = __set_locale__(file->encoding);
    }

    return cur_lines_num;
}

bool
fs_readable(File* file)
{
    CHECK_ASSERT(file);

    if(strchr(file->mode, 'r') || strchr(file->mode, '+'))
    {
        return true;
    }

    return false;
}

size_t
fs_write(File* file, char* buf, size_t buf_size)
{
    CHECK_ASSERT(file);

    size_t write_size = 0;

    if(buf)
    {
        char* cur_locale = __set_locale__(file->encoding);
        write_size = fwrite(buf, buf_size, 1, file->fd);
        cur_locale = __set_locale__(cur_locale);
    }

    return write_size;
}

size_t
fs_writelines(File* file, char** buf, size_t lines_num, const char* newline)
{
    CHECK_ASSERT(file);

    int cur_lines_num = 0;

    if(buf)
    {
        char* cur_locale = __set_locale__(file->encoding);
        size_t written_line_size = 0;

        for(; cur_lines_num < lines_num; ++cur_lines_num)
        {
            written_line_size = fwrite(buf[cur_lines_num], strlen(buf[cur_lines_num]), 1, file->fd);
            written_line_size = fwrite(newline, strlen(newline), 1, file->fd);
        }

        cur_locale = __set_locale__(file->encoding);
    }

    return cur_lines_num;
}

bool
fs_writable(File* file)
{
    CHECK_ASSERT(file);

    if(strchr(file->mode, 'w') || strchr(file->mode, '+'))
    {
        return true;
    }

    return false;
}

bool
fs_flush(File* file)
{
    CHECK_ASSERT(file);

    if(fflush(file->fd) == 0)
    {
        return true;
    }

    return false;
}

bool
fs_seek(File* file, size_t offset, FS_Seek whence)
{
    CHECK_ASSERT(file);

    if(fseek(file->fd, offset, whence) == 0)
    {
        return true;
    }

    return false;
}

bool
fs_seekable(File* file)
{
    CHECK_ASSERT(file);

    return fs_seek(file, 0, FS_SEEK_CUR);
}

long
fs_tell(File* file)
{
    CHECK_ASSERT(file);

    return ftell(file->fd);
}

/*************************************************************/
static char*
__set_locale__(const char* encoding)
{
    char* cur_locale = setlocale(LC_ALL, "");
    char* locale_state = setlocale(LC_ALL, encoding);

    if(!locale_state)
    {
        fprintf(stderr, "Error: in '%s:%d'\nCouldn't set locale to %s\n", __FILE__, __LINE__, encoding);
        abort();
    }

    return cur_locale;
}

#endif // _WIN32