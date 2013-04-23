/*
  rc5input.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ID        77                                         

#define START     16
#define STOP      17
#define SEPARATOR 32
#define NEGATIVE  33
#define REPEAT    33

#define EMPTY     -1
#define MAX       200

/*
  states
 */
char command_idx = -1;
char command_input[5];
char command_sequence[5];
char command_signed = 1;

void remote_control( char *rc5_command )
{
        int idx = 0;
        printf("\nremote control says: %i\n", *rc5_command);
        
        if( START == *rc5_command ){
                puts("START");  
                command_idx = 0;
                command_signed = 1;
                return;
        }

        // listening not enabled
        if( (-1 == command_idx) || (5 < command_idx) ){
                puts("BREAK - listen not enabled");  
                return;
        }

        if( STOP == *rc5_command ){
                puts("STOP");  
                if( 4 == command_idx ){
                        for( idx=0; idx<5; ++idx ){
                                command_sequence[idx] = command_input[idx];
                        }
                }
                command_idx = -1;
                command_signed = 1;
                for( idx=0; idx<5; ++idx ){
                        command_input[idx] = EMPTY;
                }
                return;
        }

        if( SEPARATOR == *rc5_command ){
                puts("SEPARATOR");  
                command_signed = 1;
                if( 0 == command_idx ){
                        puts("ID check");
                        // if fully parsed ID is not correct, stop listening
                        printf( "id %i\n", ID ); 
                        printf( "command_input[0] %i\n", command_input[0] ); 
                        if( ID != command_input[0] ){
                                perror("ERROR: wrong id");  
                                command_idx = -1;
                                return;
                        }
                }
                command_idx += 1;
                return;
        }

        if( NEGATIVE == *rc5_command ){
                puts("NEGATIVE");  
                if( (-1 == command_signed) || (command_input[command_idx] != EMPTY ) ){
                        puts("ERROR: pressed negative more than once");  
                        command_idx = -1;
                        return;
                }
                command_signed = -1;
                return;
        }

        if( (0 == *rc5_command)
            || (1 == *rc5_command)
            || (2 == *rc5_command)
            || (3 == *rc5_command)
            || (4 == *rc5_command)
            || (5 == *rc5_command)
            || (6 == *rc5_command)
            || (7 == *rc5_command)
            || (8 == *rc5_command)
            || (9 == *rc5_command) ){
                puts("READ OUT NUMBER");  

                printf( "EMPTY %d\n", EMPTY );  
                printf( "command_input[command_idx] %d\n", command_input[command_idx] );  

                if( EMPTY != command_input[command_idx] ){
                        puts( "\tfurther digits");  
// TODO check max value!
/*
                        if( MAX < command_input[command_idx]){
                                puts("ERROR: overrun");
                                command_idx = -1;
                                return;
                        }
//*/
                        command_input[command_idx] = 10 * command_input[command_idx];
                }else{
                        puts( "\tfirst digit" );  
                        command_input[command_idx] = 0;
                }
                puts( "\tadd new reading and multiply with signedness" );  
                command_input[command_idx] += command_signed * (*rc5_command);
        }else{
                puts("ERROR: wrong key pressed");  
                command_idx = -1;
                return;
        }
}

int main( int argc, char** argv )
{
        /*
          id      = 7
          x_orig  = 1
          y_orig  = 2
          x_next  = 3
          y_next  = 4
        */
// CAUTION: id = 7
//  char input[] = {16, 7, 32, 1, 32, 2, 32, 3, 32, 4, 17 };
//  char input[] = {16, 7, 32, 1, 32, 2, 32, 3, 32, 4, 17 };

        /*
          id      = 77
          x_orig  = 11
          y_orig  = 22
          x_next  = 33
          y_next  = 44
        */
//  char input[] = {16, 7, 7, 32, 1, 1, 32, 2, 2, 32, 3, 3, 32, 4, 4, 17 };

        /*
          id      = 77
          x_orig  = 11
          y_orig  = -22
          x_next  = 33
          y_next  = -44
        */
//        char input[] = {16, 7, 7, 32, 1, 1, 32, 33, 2, 2, 32, 3, 3, 32, 33, 4, 4, 17 };

// FAIL - negative in between
//        char input[] = {16, 7, 7, 32, 1, 1, 32, 2, 2, 32, 3, 3, 32, 4, 33, 4, 17, 1 };

// FAIL - number too short
//        char input[] = {16, 7, 7, 32, 1, 1, 32, 2, 2, 32, 3, 17 };

        char input[] = {16, 7, 7, 32, 1,9,9, 32, 2, 5, 32, 2, 5, 32, 2, 5, 17 };

// TODO error in signedness (more than one, within number, etc)
// TODO error not having 17 at end

        int idx=0;
        char *rc5_command = input;

        // reset
        for( idx=0; idx<5; ++idx ){
                command_input[idx] = EMPTY;
                command_sequence[idx] = EMPTY;
        }

        // remote control
        for( idx=0; idx<sizeof(input); idx++, rc5_command++ ){
                remote_control( rc5_command );
        }
        printf("\n\n");

        // evaluation
        printf("result:\n");
        for( idx=0; idx<sizeof(command_sequence); ++idx){
                printf("%i\n", command_sequence[idx] );
        }

        printf("READY.\n");
        exit( EXIT_SUCCESS );
}
