#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define PERMS 0644           // Quyền truy cập cho hàng đợi thông điệp
#define Send_KEY 0x124       // Khóa cho hàng đợi gửi
#define Read_KEY 0x123       // Khóa cho hàng đợi nhận

// Định nghĩa cấu trúc cho thông điệp gửi
struct my_send {
   long mtype;               
   char mtext[200];          
};

// Định nghĩa cấu trúc cho thông điệp nhận
struct my_read {
   long mtype;               
   char mtext[200];         
};

// Hàm gửi thông điệp
void* Send(void* arg) {
    struct my_send send;
    int msqid;             
    int len;               
    key_t key;

    // Tạo file rỗng để kiểm tra
    system("touch Send2.txt");

    // Tạo hoặc truy cập hàng đợi thông điệp với khóa `Send_KEY`
    if ((msqid = msgget(Send_KEY, PERMS | IPC_CREAT)) == -1) {
        perror("msgget");    
        exit(1);
    }
    send.mtype = 1;         

    // Vòng lặp đọc dữ liệu từ stdin và gửi vào hàng đợi
    while (fgets(send.mtext, sizeof send.mtext, stdin) != NULL) {
        len = strlen(send.mtext);  

        // Nếu có ký tự xuống dòng, xóa nó
        if (send.mtext[len - 1] == '\n') 
            send.mtext[len - 1] = '\0';

        // Gửi thông điệp vào hàng đợi
        if (msgsnd(msqid, &send, len + 1, 0) == -1) 
            perror("msgsnd");   
    }

    // Gửi thông điệp "end" để báo hiệu kết thúc
    strcpy(send.mtext, "end");
    len = strlen(send.mtext);
    if (msgsnd(msqid, &send, len + 1, 0) == -1) 
        perror("msgsnd");

    // Xóa hàng đợi thông điệp sau khi sử dụng
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");    
        exit(1);
    }
    return NULL;             
}

// Hàm nhận thông điệp
void* Read(void* arg) {
    struct my_read read;
    int msqid;               // ID của hàng đợi thông điệp
    int toend;               // Biến kiểm tra điều kiện kết thúc
    key_t key;

    // Tạo hoặc truy cập hàng đợi thông điệp với khóa `Read_KEY`
    if ((msqid = msgget(Read_KEY, PERMS | IPC_CREAT)) == -1) {
        perror("msgget");    
        exit(1);
    }

    // Vòng lặp nhận thông điệp từ hàng đợi
    while (1) {
        // Nhận thông điệp từ hàng đợi
        if (msgrcv(msqid, &read, sizeof(read.mtext), 0, 0) == -1) {
            perror("msgrcv");   
            exit(1);
        }

        // In nội dung thông điệp ra màn hình
        printf("Receive from MS1: \"%s\"\n", read.mtext);

        // Kiểm tra nếu thông điệp là "end"
        toend = strcmp(read.mtext, "end");
        if (toend == 0)
            break;          
    }

    // Xóa file kiểm tra
    system("rm Send1.txt");
    return NULL;             
}

int main() {
    pthread_t send, read;    

    pthread_create(&send, NULL, Send, NULL);

    pthread_create(&read, NULL, Read, NULL);

    pthread_join(read, NULL);
    pthread_join(send, NULL);

    return 0;               
}
