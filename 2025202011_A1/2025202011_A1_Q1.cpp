#include <unistd.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>
#include <errno.h>
#include <stdio.h>

void copy_string(char* new_s, const char* old_S, int max_len=4048) {
    int i = 0;
    while (i < max_len - 1 && old_S[i] != '\0') {
        new_s[i] = old_S[i];
        i++;
    }
    new_s[i] = '\0'; 
}

void consol_out(const char* msg,bool is_Error=false){
    if(is_Error){
        const char* error_prefix="\e[1;31mError : \e[0m";
        write(2,error_prefix,strlen(error_prefix));
        write(2,msg,strlen(msg));
        write(2,"\n",1);
        _exit(1);
    }
    else{
        const char* info_prefix="\e[1;33mInfo : \e[0m";
        write(2,info_prefix,strlen(info_prefix));
        write(1,msg,strlen(msg));
        write(1,"\n",1);

    }
}

void check_directory_permision(const char* file_path){
    char dir_path[PATH_MAX];
    copy_string(dir_path, file_path, PATH_MAX);  
    char* dir_name = dirname(dir_path);   

    struct stat dir_status;
    if (stat(dir_name, &dir_status) == -1) {
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
    consol_out("Directory permission Checked");
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
    consol_out("File Existed and permission Checked");


}

void file_validation_check(const char* file_name){

    check_file_permision(file_name);
    check_directory_permision(file_name);
}

bool check_is_number(const char* num) {
    for (int i = 0; num[i] != '\0'; i++) {
        if (num[i] < '0' || num[i] > '9') {
            return false;
        }
    }
    return true;
}

void flag_validation(const char* flag_arg, int argc){
    if (!check_is_number(flag_arg)) {
            consol_out("Please enter flag in numeric", true);
    }

    int flag=atoi(flag_arg);
    if(flag==0){
        if(argc!=4) consol_out("Your number of argument is not according to flag 0",true);
    }
    else if(flag==1){
        if(argc!=3) consol_out("Your number of argument is not according to flag 1",true);
    }
    else if(flag==2){
        if(argc!=5) consol_out("Your number of argument is not according to flag 2",true);
    }
    else{
        consol_out("Invalid flag. Must be 0, 1, or 2 !!!",true);
    }
    consol_out("Flags Validation checked");

}

void extra_argument_validation(int flag,char* argv[]){
    if (flag == 0) {
        if (!check_is_number(argv[3])) {
            consol_out("Please enter block size in positive numeric", true);
        }
    }

    if (flag == 2) {
        if (!check_is_number(argv[3]) || !check_is_number(argv[4])) {
            consol_out("Please enter positive numeric values for start and end index", true);
        }
        else{
            int start_index=atoi(argv[3]);
            int end_index=atoi(argv[4]);
            if(start_index>end_index){
                consol_out("Please enter numeric of start and end index like start less than equal to end index", true);
            }
        }
    }
    consol_out("Extra Argument Validation checked");
}

void flag_2_extra_argument_validation(int start,int end,const char* file_name){
    if(start<0){
        consol_out("Start index must be grater than 0 !!1",true);
    }
    int fd_in = open(file_name, O_RDONLY);
    if(fd_in < 0){
        consol_out("Error opening file to validate end index", true);
        _exit(1);
    }

    off_t file_size = lseek(fd_in, 0, SEEK_END);
    close(fd_in);

    if(end > file_size){
        consol_out("End index exceeds file length!", true);
    }

    if(start >= end){
        consol_out("Start index must be less than End index!", true);
    }
    consol_out("Flag 2 extra argument validation Checked");
}

const char* generate_output_file(const char* input_path, int flag) {
    char* path= (char*)malloc(strlen(input_path) + 1);
    if (!path) {
        perror("malloc failed");
        _exit(1);
    }
    copy_string(path,input_path);

    const char* base_dir = dirname(path);
    const char* base_file = basename(path); 
    char ass_dir[1024];
    snprintf(ass_dir, sizeof(ass_dir), "%s/Assignment1", base_dir);

    struct stat st;
    if (stat(ass_dir, &st) == -1) {
        if (mkdir(ass_dir, 0700) == -1) {
            perror("Mkdir failed"); 
            _exit(1);
        }
    } else if (!S_ISDIR(st.st_mode)) {
        consol_out("Assignment1 exists but is not a directory", true); 
        _exit(1);
    }


    char file_name[512];
    snprintf(file_name, sizeof(file_name), "%d_%s", flag, base_file);

    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s/%s", ass_dir, file_name);

    int fd = open(full_path, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fd == -1) {
        consol_out("File creation failed",true);
        _exit(1);
    }
    close(fd);

    char* result = (char*)malloc(strlen(full_path) + 1);
    if (!result) {
        perror("malloc failed");
        _exit(1);
    }
    copy_string(result, full_path);
   
    return result;
}

void reverse_block(char* block, int length) {
    for (int i = 0; i < length / 2; ++i) {
        char temp = block[i];
        block[i] = block[length - i - 1];
        block[length - i - 1] = temp;
    }
}

void update_progress_bar(int curr, int n) {
    float per = (float)curr / n;
    int bar_width = 30;

    char buff[100];
    int len = snprintf(buff, sizeof(buff), "\r[Block %d/%d] [", curr, n);
    
    int prog_str = (int)(per * bar_width);
    for (int i = 0; i < bar_width; ++i) {
        buff[len++] = (i < prog_str) ? '=' : ' ';
    }
    buff[len++] = ']';

    len += snprintf(buff + len, sizeof(buff) - len, " %3.0f%%", per * 100);

    write(STDOUT_FILENO, buff, len);
    fsync(STDOUT_FILENO);
}

void flag_0_operation(const char* input_file, const char* output_file, const char* BLOCK_SIZE) {
    int block_size = atoi(BLOCK_SIZE);
    if (block_size <= 0) {
        consol_out("Invalid block size",true);
        _exit(1);
    }

    int fd_in = open(input_file, O_RDONLY);
    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);

    off_t file_size = lseek(fd_in, 0, SEEK_END);
    char* block = new char[block_size];
    off_t offset = file_size;
    int total_blocks = (file_size + block_size - 1) / block_size;
    int current_block = 0;

    while (offset > 0) {
        int bytes_to_read = (offset >= block_size) ? block_size : offset;
        offset -= bytes_to_read;
        lseek(fd_in, offset, SEEK_SET);
        read(fd_in, block, bytes_to_read);
        current_block++;
        if (current_block > total_blocks) current_block = total_blocks;
        update_progress_bar(current_block, total_blocks);

        reverse_block(block, bytes_to_read);
        write(fd_out, block, bytes_to_read);
        sleep(1);
    }

    delete[] block;
    close(fd_in);
    close(fd_out);
}

