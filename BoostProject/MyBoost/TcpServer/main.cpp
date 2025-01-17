#include "BoostServer.h"

#define DEFAULT_SERVER_IP   "127.0.0.1"
#define DEFAULT_SERVER_PORT1  8888
#define DEFAULT_SERVER_PORT2  9999


int main(int argc, char** argv)
{
    boost::asio::io_context ctx;
    BoostServer server(ctx, DEFAULT_SERVER_PORT1, DEFAULT_SERVER_PORT2, DEFAULT_SERVER_IP, DEFAULT_SERVER_IP);

    ctx.run();


    return 0;
}

