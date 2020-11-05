/*
Copyright 2019 BAE Systems

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
This is an intentionally vulnerable application that implements behaviors of a
Variable Message Sign (https://en.wikipedia.org/wiki/Variable-message_sign).

Intentional vulnerabilities include an authentication bypass and a buffer
overflow that can lead to execution of arbitrary code via a network attack
vector.

This application is provided to support education and training in the field of
Internet of Things (IoT) security.

Author: Troy Shurtleff

Approved for public release; unlimited distribution
Not export controlled per ES-FL-091619-0203
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

// This is key used for XOR encryption of the default password.
// The value is chosen to be outside the range of 7-bit ASCII characters.
// Applying this key a 7-bit ASCII character will yield a result that is
// outside the range of the 7-bit ASCII characters.
#define KEY			0xAA

#define PASS_LEN 		10
#define PASS_INPUT_BUFFER_LEN	64
#define MOTD_BUFFER_LEN		128

#define LISTEN_TCP_PORT		5555

char ciphertext_password[PASS_LEN] = {'i' ^ KEY,
                                      'n' ^ KEY,
                                      'c' ^ KEY,
                                      'o' ^ KEY,
                                      'r' ^ KEY,
                                      'r' ^ KEY,
                                      'e' ^ KEY,
                                      'c' ^ KEY,
                                      't' ^ KEY,
                                      '\0' ^ KEY};

char message_of_the_day[MOTD_BUFFER_LEN] = "Changing lanes? Use yah blinkah.";

int check_password(int socketfd) {
    char plaintext_password[PASS_LEN];
    char input_buffer[PASS_INPUT_BUFFER_LEN];
    int password_correct = 0;

    memset(&input_buffer, 0, sizeof(input_buffer)); 

    // Prompt for the password
    write(socketfd, "password: ", 11);

    if (read(socketfd, input_buffer, 256) == 0) {
        printf("signApp: Client disconnected.\n");
        return -1;
    }

    // Check the last two characters to see if they are either 
    // 0xA (Line Feed) or 0xD (Carriage Return).
    // If they are, change them to NULL
    for (unsigned int i = 0; i <= 1; i++) {
	if (input_buffer[strlen(input_buffer)-1] == 0x0A ||
            input_buffer[strlen(input_buffer)-1] == 0x0D) {
            input_buffer[strlen(input_buffer)-1] = 0x0;
        }
    }

    // Decrypt the correct password
    for (unsigned int i = 0; i < PASS_LEN; i++) {
        plaintext_password[i] = ciphertext_password[i] ^ KEY;
    }

    // Check to see if the provided password is correct
    if(strcmp(input_buffer, plaintext_password) == 0) {
        password_correct = 1;
    }

    return password_correct;    
}

void change_message(int socketfd) {
    char * message;

    memset(&message_of_the_day, 0, sizeof(message_of_the_day)); 

    // Prompt for the new message of the day
    message = "Enter the new message of the day: ";
    write(socketfd, message, strlen(message));

    if (read(socketfd, message_of_the_day, MOTD_BUFFER_LEN) == 0) {
        printf("signApp: Client disconnected.\n");
        return;
    }

    // Check the last two characters to see if they are either 
    // 0xA (Line Feed) or 0xD (Carriage Return).
    // If they are, change them to NULL
    for (unsigned int i = 0; i <= 1; i++) {
	if (message_of_the_day[strlen(message_of_the_day)-1] == 0x0A ||
            message_of_the_day[strlen(message_of_the_day)-1] == 0x0D) {
            message_of_the_day[strlen(message_of_the_day)-1] = 0x0;
        }
    }

    // Echo the new message of the day
    message = "\nThe message of the day is now: ";
    write(socketfd, message, strlen(message));
    write(socketfd, message_of_the_day, strlen(message_of_the_day));
    message = "\n\n";
    write(socketfd, message, strlen(message));

    return;
}

void * connection_handler(void * socket_fd) {
    int conn_fd = *(int*)socket_fd;   
    char * message;
    int result = 0;

    char plaintext_password[PASS_LEN];
    char input_buffer[10];
    char password_correct = 0;

    // Prompt for the password
    message = "The message of the day is: ";
    write(conn_fd, message, strlen(message));
    write(conn_fd, message_of_the_day, strlen(message_of_the_day));
    message = "\n\nIf you enter the correct password you can change the message of the day.\n\n";
    write(conn_fd, message, strlen(message));

    while(result == 0) {
        result = check_password(conn_fd);

        switch (result) {
            case 0:
                message = "\nYour password is incorrect.\n\n";
                write(conn_fd, message, strlen(message));
                break;
            case 1:
                message = "\nYou entered the correct password!\n\n";
                write(conn_fd, message, strlen(message));

                // Now let the client change the message
                change_message(conn_fd);

                break;
        }
    }

    close(conn_fd);
    free(socket_fd);

    return 0;
}

int main()
{
    int sfd, cfd, *new_fd;
    int read_size = 0;
    struct sockaddr_in servaddr;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        printf("signApp: Socket creation failure.\n");
        exit(0);
    }
    else
        printf("signApp: Socket creation success.\n");

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(LISTEN_TCP_PORT);

    if (bind(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        printf("signApp: Socket bind failure.\n");
        exit(0);
    }

    if (listen(sfd, 10) == -1) {
        printf("signApp: Socket listen failure.\n");
        exit(0);
    }

    while (cfd = accept(sfd, NULL, NULL)) {
        if (cfd == -1) {
            printf("signApp: Socket accept failed.\n");
            exit(0);
        }
 
        printf("signApp: Socket conneciton accepted successfully.\n");

        pthread_t connection_thread;
        new_fd = malloc(1);
        *new_fd = cfd;

        if (pthread_create(&connection_thread, NULL, connection_handler, (void *) new_fd) < 0) {
            printf("signApp: Could not create thread.\n");
        }

        printf("signApp: Handler assigned.\n");
    }

    return 0;
}
