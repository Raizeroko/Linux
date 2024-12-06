#include "Init.hpp"

int main()
{
    int fd = open(FIFO_FILE, O_WRONLY);
    if(fd < 0){
        perror("open");
        exit(FIFO_OPEN_ERR);
    }
    cout<<"Finished open clien" << endl;

    string input;
    while(true){
        cout << "Enter: ";
        getline(cin, input);
        write(fd, input.c_str(), input.size());
    }


    close(fd);

    return 0;
}