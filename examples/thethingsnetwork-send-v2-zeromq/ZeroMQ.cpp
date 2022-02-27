#include <lmic.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <iostream>
#include <future>
#include <thread>

#include "Credentials.h"

u1_t *currentData = nullptr;
std::thread thread;

zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REP);

bool recv(std::vector<zmq::message_t> &recv_msgs)
{
    auto result =
        zmq::recv_multipart(socket, std::back_inserter(recv_msgs));
    // assert(result && "recv failed");
    if (result && *result != 2)
    {
        std::cout << "Invalid input!" << std::endl;
        return false;
    }
    std::cout << "Topic: [" << recv_msgs[0].to_string() << "] "
              << recv_msgs[1].str() << std::endl;
    return true;
}

void ZeroMQThread()
{
    std::cout << "Thread running" << std::endl;
    while (true)
    {
        std::string msgText;
        std::vector<zmq::message_t> recv_msgs;

        if (!recv(recv_msgs))
            continue;

        auto topic = recv_msgs[0].to_string();

        if (topic == "COMMEAN_DATA")
        {
            auto data = recv_msgs[1].data<u1_t>();

            if (currentData == nullptr)
            {
                std::cout << "Receiving new data..." << std::endl;
                currentData = data;
                msgText = "ACK";
            }
            else
            {
                // TODO: Buffer
                std::cout << "Buffer full" << std::endl;
                msgText = "BUFFER_FULL";
            }
        }
        else
        {
            std::cout << "Wrong topic [" << topic << "]" << std::endl;

            msgText = "WRONG_TOPIC";
        }

        zmq::message_t msg(msgText.c_str(), msgText.length());
        socket.send(recv_msgs[0], zmq::send_flags::sndmore);
        socket.send(msg);
    }
}

void setupZeroMQ()
{
    auto address = "tcp://*:5555";
    socket.bind(address);
    std::cout << "ZeroMQ running on: " << address << std::endl;

    // APPEUI DEVEUI APPKEY
    bool setupDone[3] = {false, false, false};

    do
    {
        std::string msgText;
        std::vector<zmq::message_t> recv_msgs;
        printf("Waiting for credentials... (%s/%s/%s)\n", printBool(setupDone[0]), printBool(setupDone[1]), printBool(setupDone[2]));

        if (!recv(recv_msgs))
            continue;

        if (recv_msgs[0].to_string() == "APPEUI")
        {
            // std::cout << "Heureka!!!!!" << std::endl;
            os_setArtEui(recv_msgs[1].data<u1_t>());
            msgText = "ACK";
            setupDone[0] = true;
        }
        else if (recv_msgs[0].to_string() == "DEVEUI")
        {
            os_setDevEui(recv_msgs[1].data<u1_t>());
            msgText = "ACK";
            setupDone[1] = true;
        }
        else if (recv_msgs[0].to_string() == "APPKEY")
        {
            os_setDevKey(recv_msgs[1].data<u1_t>());
            msgText = "ACK";
            setupDone[2] = true;
        }
        else
        {
            msgText = "WRONG_TOPIC";
        }

        zmq::message_t msg(msgText.c_str(), msgText.length());
        socket.send(recv_msgs[0], zmq::send_flags::sndmore);
        socket.send(msg);

    } while (!(setupDone[0] && setupDone[1] && setupDone[2]));
    printf("Received all credentials!\n");
    // printCred(APPEUI);

    // Starting server thread
    thread = std::thread(ZeroMQThread);
}