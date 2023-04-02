#include <iostream>
#include <cstring>

#include "server_class.h"
#include "event_loop.h"

int main()
{
    EventLoop *main_loop = new EventLoop();
    Server *server = new Server(main_loop);
    main_loop->loop();

    return 0;
}
