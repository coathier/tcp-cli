#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <winsock2.h>
#include <curses.h>

void bzero(void *s, size_t n);

#define PORT 4639
#define BUF_SIZE 256
const int size = BUF_SIZE * sizeof(char);

int bind_socket(char *ip) {
	struct sockaddr_in servaddr;
	int client_socket;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) return -1;
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(PORT);

	if (connect(client_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) return -1;
	return client_socket;
}

void display_output(WINDOW *window, const char *str) {
    static int output_y = 1;
    int output_x = 1;

    char input;
    int input_index = 0;

    do {
        input = str[input_index];

        if (output_x >= window->_maxx) {
            output_x = 1;
            output_y++;
        }

        if (output_y >= window->_maxy) {
            wclear(window);
            box(window, 0, 0);
            output_y = 1;
            output_x = 1;
        }

        if (input == '\0') {
            if (input_index > 0 && str[input_index - 1] != '\n')
                output_y++;
        }

        else if (input == '\n') {
            output_y++;
            output_x = 1;
        }

        else {
            mvwaddch(window, output_y, output_x, input);
            output_x++;
        }

        input_index++;
    } while (input != '\0');

    wrefresh(window);
}

void read_input(char *input_buffer, bool *command_ready, bool *input_ready) {
    static int input_index = 0;
    int input;

    input = getch();

    if (input == ERR) {
        return;
    }

    else if (input == '\n') {
        input_index = 0;
        *command_ready = true;
    }

    else if (input == KEY_BACKSPACE) {
        if (input_index == 0) return;
        input_index--;
        memset(input_buffer + input_index, 0, size - input_index * sizeof(char));
        *input_ready = true;
    }

    else {
        input_buffer[input_index] = input;
        *input_ready = true;
        input_index++;
    }
}

int main(int argc, char *argv[]) {
    int running = true;
    char input_buffer[BUF_SIZE] = {0};
    bool input_ready = false;
    bool command_ready = false;

    if (argc < 2) {
        printf("Usage: %s <ip>", argv[0]);
        return 0;
    }

    int client_socket = bind_socket(argv[1]);
    if (client_socket < 0) {
        printf("Couldn't bind the socket");
        return 1;
    }

    struct pollfd socket = (struct pollfd) {
        .fd = client_socket,
        .events = POLLIN
    };

    WINDOW *win, *input_win, *output_win;
    win = initscr();
    noecho();
    cbreak();
    nodelay(win, true);
    notimeout(win, true);
    keypad(win, true);
    refresh();

    output_win = newwin(win->_maxy - 3, win->_maxx, 0, 0);
    box(output_win, 0, 0);
    wrefresh(output_win);

    input_win = newwin(3, win->_maxx, win->_maxy - 3, 0);
    box(input_win, 0, 0);
    wmove(input_win, 1, 1);
    wrefresh(input_win);

    char socket_buffer[BUF_SIZE];
    while (running) {

        WSAPoll(&socket, 1, 1);
        if (socket.revents & (POLLRDNORM | POLLRDBAND)) {
            bzero(socket_buffer, size);
            int bytes = read(client_socket, socket_buffer, size);
            if (bytes == 0) continue;
            display_output(output_win, socket_buffer);
            wmove(input_win, 1, 1);
            wrefresh(input_win);
        }

        if (command_ready) {
            if (strncmp(input_buffer, "exit", 4) == 0) running = false;
            display_output(output_win, input_buffer);

            write(client_socket, input_buffer, strnlen(input_buffer, BUF_SIZE) * sizeof(char));
            bzero(input_buffer, size);
            command_ready = false;

            wclear(input_win);
            box(input_win, 0, 0);
            wmove(input_win, 1, 1);
            wrefresh(input_win);
        }

        if (input_ready) {
            wclear(input_win);
            box(input_win, 0, 0);
            mvwaddstr(input_win, 1, 1, input_buffer);
            wrefresh(input_win);
            input_ready = false;
        }

        read_input(input_buffer, &command_ready, &input_ready);
    }

    endwin();
    close(client_socket);

    return 0;
}
