#include "./heads/home_lib.h"
#include "./heads/gene_lib.h"
#include "./heads/argu_lib.h"
#include "./heads/memo_lib.h"
#include "./heads/requ_lib.h"
#include "./heads/pthr_lib.h"
#include "./heads/send_lib.h"

/**
 * START
 * 
 * Author:
 *      Mr. Owen Thomas Claxton
 *      owenclaxton@gmail.com
 * 
 * Project:
 *      Webserver requesting
 * 
 * Compilation:
 *      This file does not compile on its' own but
 *      serves as an example of my capability for 
 *      software complexity.
 * 
 * END
 **/

// handle any signal sent.
void controller_sig_handler(int signal) {
    if (signal == SIGINT) { // handle SIGINT

        if (DEBUG == 1) fprintf(stdout, "\n[controller_sig_handler()] SIGINT Received.\n");
        SIGINT_RECEIVED = true;

    } else if (signal == SIGCHLD) {

        if (DEBUG == 1) fprintf(stdout, "\n[controller_sig_handler()] SIGCHLD Received.\n");
        SIGCHLD_RECEIVED = true;
    }
}

//Function for printing controllers usage
void do_help(int argc, char* argv[]) {

    fprintf(stdout, "Usage: controller <address> <port> {[-o out_file] [-log log_file]\n");
    fprintf(stdout, "[-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
    exit(1);
}

//Function for printing controllers usage
void usage_help(int argc, char* argv[]) {

    fprintf(stderr, "Usage: controller <address> <port> {[-o out_file] [-log log_file]\n");
    fprintf(stderr, "[-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
    exit(1);
}

//Function for printing out all input client arguments for debugging purposes
void exit_debug(int code, int argc, char *argv[], arg_lst* list_argu_ptr) {
    if (DEBUG == 1) {
        for (int i = 0; i < argc; i++) {
            
            fprintf(stdout, "[exit_debug()] Argument <%i>: <%s>\n", i, argv[i]);
        }
    }
    if (list_argu_ptr != NULL) {
        
        clear_arguments(list_argu_ptr);
        free(list_argu_ptr);
    }
    if (code == EXIT_USAGE_) {

        usage_help(argc, argv);
    }

    _Exit(code);
}

// function that checks if termination bools are ticked
int check_cleanup(int argc, char* argv[], arg_lst* list_argu_ptr) {
    if (SIGINT_RECEIVED == true) {
        if (argc != -1) {

            exit_debug(2, argc, argv, list_argu_ptr); 
        }
        return SIGINT_CHECK;
    } else if (SIGCHLD_RECEIVED == true) {

        return SIGCHLD_CHECK;
    }
    return 0;
}

// Function for handling the optional arguments
void do_optional_args(int argc, char* argv[], arg_lst* list_argu_ptr) {
    //Initialise struct containing parameters for each argument that could be included
    static const struct option longopts[] = {
        {.name = "out",     .has_arg = required_argument,   .val = 'o'},
        {.name = "log",     .has_arg = required_argument,   .val = 'l'},
        {.name = "time",    .has_arg = required_argument,   .val = 't'},
        {.name = "help",    .has_arg = no_argument,         .val = 'h'},
        //{.name = 0,         .has_arg = 0,                   .val = 0}, // for catching the bad boys
    };

    //Check if these arguments have been included when the controller was run
    bool getting_optional_arguments = true;
    while (getting_optional_arguments) {
        int opt = getopt_long_only(argc, argv, ":ht:l:o:", longopts, 0);

        arg_ele* arg_ele_ptr;
        switch(opt) {

            case 'o':
                arg_ele_ptr = add_argument(list_argu_ptr, OUT_, optarg, 2, NULL);
                if (arg_ele_ptr == NULL) {

                    if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: -o add_argument failed.\n");
                    free(arg_ele_ptr);
                    exit_debug(1, argc, argv, list_argu_ptr);
                }
                break;

            case 'l':
                arg_ele_ptr = add_argument(list_argu_ptr, LOG_, optarg, 2, NULL);
                if (arg_ele_ptr == NULL) {

                    if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: -l add_argument failed.\n");
                    free(arg_ele_ptr);
                    exit_debug(1, argc, argv, list_argu_ptr);
                }
                break;

            case 't':
                if (is_double(optarg)) {
                    arg_ele_ptr = add_argument(list_argu_ptr, TIME_, optarg, 2, NULL);
                    if (arg_ele_ptr == NULL) {

                        if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: -t add_argument failed.\n");
                        free(arg_ele_ptr);
                        exit_debug(1, argc, argv, list_argu_ptr);
                    }
                } else {
                    if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: Invalid time value.\n");
                    if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Usage: %s <address> <port> {args}\n", argv[0]);
                    if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] See --help for more details.\n");
                    exit_debug(EXIT_USAGE_, argc, argv, list_argu_ptr);
                }
                break;

            case 'h':
                do_help(argc, argv);
                if (argc == list_argu_ptr->index) {

                    exit_debug(1, argc, argv, list_argu_ptr);

                } else {

                    arg_ele_ptr = add_argument(list_argu_ptr, HELP_, NULL, 1, NULL);
                    if (arg_ele_ptr == NULL) {
                        if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: -h add_argument failed.\n");
                        free(arg_ele_ptr);
                        exit_debug(1, argc, argv, list_argu_ptr);
                    }
                }
                break;

            case '?':
                if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: Missing value for <%s>.\n", argv[optind - 1]);
                exit_debug(1, argc, argv, list_argu_ptr);
                break;

            case ':':
                if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: Unexpected option <%c>.\n", (char) optopt);
                exit_debug(1, argc, argv, list_argu_ptr);
                break;

            default:
                if (optarg == NULL || (char*) optarg == (char*) NULL) { // should already be caught.

                    if (DEBUG == 1) fprintf(stdout, "[do_optional_args()] opt: <%i> optopt: <%c> optind: <%i> optarg: <%s>\n", (int) opt, (char) optopt, (int) optind, (char*) optarg);
                    getting_optional_arguments = false;
                    break;

                } else {

                    if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: Optional argument with no specification.\n"); 
                }
                exit_debug(1, argc, argv, list_argu_ptr);
                break;
        }
    }
}

// Function to check all required arguments are acceptable and store them
struct hostent* do_required_args(int argc, char* argv[], arg_lst* list_argu_ptr) {

    struct hostent* he;
    //Check if address and port were input
    if (argc == list_argu_ptr->index) { // if only ./controller {optional args}

        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: Missing address and port!\n");
        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Usage: %s <address> <port> {args}\n", argv[0]);
        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] See --help for more details.\n");
        exit_debug(EXIT_USAGE_, argc, argv, list_argu_ptr);

    } else if (argc == list_argu_ptr->index + 1) { // if only ./controller <req> {optional args}

        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: Missing port!\n");
        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Usage: %s <address> <port> {args}\n", argv[0]);
        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] See --help for more details.\n");
        exit_debug(EXIT_USAGE_, argc, argv, list_argu_ptr);

    } else { // two req args, ./controller <req> <req> {optional args}. assume address and port.
        //Get the controllers info
        if ((he = gethostbyname(argv[list_argu_ptr->index])) == NULL) { // assumption was incorrect
            
            if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: Failed to get host info. Host: %s\n", (char*) argv[list_argu_ptr->index]);
            if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Usage: %s <address> <port> {args}\n", argv[0]);
            if (DEBUG == 1) fprintf(stderr, "[do_required_args()] See --help for more details.\n");

            if (is_int(argv[list_argu_ptr->index + 1])) { // if port value is integer but address invalid

                fprintf(stderr, "Could not connect to overseer at %s %s\n", argv[list_argu_ptr->index], argv[list_argu_ptr->index + 1]); // REQUIRED
                exit_debug(1, argc, argv, list_argu_ptr);

            } else {

                exit_debug(EXIT_USAGE_, argc, argv, list_argu_ptr);
            }      
        }

        arg_ele* arg_ele_ptr = add_argument(list_argu_ptr, ADDRESS_, argv[list_argu_ptr->index], 1, NULL); // store address
        if (arg_ele_ptr == NULL) {

            if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: address add_argument failed.\n");
            free(arg_ele_ptr);
            exit_debug(1, argc, argv, list_argu_ptr);
        }

        arg_ele_ptr = add_argument(list_argu_ptr, PORT_, argv[list_argu_ptr->index], 1, NULL); // store port
        if (arg_ele_ptr == NULL) {

            if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: port add_argument failed.\n");
            free(arg_ele_ptr);
            exit_debug(1, argc, argv, list_argu_ptr);
        }
    }

    //Check to see if the user input mem or memkill as an argument
    while (argc > list_argu_ptr->index) { // while more then two req args
        if (strcmp(argv[list_argu_ptr->index], "mem") == 0 && list_argu_ptr->mem_exists == false) {
            if (argc > list_argu_ptr->index + 1) { // if another req after mem
                if (is_int(argv[list_argu_ptr->index + 1])) { // if mem amount is an integer

                    arg_ele* arg_ele_ptr = add_argument(list_argu_ptr, MEM_, argv[list_argu_ptr->index + 1], 2, NULL); // store mem
                    if (arg_ele_ptr == NULL) {

                        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: mem (pid) add_argument failed.\n");
                        free(arg_ele_ptr);
                        exit_debug(1, argc, argv, list_argu_ptr);
                    }
                } else {   //If an invalid input to the mem argument

                    if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: Invalid mem PID.\n");
                    if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Usage: %s <address> <port> {args}\n", argv[0]);
                    if (DEBUG == 1) fprintf(stderr, "[do_required_args()] See --help for more details.\n");
                    exit_debug(1, argc, argv, list_argu_ptr);

                }
            } else { // if there was no specified pid
                
                arg_ele* arg_ele_ptr = add_argument(list_argu_ptr, MEM_, NULL, 1, NULL);  // store mem
                if (arg_ele_ptr == NULL) {

                    if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: mem (null) add_argument failed.\n");
                    free(arg_ele_ptr);
                    exit_debug(1, argc, argv, list_argu_ptr);
                }
            
            }
        } else if (strcmp(argv[list_argu_ptr->index], "memkill") == 0 && list_argu_ptr->memkill_exists == false) {
            //memkill
            if (argc > list_argu_ptr->index + 1) {// if there is an argument afterwards
                if (is_double(argv[list_argu_ptr->index + 1])) { // if the percent arg is a double

                    double perc = strtod(argv[list_argu_ptr->index + 1], NULL);
                    if ( (int) floor(perc) >= 0 && (int) ceil(perc) <= 100 ) { // ensure percent is bounded between 0 and 100
                        
                        arg_ele* arg_ele_ptr = add_argument(list_argu_ptr, MEMKILL_, argv[list_argu_ptr->index + 1], 2, NULL);
                        if (arg_ele_ptr == NULL) {

                            if (DEBUG == 1) fprintf(stderr, "[do_optional_args()] Error: memkill add_argument failed.\n");
                            free(arg_ele_ptr);
                            exit_debug(1, argc, argv, list_argu_ptr);
                        }
                    
                    } else {   //If invalid memkill percent input

                        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: Invalid memkill percent.\n");
                        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Usage: %s <address> <port> {args}\n", argv[0]);
                        if (DEBUG == 1) fprintf(stderr, "[do_required_args()] See --help for more details.\n");
                        exit_debug(1, argc, argv, list_argu_ptr);
                    }
                }
            } else {   //If memkill percent was not input

                if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: Missing memkill percent.\n");
                if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Usage: %s <address> <port> {args}\n", argv[0]);
                if (DEBUG == 1) fprintf(stderr, "[do_required_args()] See --help for more details.\n");
                exit_debug(1, argc, argv, list_argu_ptr);

            }
        } else {   //If none of the obove arguments, check if argument was a file

            if (list_argu_ptr->file_name_entered == false) {

                arg_ele* arg_ele_ptr = add_argument(list_argu_ptr, FILE_, argv[list_argu_ptr->index], 1, NULL);
                if (arg_ele_ptr == NULL) {

                    if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: file add_argument failed.\n");
                    free(arg_ele_ptr);
                    exit_debug(1, argc, argv, list_argu_ptr);
                }
                list_argu_ptr->file_name_entered = true;

            } else if (list_argu_ptr->arg_exists == false) { //Check if user input arguments after file name

                arg_ele* arg_ele_ptr = add_argument(list_argu_ptr, ARG_, argv[list_argu_ptr->index], 1, NULL);
                if (arg_ele_ptr == NULL) {

                    if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: arg add_argument failed.\n");
                    free(arg_ele_ptr);
                    exit_debug(1, argc, argv, list_argu_ptr);
                }
                list_argu_ptr->arg_count = list_argu_ptr->arg_count + 1;
                list_argu_ptr->arg_exists = true;

            } else {
                //collect and store all arguments input after a file argument is declared
                arg_ele* arg_ele_ptr = get_argument_ele(list_argu_ptr->head_ptr, ARG_);
                if (arg_ele_ptr == NULL) {

                   if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: arg_argument is NULL.\n"); // should never trigger.
                   free(arg_ele_ptr);
                   exit_debug(1, argc, argv, list_argu_ptr);
                }
                arg_ele_ptr = extend_argument(list_argu_ptr, arg_ele_ptr, argv[list_argu_ptr->index], " ", 1);
                if (arg_ele_ptr == NULL) {

                   if (DEBUG == 1) fprintf(stderr, "[do_required_args()] Error: arg_ele_ptr is NULL.\n"); // should never trigger.
                   free(arg_ele_ptr);
                   exit_debug(1, argc, argv, list_argu_ptr);
                }
            }
        }
    }
    return he;
}

