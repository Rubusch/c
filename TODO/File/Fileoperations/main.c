// main.c
#include "fo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIN_LEN 2*BUFSIZ

int main(int argc, char** argv)
{
  // init
  FILE* fp = NULL;
  char file1[] = "file1.txt";
  char file2[] = "file2.txt";
  char file3[] = "file3.txt";
  char text1[] = "Jack and Jill went up the hill to fetch a pail of water\n";
  char text2[] = "Jack fell down and broke his crown\nAnd Jill came tumbling after.\n";
  char text3[] = "Up got Jack, and home did trot\nAs fast as he could caper\n";
  char text4[] = "He went to bed and bound his head\nWith vinegar and brown paper.\n";
  unsigned long int text1_size = sizeof(text1);
  unsigned long int text2_size = sizeof(text2);
  unsigned long int text4_size = sizeof(text4);
  unsigned long int content_size = LIN_LEN;

  char *content = NULL;
  if( (content = calloc(content_size, sizeof(*content))) == NULL) return EXIT_FAILURE;
  strcpy(content,"");

  printf("~~~ TEST SUITE FOR THE FILE OPERATIONS ~~~\n\n");
  printf("file1 = \'%s\'\nfile2 = \'%s\'\nfile3 = \'%s\'\ntext = %scontent = \'%s\' (empty)\n\n", file1, file2, file3, text1, content);


  // write a file characterwise and create write file pointer - OK
  puts("1. write a file characterwise and create write file pointer");
  printf("%i - Get write file pointer\n", get_write_file_pointer(&fp, file1));
  printf("%i - Writing file characterwise\n", write_char(fp, text1, text1_size));
  printf("%i - Close stream\n", close_stream(&fp));
  printf("%i - fp != NULL\n", (fp != NULL));
  printf("Done.\n\n\n");
  //*/

  // creating a new file - OK
  puts("2. creating a new file");
  printf("%i - Creating a new file\n", create_file(file2, LIN_LEN));
  printf("Done.\n\n");
  //*/

  // copying unbuffered - OK
  puts("3. copying unbuffered");
  printf("%i - Copying %s to %s characterwise, unbuffered\n", copy_characterwise_unbuffered(file1, file2), file1, file2);
  printf("Done.\n\n");
  //*/

  // appending a line to file2 - OK
  puts("4. appending a line to file2");
  printf("%i - Get append file pointer\n", get_append_file_pointer(&fp, file2));
  printf("%i - Append some text to the file (linewise)\n", write_linewise(fp, text2, text2_size));
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // appending a line to file2 - OK
  puts("5. appending a line to file2");
  printf("%i - Get append file pointer\n", get_append_file_pointer(&fp, file2));
  printf("%i - Append some text to the file (formated, using printf)\n",  write_formated(fp, text3));
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // append a line to file2 (write_char() is already tested!) - OK
  puts("6. append a line to file2 (write_char() is already tested!)");
  printf("%i - Get append file pointer\n", get_append_file_pointer(&fp, file2));
  printf("%i - Append some text to the file (characterwise)\n", write_char(fp, text4, text4_size));
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  //
  printf("\'%s\' contains now 3 \"lines\" more!\n\n", file2);
  //*/

  // reading linewise and get read file pointer - 3 lines more in file2 - OK
  puts("7. reading linewise and get read file pointer - 3 lines more in file2");
  printf("%i - Get read file pointer to %s\n", get_read_file_pointer(&fp, file2), file2);
  printf("%i - Reading from a file linewise\n", read_linewise(fp, &content, &content_size));
  printf("content:\n\'%s\'\n", content);
  strcpy(content,"");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // reading file characterwise - OK
  puts("8. reading file characterwise");
  printf("%i - Get read file pointer to %s\n", get_read_file_pointer(&fp, file2), file2);
  printf("%i - Reading file characterwise\n", read_char(fp, &content, &content_size));
  printf("content:\n\'%s\'\n", content);
  strcpy(content, "");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // read blockwise - OK
  puts("9. read blockwise");
  printf("%i - Get a Read pointer to %s\n", get_read_file_pointer(&fp, file2), file2);
  printf("%i - Read blockwise\n", read_blockwise(fp, content, content_size));
  printf("content:\n\'%s\'\n", content);
  strcpy(content, "");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // reading out without spaces nor line feed - same, 3 lines more - OK
  puts("10. reading out without spaces nor line feed - same, 3 lines more");
  printf("%i - Get read file pointer to %s\n", get_read_file_pointer(&fp, file2), file2);
  printf("%i - Reading without spaces\n", read_without_spaces(fp, &content, &content_size));
  printf("content:\n\'%s\'\n", content);
  strcpy(content, "");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // copying buffered - without created file2 - OK (already done before)
  puts("11. copying buffered - without created file2 - OK (already done before)");
  printf("Copy characterwise and buffered...\n");
  printf("%i - Copying %s to %s characterwise, buffered\n", copy_characterwise_buffered(file2, file3, LIN_LEN), file2, file3);
  printf("Done.\n\n");
  //*/

  // copy characterwise and buffered
  puts("12. copy characterwise and buffered");
  printf("Copy characterwise and unbuffered...\n");
  printf("%i - Copying %s to %s characterwise, unbuffered\n", copy_characterwise_unbuffered(file2, "Jill.txt"), file2, "Jill.txt");
  printf("Done.\n\n");
  //*/

  // renaming the file3 - OK
  puts("13. renaming the file3");
  printf("%i - Renaming the file \'%s\' to \'Jack.txt\'\n", rename_file(file1, "Jack.txt"), file1);
  printf("Done.\n\n");
  //*/

  // read a single line - OK
  puts("14. read a single line");
  printf("%i - Check out the read/write pointer to \'%s\'\n", get_read_write_file_pointer(&fp, file3), file3);
  printf("%i - Read the %i. line (starting with 1) of \'%s\'\n", read_nth_line(fp, content, content_size, 3), 3, file3);
  printf("content:\n\'%s\'\n", content);
  strcpy(content,"");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // read a line interval - OK
  puts("15. read a line interval");
  printf("%i - check out the read/write pointer to \'%s\'\n", get_read_write_file_pointer(&fp, file3), file3);
  printf("%i - Read \'%s\' from line %i until %i\n", read_lines(fp, content, content_size, 2, 4), file3, 2, 4);
  printf("content:\n\'%s\'\n\n", content);
  strcpy(content, "");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  /*
  // read without eof - OK
  puts("16. read without eof");
  printf("%i - Check out the read/write pointer to \'%s\'\n", get_read_write_file_pointer(&fp, file3), file3);
  printf("%i - Read \'%s\' without eof\n", read_without_eof(fp, content, content_size), file3);
  printf("content:\n\'%s\'\n\n", content);
  strcpy(content, "");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  //* File Data - OK
  puts("17. File Data");
  printf("Show temp file data:\n");
  printf("%i - Number of allowed temp files\n", number_of_tempfiles());
  printf("%i - Number of characters in static temp\n", number_of_characters_in_static_temp());
  printf("%i - Get BUFSIZ\n", get_bufsize());
  printf("Done.\n\n");
  //*/

  //* File Size - OK
  puts("18. File Size");
  unsigned long file_size = 0;
  printf("Check File \"%s\"\n", file3);
  printf("%i - Get a read & write pointer\n", get_read_write_file_pointer(&fp, file3));
  printf("%i - The file \'%s\' (function failed) ", filesize(fp, &file_size), file3);
  printf("contains %ld tokens.\n", file_size);
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // Check EOF - OK
  puts("19. Check EOF");
  printf("Check \"%s\" for EOF\n", file3);
  printf("%i - Get a read & write pointer\n", get_read_write_file_pointer(&fp, file3));
  printf("%i - Check eof of \'%s\' - will be 1!!!\n", check_eof(fp), file3);
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // Check for Error - OK
  puts("20. Check for Error");
  printf("Check \"%s\" for errors\n", file3);
  printf("%i - Get a read & write pointer\n", get_read_write_file_pointer(&fp, file3));
  int iError = check_error(fp);
  printf("%i - Check \'%s\' for errors:\'%s\'\n", iError, file3, ((check_error(fp) == 0)?("No errors!"):("containts errors")));
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  //* Shredding file2 - OK
  puts("21. Shredding file2");
  printf("%i - Get a read & write pointer\n", get_read_write_file_pointer(&fp, file2));
  printf("%i - Shredding \'%s\'\n", shred_file(file2), file2);
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  //* Text with pattern - OK
  puts("22. Text with pattern");
  printf("Read text with a pattern\n");
  memset(content, '\0', content_size);
  printf("%i - Get a read & write pointer\n", get_read_write_file_pointer(&fp, file3));
  printf("%i - Reading lines with pattern \"%s\"\n", read_lines_with_pattern(fp, content, content_size, "he"), "he");
  printf("\tResult (pattern):\n\"%s\"\n", content);
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // Temporary files - File: "tmp.txt"
  puts("23. Temporary files - File: \"tmp.txt\"");
  char szTmp[] = "tmp.txt";
  memset(content, '\0', content_size);
  printf("%i - Create temp file pointer (read/write): %s\n", create_tmp(&fp, szTmp), szTmp);
  printf("%i - Append some text to the \'%s\' (linewise)\n", write_linewise(fp, text1, text1_size), szTmp);
  printf("%i - Reading from a file linewise\n", read_linewise(fp, &content, &content_size));
  printf("\tcontent:\n\'%s\'\n", content);
  printf("%i - And close the \'%s\' file again\n", close_tmp(&fp), szTmp);
  printf("Done.\n\n");
  //*/

  // ########### tmp files end ###


  /* write structs - demo
  printf("Write blockwise \'%s\'\n", file2);
  printf("%i - Get a read & write pointer\n", get_read_write_file_pointer(&fp, file2));
  printf("NO TEST HERE: THE FUNCTION IS A DEMO HOW TO WRITE STRUCTS DIRECTLY!\n");
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");
  //*/

  // free
  printf("free()\n");


  printf("READY.\n");
  return EXIT_SUCCESS;
}
