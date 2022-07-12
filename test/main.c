#include "test.h"
#include "../filesystem.h"

#include <stdlib.h>
#include <locale.h>
#include <stdint.h>

static const char* tests_path = TESTS_PATH;

static char*
path_concat(const char* part1, const char* part2, const char* part3);

int main(int argc, const char **argv)
{
    Unit_Test unit_test = test_init(100, argc, argv);

    test_register(&unit_test, test_write);
    test_register(&unit_test, test_writelines);
    test_register(&unit_test, test_read);
    test_register(&unit_test, test_readline);
    test_register(&unit_test, test_readlines);
    test_register(&unit_test, test_seek);
    test_register(&unit_test, test_utf8);

    test_run(&unit_test);
}

void test_write(Unit_Test* test)
{
    const char* out_file_relative_path = "/input/file-out.txt";
    char* out_file_path = path_concat(tests_path, out_file_relative_path, "");

    File file = fs_open(out_file_path, "w", "C.UTF-8");
    char* buf = "Dega\nديجة\n";
    size_t buf_len = strlen(buf);
    size_t read_size = fs_write(&file, buf, buf_len);

    fs_close(&file);
    free(out_file_path);
}

void test_writelines(Unit_Test* test)
{
    const char* out_file_relative_path = "/input/file_lines-out.txt";
    char* out_file_path = path_concat(tests_path, out_file_relative_path, "");

    File file = fs_open(out_file_path, "w", "C.UTF-8");
    size_t buf_len = 3;
    char* buf[] = {
        "Dega",
        "ديجة",
        "وردُةٌ"
    };
    size_t read_size = fs_writelines(&file, buf, buf_len, "\r\n");

    fs_close(&file);
    free(out_file_path);
}

void test_read(Unit_Test* test)
{
    const char* in_file_relative_path = "/input/file1.txt";
    char* in_file_path = path_concat(tests_path, in_file_relative_path, "");
    puts(in_file_path);

    File file = fs_open(in_file_path, "r", "C.UTF-8");
    const int buf_size = 100;
    char buf[buf_size];
    size_t read_size = fs_read(&file, buf, buf_size);
    puts(buf);

    fs_close(&file);
    free(in_file_path);
}

void test_readline(Unit_Test* test)
{
    const char* in_file_relative_path = "/input/file1.txt";
    char* in_file_path = path_concat(tests_path, in_file_relative_path, "");
    puts(in_file_path);

    File file = fs_open(in_file_path, "r", "C.utf-8");
    const int buf_size = 100;
    char buf[buf_size];
    size_t read_size = fs_readline(&file, buf, buf_size, "\n");
    read_size = fs_readline(&file, buf, buf_size, "\n");
    puts(buf);

    fs_close(&file);
    free(in_file_path);
}

void test_readlines(Unit_Test* test)
{
    const char* in_file_relative_path = "/input/file1.txt";
    char* in_file_path = path_concat(tests_path, in_file_relative_path, "");
    puts(in_file_path);

    File file = fs_open(in_file_path, "r", "C.utf-8");
    const int32_t buf_size = 100;
    const int32_t lines_num = 4;
    char* buf_data = malloc(lines_num * buf_size);
    char* buf[lines_num];

    for(int iii = 0; iii < lines_num; ++iii)
    {
        buf[iii] = &buf_data[iii * buf_size];
    }

    size_t read_size = fs_readlines(&file, buf, buf_size, lines_num, "\n");
    puts(buf[0]);
    puts(buf[1]);

    fs_close(&file);
    free(in_file_path);
    free(buf_data);
}

void test_seek(Unit_Test* test)
{

}

void test_utf8(Unit_Test* test)
{

}

/************************************************************************/

char*
path_concat(const char* part1, const char* part2, const char* part3)
{
    size_t parts_len = strlen(part1) + strlen(part2) + strlen(part3) + 1;
    char* parts_str = malloc(parts_len);
    // strcpy(in_file_abs_path, tests_path);
    // strcat(in_file_abs_path, in_file_relative_path);
    // in_file_abs_path[in_file_abs_path_len - 1] = '\0';

    snprintf(parts_str, parts_len, "%s%s%s", part1, part2, part3);

    return parts_str;
}
