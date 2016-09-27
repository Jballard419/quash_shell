/**
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */

#include "execute.h"
#include <fcntl.h>
#include <stdio.h>
#include "deque.h"
#include "quash.h"


static char* env_val;
//pipe deque
 int plumber_pipes[2][2];
 //pid deque
 IMPLEMENT_DEQUE_STRUCT(PidQueue, int);
 IMPLEMENT_DEQUE(PidQueue, int);

 //pipe deque
 IMPLEMENT_DEQUE_STRUCT(plumber, int*);
 IMPLEMENT_DEQUE(plumber, int*);

 struct Job{
   int job_id;
   Command cmd;
   bool bg;
   bool done;
   pid_t first_job;
  //  struct PidQueue pid_Queue;
  //  struct plumber pipe_queue;
 };

 //jobs deque
 IMPLEMENT_DEQUE_STRUCT(JobQueue, struct Job);
 // PROTOTYPE_DEQUE(JobQueue, struct Job);
 IMPLEMENT_DEQUE(JobQueue, struct Job);

 struct State{
   struct Job workingJob;
   struct JobQueue job_queue;
   int jobNum;
 };

 struct State globalState;
/**
 * @brief Note calls to any function that requires implementation
 */
#define IMPLEMENT_ME()                                                  \
  printf("IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)

/***************************************************************************
 * Interface Functions
 ***************************************************************************/

// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
  // TODO: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  // cwd = getcwd(NULL,1024);
  // Change this to true if necessary
  *should_free = true;

  return  get_current_dir_name();
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {

  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple

  return getenv(env_var);
}

// Sets the environment variable env_var to the value val
void write_env(const char* env_var, const char* val) {
  // TODO: Write environment variables
  // HINT: This should be pretty simple

  setenv(env_var,val,1);
  }

// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all of the background jobs
  IMPLEMENT_ME();

  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path
void run_generic(GenericCommand cmd) {
  // Print an array of strings. The `str` array is a NULL terminated (last
  // character pointer is always NULL) list of strings. The first element in the
  // array is the executable
  char** str = cmd.args;
  execvp(str[0], str);
}

// Print strings
void run_echo(EchoCommand cmd) {
  // Print an array of strings. The args array is a NULL terminated (last
  // character pointer is always NULL) list of strings.
  char** str = cmd.args;
  int i= 0;
  while (str[i]!=NULL) {
    printf("%s ", str[i]);
    i=i+1;
  }
  printf("\n");
  //free(str);
  return;
}

// Sets an environment variable
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;
  write_env(env_var, val);
}

// Changes the current working directory
void run_cd(CDCommand cmd) {
  if(cmd.dir == " "){
    chdir(getenv("HOME"));

  }
  else{
    char* oldDir = getcwd(NULL, 1024);
    if(chdir(cmd.dir)==-1){
      fprintf(stderr, "error");
    }
    else{
      //changed

      write_env("PWD", cmd.dir);
      write_env("OLD_PWD",  oldDir);
      free(oldDir);
    //  delete(oldDir);
    }
  }
}

// Sends a signal to all processes contained in a job
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  // TODO: Remove warning silencers
  (void) signal; // Silence unused variable warning
  (void) job_id; // Silence unused variable warning

  // TODO: Kill a background job
  IMPLEMENT_ME();
}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  //IMPLEMENT_ME();
  char *buf = get_current_dir_name() ;
printf("%s\n",buf);
  free(buf);
  // Flush the buffer before returning
  fflush(stdout);
  return;
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  IMPLEMENT_ME();
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief An example of how to resolve a Command type from a @a Command union.
 *
 * You will need some variant(s) of this switch structure to direct the command
 * statements to their correct caller. Probably you won't want all commands
 * accessible from a single function like this.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case JOBS:
    run_jobs();
    break;

  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case EXPORT:
    exit(0);
    break;

  case CD:
    //exit(0);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}
/**
 * @brief Create a process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
void create_process(CommandHolder holder, int p_num, int plumber_pipes[2][2], struct Job* j) {
  // Read the flags field from the parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND;

  int new_id= p_num%2;
  int old_id= new_id? 0:1;

  //TODO:: are there other cases to consider?
  if(p_out)
  {
    pipe(plumber_pipes[new_id]);
  }

  int pid_id=fork();
  // if(!p_in){
  //   j->first_job =  pid_id;
  // }
  if(pid_id!=0){
    //printf("%s\n", "parent");
    if(get_command_type(holder.cmd) == CD || get_command_type(holder.cmd) == EXPORT)
    parent_run_command(holder.cmd);

    if (p_in){
      close(plumber_pipes[old_id][1]);
    }
    wait(pid_id);
  }
  else
  {
      //child
    if(r_in){
      int file_name= open(holder.redirect_in , O_RDONLY| O_CREAT , S_IRWXU);
      dup2(file_name,STDIN_FILENO );
      }
    if(r_out){
        int file_out= open(holder.redirect_out , O_WRONLY);
        dup2(file_out,STDOUT_FILENO);
    }
    if (p_in){
      close(plumber_pipes[new_id][0]);
      dup2(plumber_pipes[old_id][0],STDIN_FILENO);
    }
    if(p_out){
      dup2(plumber_pipes[new_id][1],STDOUT_FILENO);
      close(plumber_pipes[new_id][1]);
  }

  close(plumber_pipes[new_id][1]);
  close(plumber_pipes[old_id][1]);
  close(plumber_pipes[old_id][0]);

  if(get_command_type(holder.cmd) != CD && get_command_type(holder.cmd) != EXPORT)
    child_run_command(holder.cmd);

  exit(0);
  }
  (void) r_in;  // Silence unused variable warning
  (void) r_out; // Silence unused variable warning
  (void) r_app; // Silence unused variable warning
}

// Run a list of commands
void run_script(CommandHolder* holders) {

  int plumber_pipes[2][2];
  if (holders == NULL)
    return;

  check_jobs_bg_status();

  pipe(plumber_pipes[0]);
  pipe(plumber_pipes[1]);
  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }

  CommandType type;
  int num_processes =0;


  //create job here
  struct Job* newJob;
  //newJob = (struct Job*) malloc(sizeof(struct Job));
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i){
    create_process(holders[i], i,  plumber_pipes, newJob);
    num_processes++;
  }

  push_back_JobQueue(&globalState.job_queue, *newJob);
  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background Job
    // TODO: Wait for all processes under the job to complete
    IMPLEMENT_ME();


  }
  else {
    // A background job.
    // TODO: Push the new job to the job queue
    IMPLEMENT_ME();

    // TODO: Once jobs are implemented, uncomment and fill the following line
    // print_job_bg_start(job_id, pid, cmd);
  }
}
