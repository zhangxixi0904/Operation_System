#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

# define SHOWHOST

void showtime(long);
void show_info(struct utmp*);

void show_info(struct utmp * utbufp){

    if(utbufp->ut_type != USER_PROCESS)
        return;

    printf("%-8.8s", utbufp->ut_name);
    printf(" ");
    printf("%-8.8s", utbufp->ut_line);
    printf(" ");
    showtime(utbufp->ut_time);
    printf(" ");
    # ifdef SHOWHOST
        if( utbufp->ut_host[0] != '\0'){
            printf("(%s)", utbufp->ut_host);
        }
    # endif
        printf("\n");

}

void showtime(long timeval){
    
    // unix时间是用long型保存的时间戳，可以用ctime函数转换成UTC标准格式时间，方便阅读
    char* cp;
    cp = ctime(&timeval);
    // 从第4个字节开始读12个字节，前四个字节是表示星期，其实从0开始读也可以
    printf("%12.12s", cp+4);
}

int main(){

    struct utmp current_record;
    int utmpfd;
    int reclen = sizeof(current_record);
    
    if( (utmpfd = open(UTMP_FILE, O_RDONLY))==-1 ){
        perror(UTMP_FILE);
        exit(1);
    }

    while( read(utmpfd, &current_record, reclen)== reclen){
        show_info(&current_record);
    }

    close(utmpfd);
    return 0;

}