void flag_1_operation(const char* input_file, const char* output_file){
    int fd_in = open(input_file, O_RDONLY);
    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);

    off_t file_size = lseek(fd_in, 0, SEEK_END);
    off_t offset = file_size;
    int block_size = 256;
    char* block = new char[block_size];

    int total_blocks = (file_size + block_size - 1) / block_size;
    int current_block = 0;

    while (offset > 0) {
        int bytes_to_read = (offset >= block_size) ? block_size : offset;
        offset -= bytes_to_read;
        lseek(fd_in, offset, SEEK_SET);
        read(fd_in, block, bytes_to_read);
        current_block++;
        if (current_block > total_blocks) current_block = total_blocks;
        update_progress_bar(current_block, total_blocks);
        
        reverse_block(block, bytes_to_read);
        write(fd_out, block, bytes_to_read);
        sleep(1);
    }

    delete[] block;
    close(fd_in);
    close(fd_out);
}

void flag_2_operation(const char* input_file, const char* output_file,int start, int end){
    int fd_in = open(input_file, O_RDONLY);
    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    off_t file_size = lseek(fd_in, 0, SEEK_END);

    int block_size = 256;
    char* block = new char[block_size];
    lseek(fd_in, 0, SEEK_SET);
    off_t s = 0;
    while (s < start) {
        int bytes_to_read = (start - s >= block_size) ? block_size : (start - s);
        read(fd_in, block, bytes_to_read);
        write(fd_out, block, bytes_to_read);
        s += bytes_to_read;
    }

    
    off_t offset = end;
    int total_bytes = end - start;
    int total_blocks = (total_bytes + block_size - 1) / block_size;
    int current_block = 0;

    while (offset > start) {
        int bytes_to_read = (offset - start >= block_size) ? block_size : (offset - start);
        offset -= bytes_to_read;
        lseek(fd_in, offset, SEEK_SET);
        read(fd_in, block, bytes_to_read);
        current_block++;
        if (current_block > total_blocks) current_block = total_blocks;
        update_progress_bar(current_block, total_blocks);

        reverse_block(block, bytes_to_read);
        write(fd_out, block, bytes_to_read);
        sleep(1);
    }

    s = end;
    lseek(fd_in, s, SEEK_SET);
    while (s < file_size) {
        int bytes_to_read = (file_size - s >= block_size) ? block_size : (file_size - s);
        read(fd_in, block, bytes_to_read);
        write(fd_out, block, bytes_to_read);
        s += bytes_to_read;
    }

    delete[] block;
    close(fd_in);
    close(fd_out);
}
int main(int argc ,char* argv[]){
    if(argc<3 ||  argc>5){
        consol_out("Please, Enter corrcet number of arguments !!!",true);
    }
    else{
        consol_out("Total Number of Argument Checked");

    }

    const char* input_file=argv[1];
    
    //validations
    file_validation_check(input_file);
    flag_validation(argv[2],argc);
    int flag=atoi(argv[2]);
    extra_argument_validation(flag,argv);
    const char* output_file=generate_output_file(input_file,flag);

    if(flag==0){
        flag_0_operation(input_file,output_file,argv[3]);
    }
    else if(flag==1){
        flag_1_operation(input_file,output_file);
    }
    else if(flag==2){
        int start=atoi(argv[3]);
        int end=atoi(argv[4]);
        flag_2_extra_argument_validation(start,end,input_file); 
        flag_2_operation(input_file,output_file,start,end);
    }

    consol_out("\nYou File Generated at Output file at");
    consol_out(output_file);


    return 0;
}