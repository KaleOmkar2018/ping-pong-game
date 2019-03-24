#include "tasks.hpp"
#include "examples/examples.hpp"
#include<FreeRTOS.h>
#include<queue.h>
#include<event_groups.h>
#include<string>
#include<sstream>
#include<storage.hpp>
#include<io.hpp>
#include <printf_lib.h>
#include<stdio.h>
#include<time.h>
#include "L5_Application/RGBLed_Matrix.hpp"
#include<math.h>
#include "utilities.h"
#include "io.hpp"
#include "wireless/wireless.h"
#include "display_node.hpp"

using namespace std;

#define PLAYER_1 100
#define MASTER 101
#define PLAYER_2 102

RGBmatrixPanel matrix;

volatile uint8_t accValue_Player1;
volatile uint8_t accValue_Player2;

class updateDisplayTask: public scheduler_task
{
public :
    updateDisplayTask(uint8_t priority): scheduler_task("UpdateLED", 2048, priority){
        matrix.begin();
    }
    bool run(void *p)
    {
        matrix.updateDisplay();
        vTaskDelay(1);
        return true;
    }

};
const uint8_t dataArray[][2] =
{
        {15, 13}, {14, 12}, {13, 11}, {12, 10}, {11, 9}, {10, 8}, {9, 7},
        {8, 6}, {7, 5}, {6, 4}, {5, 3}, {4, 2}, {3, 1}, {2, 0}
};

uint8_t findDir(int16_t x)
{
    uint8_t retIndex;
    switch(x)
    {
        case 850 ... 32767:
            retIndex = 0;
            break;
        case 700 ... 849:
            retIndex = 1;
            break;
        case 560 ... 699:
            retIndex = 2;
            break;
        case 420 ... 559:
            retIndex = 3;
            break;
        case 280 ... 419:
            retIndex = 4;
            break;
        case 140 ... 279:
            retIndex = 5;
            break;
        case 0 ... 139:
            retIndex = 6;
            break;
        case -140 ... -1:
            retIndex = 7;
            break;
        case -280 ... -141:
            retIndex = 8;
            break;
        case -420 ... -281:
            retIndex = 9;
            break;
        case -560 ... -421:
            retIndex = 10;
            break;
        case -700 ... -561:
            retIndex = 11;
            break;
        case -849 ... -701:
            retIndex = 12;
            break;
        case -32768 ... -850:
            retIndex = 13;
           break;
        default:
            retIndex = 13;
            break;
    }
    return retIndex;
}

volatile uint8_t slider_Player1;
volatile uint8_t slider_Player2;

class remote_wireless: public scheduler_task
{
    mesh_packet_t rcvPkt;
public:
    remote_wireless(uint8_t priority) :
        scheduler_task("SampleWireless", 2048, priority)
{
}

    bool run(void *p)
    {
        static uint8_t rcv_pckt_integer = 0;
        const char max_hops = 2;

        int timeout_ms = 900;
        while (wireless_get_rx_pkt(&rcvPkt, timeout_ms))
        {
            rcv_pckt_integer = (uint8_t)rcvPkt.data[0];

            if(PLAYER_1 == rcvPkt.nwk.src)
            {
                accValue_Player1 = rcv_pckt_integer;

               // printf("\n received1: %d", rcv_pckt_integer);

            }
            else if(PLAYER_2 == rcvPkt.nwk.src)
            {
                accValue_Player2 = rcv_pckt_integer;

               // printf("\n received2: %d", rcv_pckt_integer);

            }
        }
        return true;
    }
    private:
};

sliderTask *sliderobj;
BallTask *ballobj;
ScoreTask *scoreobj;
StartScreen *startobj;

int main(void)
{
    sliderobj = new sliderTask(PRIORITY_LOW);
    ballobj = new BallTask(PRIORITY_LOW);
    scoreobj = new ScoreTask(PRIORITY_LOW);
    startobj = new StartScreen(PRIORITY_LOW);

    scheduler_add_task(startobj);
    scheduler_add_task(new updateDisplayTask(PRIORITY_MEDIUM));
    scheduler_add_task(sliderobj);


    scheduler_add_task(new wirelessTask(PRIORITY_HIGH));
    scheduler_add_task(new remote_wireless(PRIORITY_MEDIUM));
    scheduler_add_task(ballobj);

    scheduler_add_task(scoreobj);

    scheduler_start();
    return -1;
}
