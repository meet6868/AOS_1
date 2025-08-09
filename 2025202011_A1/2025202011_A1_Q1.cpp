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
#include <stdlib.h> 

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
        write(1,info_prefix,strlen(info_prefix));
        write(1,msg,strlen(msg));
        write(1,"\n",1);

    }
}

void reverse_block(char* block, int length) {
    for (int i = 0; i < length / 2; ++i) {
        char temp = block[i];
        block[i] = block[length - i - 1];
        block[length - i - 1] = temp;
    }
}

void copy_string(char* new_s, const char* old_s, int max_len=4048) {
    int i = 0;
    while (i < max_len - 1 && old_s[i] != '\0') {
        new_s[i] = old_s[i];
        i++;
    }
    new_s[i] = '\0'; 
}

void concat_string(char* s1,const char* s2, int max_len=4048){
    int i=0;
    while (i<max_len-1 && s1[i]!='\0') i++;

    int j=0;
    while(i<max_len-1 && s2[j]!='\0'){
        s1[i]=s2[j];
        i++;
        j++;
    }
    s1[i]='\0';
}

void long_long_to_str(long long num, char* str) {
    if (num < 0) {
        consol_out("Negative Numebr is not allowed in consol input",true);
    }

    long long i = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }
    str[i] = '\0';

    reverse_block(str, i);
}

void combine_path(char* path, const char* base_dir, const char* file_name, int max_len = 4096) {
    path[0] = '\0'; 
    concat_string(path, base_dir, max_len);
    int len = 0;
    while (path[len] != '\0') len++;
    if (len > 0 && path[len - 1] != '/') {
        concat_string(path, "/", max_len);
    }
    concat_string(path, file_name, max_len);
}

void file_validation_check(const char* file_name){

    struct stat file_status;
    if(stat(file_name, &file_status) == -1){
        consol_out("File does not exist or cannot be accessed !!", true);
    }


    int fd=open(file_name,O_RDONLY );
    if(fd==-1){
        consol_out("Owner does not have read/write permission.", true);

    }
    consol_out("File Existed and permission Checked");

}

bool check_is_number(const char* num) {
    if(num[0]=='-'){
        consol_out("Number is not start form - , It should not be Negative",true);
    }
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

void flag_2_extra_argument_validation(long long start,long long end,const char* file_name){
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


    const char* base_file = basename(path); 
    char* ass_dir=(char*)malloc(strlen(input_path) + 1);
    if (!ass_dir) {
        perror("malloc failed");
        _exit(1);
    }
    copy_string(ass_dir, "Assignment1");

    struct stat st;
    if (stat(ass_dir, &st) == -1) {
        if (mkdir(ass_dir, 0700) == -1) {
            perror("Mkdir failed"); 
            _exit(1);
        }
    } 


    int len = strlen(base_file) + 3;
    char* file_name = (char*)malloc(len);
    if (!file_name) {
        perror("malloc failed");
        _exit(1);
    }
    char flag_str[2];
    flag_str[0] = '0' + flag;
    flag_str[1] = '\0';
    file_name[0] = '\0';

    concat_string(file_name, flag_str);
    concat_string(file_name, "_");
    concat_string(file_name, base_file);

    
    int full_path_size=strlen(ass_dir) + strlen(file_name) + 2;
    char* full_path=(char*)malloc(full_path_size);
    if (!full_path) {
        perror("malloc failed");
        _exit(1);
    }
    combine_path(full_path, ass_dir, file_name);

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

void update_progress_bar(long long curr, long long n) {
    if (n <= 0) return; // avoid division by zero

    float per = ((float)curr / (float)n) * 100.0f; // percent value
    int bar_width = 30;

    char buff[200] = {0};

    if (curr >= n) {
        concat_string(buff, "\033[1;32m"); // green when complete
    } else {
        concat_string(buff, "\033[1;33m"); // yellow while in progress
    }

    concat_string(buff, "\r[Blocks ");

    char num_str[20];
    if(curr<10){
        concat_string(buff, "0");
    }
    long_long_to_str(curr, num_str);
    concat_string(buff, num_str);
    concat_string(buff, "/");

    long_long_to_str(n, num_str);
    concat_string(buff, num_str);
    concat_string(buff, "] [");

    int prog_str = (int)((per / 100.0f) * bar_width);
    for (int i = 0; i < bar_width; ++i) {
        char ch[2] = { (i < prog_str) ? '#' : '_', '\0' };
        concat_string(buff, ch);
    }
    concat_string(buff, "] ");

    char percent_str[20];
    snprintf(percent_str, sizeof(percent_str), "%.2f", per); // now real %
    concat_string(buff, percent_str);
    concat_string(buff, "%");
    concat_string(buff, "\033[0m");

    write(STDOUT_FILENO, buff, strlen(buff));
    fsync(STDOUT_FILENO);
    usleep(50000);
}


void flag_0_operation(const char* input_file, const char* output_file, const char* BLOCK_SIZE) {
    long long block_size = atoll(BLOCK_SIZE);
    if (block_size <= 0) {
        consol_out("Invalid block size",true);
        _exit(1);
    }

    int fd_in = open(input_file, O_RDONLY);
    if (fd_in < 0) {
        perror("Error opening input file");
        _exit(1);
    }

    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd_out < 0) {
        perror("Error opening/creating output file");
        close(fd_in);
        _exit(1);
    }

    off_t file_size = lseek(fd_in, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size with lseek");
        close(fd_in);
        close(fd_out);
        _exit(1);
    }
    lseek(fd_in, 0, SEEK_SET);

    char* block = new char[block_size];
    long long total_blocks = file_size / block_size;
    if (file_size % block_size != 0) total_blocks++;
    long long current_block = 0;

    while (true) {
        ssize_t bytes_read = read(fd_in, block, block_size);
        if (bytes_read < 0) {
            perror("Error reading from input file");
            break;
        }
        if (bytes_read == 0) { 
            break;
        }

        reverse_block(block, bytes_read);

        ssize_t bytes_written = write(fd_out, block, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to output file");
            break;
        }

        current_block++;
        if (current_block > total_blocks) current_block = total_blocks;
        update_progress_bar(current_block, total_blocks);
        
    }

    write(1,"\n",1);
    delete[] block;
    close(fd_in);
    close(fd_out);

}

void flag_1_operation(const char* input_file, const char* output_file){
    int fd_in = open(input_file, O_RDONLY);
    if (fd_in < 0) {
        perror("Error opening input file");
        _exit(1);
    }

    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd_out < 0) {
        perror("Error opening/creating output file");
        close(fd_in);
        _exit(1);
    }

    off_t file_size = lseek(fd_in, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size with lseek");
        close(fd_in);
        close(fd_out);
        _exit(1);
    }
    off_t offset = file_size;
    long long block_size = 1024;
    char* block = new char[block_size];

    long long total_blocks = file_size / block_size;
    if (file_size % block_size != 0) total_blocks++;
    long long current_block = 0;

    while (offset > 0) {
        long long bytes_to_read = (offset >= block_size) ? block_size : offset;
        offset -= bytes_to_read;
        if (lseek(fd_in, offset, SEEK_SET) == -1) {
            perror("Error seeking in input file");
            break;
        }

        ssize_t bytes_read = read(fd_in, block, bytes_to_read);
        if (bytes_read != bytes_to_read) {
            perror("Error reading from input file");
            break;
        }

        reverse_block(block, bytes_read);

        ssize_t bytes_written = write(fd_out, block, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to output file");
            break;
        }

        current_block++;
        if (current_block > total_blocks) current_block = total_blocks;
        update_progress_bar(current_block, total_blocks);
    }
    write(1,"\n",1);
    delete[] block;
    close(fd_in);
    close(fd_out);
}

