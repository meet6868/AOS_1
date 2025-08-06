#include <unistd.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>


void consol_out(const char* msg,bool is_Error=false){
    if(is_Error){
        const char* error_prefix="\e[1;31mError : \e[0m";
        write(2,error_prefix,strlen(error_prefix));
        write(2,msg,strlen(msg));
        write(2,"\n",1);
        _exit(1);
    }
    else{
        const char* info_prefix="\e[1;32mInfo : \e[0m";
        write(2,info_prefix,strlen(info_prefix));
        write(1,msg,strlen(msg));
        write(1,"\n",1);

    }
}

void check_directory_permision(const char* file_path){
    char dir_path[PATH_MAX];
    strncpy(dir_path, file_path, PATH_MAX);  
    char* dir_name = dirname(dir_path);   

    struct stat dir_status;
    if (::stat(dir_name, &dir_status) == -1) {
        consol_out("Directory does not exist or cannot be accessed.", true);
    }

    uid_t uid=getuid();

    if (dir_status.st_uid == uid) {
        if (!(dir_status.st_mode & S_IRUSR) || !(dir_status.st_mode & S_IWUSR) || !(dir_status.st_mode & S_IXUSR)) {
            consol_out("Directory lacks read/write/execute permission for the owner.", true);
        }
    } else {
        consol_out("Current user is not the owner of the directory.", true);
    }
}

void check_file_permision(const char* file_path){
    struct stat file_status;

    if(stat(file_path, &file_status) == -1){
        consol_out("File does not exist or cannot be accessed !!", true);
    }
    uid_t uid=getuid();

    if(file_status.st_uid==uid){
        if(!(file_status.st_mode & S_IRUSR) || !(file_status.st_mode && S_IWUSR)){
            consol_out("Owner does not have read/write permission.", true);
        }
    }
    else{
        consol_out("Current user is not the owner of the file.", true);
    }

}

void file_validation_check(const char* file_name){

    check_file_permision(file_name);
    check_directory_permision(file_name);
}

int main(int argc ,char* argv[]){
    if(argc<2){
        consol_out("Please, Enter corrcet number of arguments !!!",true);
    }

    const char* input_file=argv[1];
    file_validation_check(input_file);
    if(argv[2]=="0"){
        consol_out("Flag 0");

    }
    else if(argv[2]=="1"){
        consol_out("Flag 1");
    }

    else if(argv[2]=="2"){
        consol_out("Flag 2");

    }
    else{
        consol_out("Invalid flag. Must be 0, 1, or 2 !!!",true);

    }

    
    
    consol_out("You validated files Checks");



    return 0;
}