int main(int argc, char* argv[])
{   
    if (argc > 1) { // get help check out of the way.
        if (strcmp(argv[1], "--help") == 0) {

            do_help(argc, argv);
            exit_debug(1, argc, argv, NULL);
        }
    }

    //Initialise SIGINT signal finder for detecting Ctr+C exit command
    signal(SIGINT,  controller_sig_handler);
    signal(SIGCHLD, controller_sig_handler);

    // Try allocate, error handle if fail
    arg_lst* list_argu_ptr = (arg_lst*) calloc(1, sizeof(arg_lst));
    if ( list_argu_ptr == NULL ) {
        if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: Failed to allocate memory list_argu_ptr.\n");
        free(list_argu_ptr);
        exit_debug(1, argc, argv, NULL);
    }
    initialise_argu_list(list_argu_ptr); // initialise as allocate successful.
    
    //Provides user with usage if first element
    if (argc == 1) { // caught later for if only args are optional.
        // this prevents a seg fault with help check
        if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: Missing address and port!\n");
        if (DEBUG == 1) fprintf(stderr, "[MAIN] Usage: %s <address> <port> {args}\n", argv[0]);
        if (DEBUG == 1) fprintf(stderr, "[MAIN] See --help for more details.\n");
        exit_debug(EXIT_USAGE_, argc, argv, list_argu_ptr);
    } 

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] Starting Client Connection.\n");

    //Initialise socket and socket address
    int sockfd;
    struct sockaddr_in their_addr; // connector's address information
    memset(&their_addr, 0, sizeof(their_addr)); // Clear address struct

    //Initialise list of arguments to be passed to the overseer

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] 1. Checking input arguments ...\n");

    if (DEBUG == 1) fprintf(stdout, "[MAIN] \t\tChecking optional arguments ...\n");
    do_optional_args(argc, argv, list_argu_ptr);
    if (DEBUG == 1) fprintf(stdout, "[MAIN] \t\tChecking required arguments ...\n");
    struct hostent* he = do_required_args(argc, argv, list_argu_ptr);

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] 2. Initialising socket ...\n");

    //Initialise the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {

        if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: Failed to initialise socket.\n");
        exit_debug(1, argc, argv, list_argu_ptr);

    }

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] 3. Preparing network details ...\n");

    //Initialise all the connection parameters
    char* retrieved_port = get_argument(list_argu_ptr->head_ptr, PORT_);
    if (retrieved_port != NULL) {

        their_addr.sin_family = AF_INET; // host byte order
        their_addr.sin_port = htons(atoi(retrieved_port)); // network byte order
        their_addr.sin_addr = *((struct in_addr *)he->h_addr); // host address

    } else {

        if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: retrieved_port is NULL!\n"); // should never trigger.
        exit_debug(1, argc, argv, list_argu_ptr);

    }

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] 4. Connecting to socket ...\n");

    //Connect to the socket (specified ip address)
    if (connect(sockfd, (struct sockaddr *) &their_addr, sizeof(struct sockaddr)) == -1)
    {
        char* retrieved_address = get_argument(list_argu_ptr->head_ptr, ADDRESS_);
        if (retrieved_address != NULL) {

            if (DEBUG == 1) fprintf(stderr, "[MAIN] Could not connect to overseer at %s:%i\n", retrieved_address, atoi(retrieved_port));
            char* current_time = print_time();
            fprintf(stderr, "Could not connect to overseer at %s %i\n", retrieved_address, atoi(retrieved_port)); // REQUIRED
            free(current_time);
            exit_debug(1, argc, argv, list_argu_ptr);

        } else {

            if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: retrieved_address is NULL!\n"); // should never trigger.
            exit_debug(1, argc, argv, list_argu_ptr);
        }
        
    }

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] 5. Preparing commands ...\n");

    //Make sure all arguments are only entered once (enfore mutually exclusive options)
    if (list_argu_ptr->file_exists + list_argu_ptr->mem_exists + list_argu_ptr->memkill_exists > 1) {
        if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: Using mutually exclusive arguments.\n");
        if (DEBUG == 1) fprintf(stderr, "[MAIN] Can only have one of the following: \n");
        if (DEBUG == 1) fprintf(stderr, "[MAIN] mem: %s, memkill: %s, file: %s\n", list_argu_ptr->mem_exists ? "true" : "false",  list_argu_ptr->memkill_exists ? "true" : "false", list_argu_ptr->file_exists ? "true" : "false");
        if (DEBUG == 1) fprintf(stderr, "[MAIN] Usage: %s <address> <port> {args}\n", argv[0]);
        if (DEBUG == 1) fprintf(stderr, "[MAIN] See --help for more details.\n");

        exit_debug(1, argc, argv, list_argu_ptr);
    } else if (list_argu_ptr->file_exists + list_argu_ptr->mem_exists + list_argu_ptr->memkill_exists == 0) {
        arg_ele* arg_ele_ptr = add_argument(list_argu_ptr, PING_, "ping", 0, NULL);
        if (arg_ele_ptr == NULL) {
            if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: ping add_argument failed.\n");
            free(arg_ele_ptr);
            exit_debug(1, argc, argv, list_argu_ptr);

        }
    }

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] 6. Transmitting data ...\n");
    // put catch to prevent sending address and port
    send_argument_list(sockfd, list_argu_ptr);

    if (DEBUG == 1) argument_list_print(list_argu_ptr->head_ptr);

    clear_arguments(list_argu_ptr); //Free all old memory

    check_cleanup(argc, argv, list_argu_ptr); // check for termination.
    if (DEBUG == 1) fprintf(stdout, "[MAIN] 7. Listening for return ...\n");
    //Wait for respose from overseer to get request information and confirmation of data sent
    int parent_pid = getpid();
    int fork_pid = fork();

    // Listen for data and listen for SIGINT simultaneously
    if (fork_pid < 0) {
        if (DEBUG == 1) fprintf(stderr, "[MAIN] Error: Fork() failed. Terminating...");
        exit_debug(1, argc, argv, list_argu_ptr);

    } else if ( fork_pid == 0 ) {
        // child id

        int receiving_data_from_client = 1;
        while (receiving_data_from_client == 1) {

            if (DEBUG == 1) fprintf(stdout, "[MAIN] \tReceiving data ...\n");
            receiving_data_from_client = receive_argument_data(sockfd, list_argu_ptr); 
            
        }

        kill(parent_pid, SIGCHLD); // ensure parent process is killed.

        char* memory = get_argument(list_argu_ptr->head_ptr, MEM_);

        if (memory != NULL) {
            //Print arguments received from overseer
            print_mem(list_argu_ptr->head_ptr);
        }

   
        if (DEBUG == 1) fprintf(stdout, "[MAIN] \nFinished!\n");
        if (DEBUG == 1) fprintf(stdout, "[MAIN] Terminating child.\n");

        //Close the socket
        close(sockfd);      

        //Free all used memory
        exit_debug(EXIT_NORM_, argc, argv, list_argu_ptr);

    } else {

        // Create clock for timing actions
        struct timespec clock;
        clock_gettime(CLOCK_MONOTONIC,&clock);
        int start_time = (int) clock.tv_sec;
        int now_time;
        int seconds_past = 0;
        bool waiting = true; 

        while ( waiting == true ) {
            clock_gettime(CLOCK_MONOTONIC,&clock);
            now_time = (int) clock.tv_sec;

            if ((now_time - start_time) > seconds_past + 1) {
                seconds_past = seconds_past + 1;
            }
            if (check_cleanup(-1, NULL, NULL) == SIGINT_CHECK) {

                if (DEBUG == 1) fprintf(stdout, "[MAIN] Sending SIGKILL to PID: %i\n", fork_pid);
                kill(fork_pid, SIGKILL); // ensure child process is killed.

                break;
            }
            if (check_cleanup(-1, NULL, NULL) == SIGCHLD_CHECK) {

                waiting = false;

                break;
            }
        }
        if (DEBUG == 1) fprintf(stdout, "[MAIN] Terminating parent.\n");
    }

    if (DEBUG == 1) fprintf(stdout, "[MAIN] Goodbye!\n");

    //Close the socket
    close(sockfd);      

    //Free all used memory
    exit_debug(EXIT_NORM_, argc, argv, list_argu_ptr);

    return SUCCESS_;
}