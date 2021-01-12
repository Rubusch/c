#include "lib_file.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__fopen) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen_rw) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen_r) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen_w) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen_a) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fclose_null) {
// TODO
} TEST__END

TEST__BEGIN(read_char) {
// TODO
} TEST__END

TEST__BEGIN(read_without_spaces) {
// TODO
} TEST__END

TEST__BEGIN(read_linewise) {
// TODO
} TEST__END

TEST__BEGIN(read_blockwise) {
// TODO
} TEST__END

TEST__BEGIN(write_char) {
// TODO
} TEST__END

TEST__BEGIN(write_formated) {
// TODO
} TEST__END

TEST__BEGIN(write_linewise) {
// TODO
} TEST__END

TEST__BEGIN(create_file) {
// TODO
} TEST__END

TEST__BEGIN(remove_file) {
// TODO
} TEST__END

TEST__BEGIN(shred_file) {
// TODO
} TEST__END

TEST__BEGIN(rename_file) {
// TODO
} TEST__END

TEST__BEGIN(copy_characterwise_unbuffered) {
// TODO
} TEST__END

TEST__BEGIN(copy_characterwise_buffered) {
// TODO
} TEST__END

TEST__BEGIN(number_of_tempfiles) {
// TODO
} TEST__END

TEST__BEGIN(number_of_characters_in_static_temp) {
// TODO
} TEST__END

TEST__BEGIN(filesize) {
// TODO
} TEST__END

TEST__BEGIN(check_eof) {
// TODO
} TEST__END

TEST__BEGIN(check_error) {
// TODO
} TEST__END

TEST__BEGIN(get_bufsize) {
// TODO
} TEST__END

TEST__BEGIN(read_without_eof) {
// TODO
} TEST__END

TEST__BEGIN(read_nth_line) {
// TODO
} TEST__END

TEST__BEGIN(read_lines) {
// TODO
} TEST__END

TEST__BEGIN(read_lines_with_pattern) {
// TODO
} TEST__END

TEST__BEGIN(get_more_space) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__lothars__fopen();
	test__lothars__fopen_rw();
	test__lothars__fopen_r();
	test__lothars__fopen_w();
	test__lothars__fopen_a();
	test__lothars__fclose_null();
	test__read_char();
	test__read_without_spaces();
	test__read_linewise();
	test__read_blockwise();
	test__write_char();
	test__write_formated();
	test__write_linewise();
	test__create_file();
	test__remove_file();
	test__shred_file();
	test__rename_file();
	test__copy_characterwise_unbuffered();
	test__copy_characterwise_buffered();
	test__number_of_tempfiles();
	test__number_of_characters_in_static_temp();
	test__filesize();
	test__check_eof();
	test__check_error();
	test__get_bufsize();
	test__read_without_eof();
	test__read_nth_line();
	test__read_lines();
	test__read_lines_with_pattern();
	test__get_more_space();

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
