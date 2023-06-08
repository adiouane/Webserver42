#include "../response.hpp"
#include <unistd.h>
#include <signal.h>

void send_http_response(Client *c, std::string& filepath) {
    // Open the file
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error: could not open file " << filepath << std::endl;
        return;
    }

    // Read the file into a string
    std::string body;
    std::string line;
    while (std::getline(file, line)) {
        body += line + "\n";
    }
    size_t pos = body.find("<!DOCTYPE html>");
    if (pos != std::string::npos) {
        body = body.substr(pos);
    }
    std::string headers;
    headers += "Content-Length: " + std::to_string(body.length()) + "\r\n";
    headers += "Content-Type: text/html\r\n";
    headers += "\r\n";

    std::string response = "HTTP/1.1 200 OK\r\n" + headers + "\r\n" + body;

    send(c->get_socket_fd(), response.c_str(), response.length(), 0);
}

void handle_get(Client *c, char *args[3])
{
    std::string cookie = c->req.get_headers()["Cookie"];
    char *env[] = {strdup("PATH_INFO=root/cgi-bin/cookie.php"), strdup(("HTTP_COOKIE=" + cookie).c_str()), NULL};
    int fd = open("cgi-env/output.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int outpt = dup(1);

    dup2(fd, 1);
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execve(args[0], args, env) == -1)
        {
            for (int i = 1; i < 3; i++){
                free(args[i]);
                free(env[i]);
            }
            perror("execve");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
        // This is the parent process
        int status;

        if (waitpid(pid, &status, WNOHANG) == 0)
        {
            // Child process is still running
            sleep(1);

            if (waitpid(pid, &status, WNOHANG) == 0)
            {
                // Child process did not complete within the timeout
                kill(pid, SIGKILL);
                lseek(fd, 0, SEEK_END);
                const char *timeoutMessage = "This request took too long time.\n";
                if (write(fd, timeoutMessage, strlen(timeoutMessage)) < 1)
                    c->is_finished = true;
            }
        }
        dup2(outpt, 1);
        c->_fileName = "cgi-env/output.txt";
        send_http_response(c, c->_fileName);
        for (size_t i = 0; i < 2; i++){
            free(env[i]);
            free(args[i]);
        }
        c->is_finished = true;
        return;
    }
    else
    {
        for (int i = 1; i < 3; i++){
            free(args[i]);
            free(env[i]);
        }
        perror("fork");
    }
}

int Response::Handle_cgi_response(Client *c, std::string url)
{
    Response r;

    char *args[3];
    char *env[] = {NULL};
    
    if (r.handle_url(c, url) == 0)
        return 0;
    
    std::string script_name = url.substr(url.find_last_of('/') + 1);
    std::string ex = r.getContentType(url);
    std::string path = config->servers[c->req.server_index].locations[c->req.location_index].root + "/" + script_name;
    if (ex == "python")
    {
        args[0] = strdup("/usr/bin/python");
        args[1] = strdup(path.c_str());
        args[2] = NULL;
    }
    else if (ex == "php")
    {
        args[0] = strdup("cgi-env/php-cgi");
        args[1] = strdup(path.c_str());
        args[2] = NULL;
    }
    else
    {
        if (ex != "python" && ex != "php")
        {
            for (int i = 0; i < 2; i++){
                free(args[i]);
                free(env[i]);
            }
            return 0;
        }
    }
    if (c->req.get_method() == "POST")
    {
        std::string str = config->servers[c->req.server_index].locations[c->req.location_index].upload_path + "/" + script_name;
        int fd2 = open(str.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        if (fd2 == -1)
        {
            std::cout << "error in open" << std::endl;
            for (int i = 0; i < 2; i++){
                free(args[i]);
            }
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // This is the child process
            dup2(fd2, 1);
            if (execve(args[0], args, env) == -1)
            {
                std::cout << "error in execve" << std::endl;
                for (int i = 0; i < 2; i++){
                    free(args[i]);
                }
                exit(EXIT_FAILURE);
            }
        }
        else if (pid > 0)
        {
            // This is the parent process
            sleep(5);
            int status;
            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == 0)
            {
                // Child process is still running
                kill(pid, SIGKILL);
            }
            close(fd2);
        }
        else
        {

            std::cout << "error in fork" << std::endl;
            for (size_t i = 0; i < 2; i++){
                free(args[i]);
            }
            exit(EXIT_FAILURE);
        }

        c->_fileName = str;
        r.sent_error(c->get_socket_fd(), 201);
        c->is_finished = true;
        for (size_t i = 0; i < 2; i++){
            free(args[i]);
        }
        return 1;
    }
    else
    {
        handle_get(c, args);
        return 1;
    }
}
