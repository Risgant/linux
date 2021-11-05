#include <fcntl.h>
#include <unistd.h>
#include <string>

#define SUCCESS 0
#define SOFT_COPYING_ERROR -1
#define FATAL_COPYING_ERROR -2
#define FIRST_BUFFER_ERROR -3
#define SECOND_FIRST_ERROR -4
#define BUFFER_SECOND_ERROR -5

using namespace std;

string buffer_file = "file_buffer"+ to_string(rand());

int copying_error(bool file_changed) {
    if(file_changed) {
        return FATAL_COPYING_ERROR;
    } else {
        return SOFT_COPYING_ERROR;
    }
}

int copy_data(int fd_from, int fd_to) {
    int success = SUCCESS;
    bool file_changed = false;
    int block_size = 100;
    char block[block_size];
    ftruncate(fd_to, 0);
    lseek(fd_from, 0, SEEK_SET);
    lseek(fd_to,0,SEEK_SET);
    long bytes_read = read(fd_from, block, block_size);
    while(bytes_read > 0) {
        long bytes_written = write(fd_to, block, bytes_read);
        if(bytes_written < 0) {
            success = copying_error(file_changed);
            break;
        }
        file_changed = true;
        bytes_read = read(fd_from, block, block_size);
    }
    if(bytes_read < 0) {
        success = copying_error(file_changed);
    }
    return success;
}

void def_on_exit() {
    remove(buffer_file.c_str());
}

void write_error(const string& str) {
    write(1, str.c_str(), str.size());
}

void abort_changes(int fd_first, int fd_second, int fd_buffer, int error_code) {
    if(error_code == SECOND_FIRST_ERROR) {
        copy_data(fd_buffer, fd_first);
    } else if(error_code == BUFFER_SECOND_ERROR) {
        copy_data(fd_first, fd_second);
        copy_data(fd_buffer, fd_first);
    }
}

void on_exit(int fd_first, int fd_second, int fd_buffer, int error_code, bool is_soft_error) {
    if(error_code != SUCCESS) {
        string error_message = "Error on copying data";
        if(error_code == FIRST_BUFFER_ERROR) {
            error_message += " from first to buffer";
        } else if(error_code == SECOND_FIRST_ERROR) {
            error_message += " from second to first";
            if(!is_soft_error) {
                abort_changes(fd_first, fd_second, fd_buffer, error_code);
            }
        } else if(error_code == BUFFER_SECOND_ERROR) {
            error_message += " from buffer to second";
            if(!is_soft_error) {
                abort_changes(fd_first, fd_second, fd_buffer, error_code);
            }
        }
        write_error(error_message);
    }
    def_on_exit();
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        write_error("Invalid args");
        return 1;
    }
    int fd_first = open(argv[1], O_RDWR);
    int fd_second = open(argv[2], O_RDWR);
    if(fd_first < 0 || fd_second < 0) {
        int error_file_idx;
        if(fd_first < 0) {
            error_file_idx = 1;
        } else {
            error_file_idx = 2;
        }
        write_error("Error on opening file "+string(argv[error_file_idx]));
        return 1;
    }
    int fd_buffer = open((buffer_file).c_str(), O_CREAT | O_RDWR);
    if(fd_buffer < 0) {
        write_error("Error on creating buffer");
        return 1;
    }
    int success = copy_data(fd_first, fd_buffer);
    if(success != SUCCESS) {
        on_exit(fd_first, fd_second, fd_buffer, FIRST_BUFFER_ERROR, success == SOFT_COPYING_ERROR);
        return 1;
    }
    success = copy_data(fd_second, fd_first);
    if(success != SUCCESS) {
        on_exit(fd_first, fd_second, fd_buffer, SECOND_FIRST_ERROR, success == SOFT_COPYING_ERROR);
        return 1;
    }
    success = copy_data(fd_buffer, fd_second);
    if(success != SUCCESS) {
        on_exit(fd_first, fd_second, fd_buffer, BUFFER_SECOND_ERROR, success == SOFT_COPYING_ERROR);
        return 1;
    }
    on_exit(fd_first, fd_second, fd_buffer, SUCCESS, true);
    return 0;
}