void flag_2_operation(const char* input_file, const char* output_file, long long start, long long end) {
    int fd_in = open(input_file, O_RDONLY);
    if (fd_in < 0) {
        perror("Error opening input file");
        _exit(1);
    }

    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd_out < 0) {
        perror("Error opening/creating output file");
        close(fd_in);
        _exit(1);
    }

    off_t file_size = lseek(fd_in, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size with lseek");
        close(fd_in);
        close(fd_out);
        _exit(1);
    }

    long long block_size = 256;
    char* block = new char[block_size];

    
    long long blocks_before = start / block_size + (start % block_size != 0 ? 1 : 0);
    long long blocks_middle = (end - start) / block_size + ((end - start) % block_size != 0 ? 1 : 0);
    long long blocks_after = (file_size - end) / block_size + ((file_size - end) % block_size != 0 ? 1 : 0);
    long long total_blocks = blocks_before + blocks_middle + blocks_after;
    long long completed_blocks = 0;

    off_t offset = start;
    while (offset > 0) {
        int bytes_to_read = (offset >= block_size) ? block_size : offset;
        offset -= bytes_to_read;

        if (lseek(fd_in, offset, SEEK_SET) == -1) {
            perror("Seek error before start");
            break;
        }

        ssize_t bytes_read = read(fd_in, block, bytes_to_read);
        if (bytes_read != bytes_to_read) {
            perror("Read error before start");
            break;
        }

        reverse_block(block, bytes_read);

        ssize_t bytes_written = write(fd_out, block, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Write error before start");
            break;
        }

        completed_blocks++;
        update_progress_bar(completed_blocks, total_blocks);
        
    }

    if (lseek(fd_in, start, SEEK_SET) == -1) {
        perror("Seek error at unchanged part");
    }

    off_t s = start;
    while (s < end) {
        int bytes_to_read = (end - s >= block_size) ? block_size : (end - s);

        ssize_t bytes_read = read(fd_in, block, bytes_to_read);
        if (bytes_read != bytes_to_read) {
            perror("Read error in unchanged part");
            break;
        }

        ssize_t bytes_written = write(fd_out, block, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Write error in unchanged part");
            break;
        }

        s += bytes_to_read;
        completed_blocks++;
        update_progress_bar(completed_blocks, total_blocks);
        sleep(1);
    }

   
    offset = file_size;
    while (offset > end) {
        int bytes_to_read = (offset - end >= block_size) ? block_size : (offset - end);
        offset -= bytes_to_read;

        if (lseek(fd_in, offset, SEEK_SET) == -1) {
            perror("Seek error after end");
            break;
        }

        ssize_t bytes_read = read(fd_in, block, bytes_to_read);
        if (bytes_read != bytes_to_read) {
            perror("Read error after end");
            break;
        }

        reverse_block(block, bytes_read);

        ssize_t bytes_written = write(fd_out, block, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Write error after end");
            break;
        }

        completed_blocks++;
        update_progress_bar(completed_blocks, total_blocks);
        sleep(1);
    }

    write(1, "\n", 1);

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
        long long start=atoll(argv[3]);
        long long end=atoll(argv[4]);
        flag_2_extra_argument_validation(start,end,input_file); 
        flag_2_operation(input_file,output_file,start,end);
    }

    consol_out("You File Generated at Output file at");
    consol_out(output_file);


    return 0;
}