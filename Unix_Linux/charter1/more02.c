#include <stdio.h>
#include <stdlib.h>
#define PAGELEN 24
#define LINELEN 512

void do_more(FILE * );
int see_more(FILE * );

// ac和av，都是在命令行写好参数后，ac会被自动赋值的av的长度
// ac表示参数的个数，即后面字符串数组char* av[]的元素个数，它不用用户自己算好了填
// 用户只需用填后面的字符串数组，ac会自动被赋值成用户填写的参数个数
int main(int ac, char* av[]){

    FILE* fp;
    // 只填写了一个参数，即只输入了一份文件的路径
    if(ac==1){
        do_more(stdin);
    }
    // 输入了多份文件的路径，则一个个依次打开
    else{
        while(--ac){
            if( (fp = fopen(*++av,"r"))!=NULL ){
                do_more(fp);
                fclose(fp);   
            }
            else{
                exit(1);
            }
        }
    }
    return 0;

}

void do_more(FILE *fp){
    char line[LINELEN];
    int num_of_lines = 0;
    // 下面这句挺无谓的，int see_more()只是再次声明函数，int reply则声明了一个int型变量
    // 但是为什么这个函数需要再次声明？实测并不需要。
    int see_more(FILE *), reply;
    // tty是键盘和显示器的设备描述文件，向这个文件写相当于显示在用户屏幕，读相当于从键盘获取用户输入
    // 用tty从键盘读用户输入，从标准输入中读取分页数据，这样cat /etc/passwd > ./more02才能正常运行
    FILE* fp_tty;
    fp_tty = fopen("/dev/tty","r");
    if( fp_tty == NULL)
        exit(1);
    while( fgets(line, LINELEN, fp)){
        // 一次性先打印PAGELEN=24行，用num_of_lines++
        // 每个while循环会打印一行
        // 如果已经打印够了24行，则调用see_more函数，由用户决定之后是要：
        // 0:退出，1:再打印多一行，24:翻页，再打印24行
        if( num_of_lines == PAGELEN ){
            reply = see_more(fp_tty);
            // 收到q，直接退出
            if( reply == 0 )
                break;
            // 这行代码很巧妙：如果收到空格键，要多打一行，reply赋值为1，则将num_of_lines-1变成23，
            // 下一次while循环会直接打多一行，而不会进入这个if区域，再下一次while循环num_of_lines又
            // 变成24，再次进入这个if函数，询问用户接下来的操作。
            // 如果收到回车键，多打24行，reply=24，num_of_lines直接变成0，在24次while循环内，都不会
            // 再进入这个if区域，而是一直打印够24行后，再进入这个if区域，询问用户下一步操作。
            num_of_lines -= reply;
        }
        if( fputs(line, stdout) == EOF)
            exit(1);
        num_of_lines++;
    }
}

// q键退出，'q'--->0:退出，
// 空格键再多打一行，' '--->1:再打印多一行，
// 回车键翻页，再多打24行，'\n'-->24:翻页，再打印24行
int see_more(FILE* cmd){
    int c;
    printf("\033[7m more? \033[m");
    while( (c=getc(cmd)) != EOF){
        if( c == 'q' )
            return 0;
        if( c == ' ')
            return PAGELEN;
        if( c == '\n')
            return 1;
    }
    return 0;
}