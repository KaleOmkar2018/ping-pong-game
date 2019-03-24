/*
 * display_node.cpp
 *
 *  Created on: May 9, 2018
 *      Author: Akil Khan
 */

#include<FreeRTOS.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include "scheduler_task.hpp"
#include "display_node.hpp"
#include "L5_Application/RGBLed_Matrix.hpp"

extern RGBmatrixPanel matrix;
uint8_t ball_x1 = 5, ball_y1 = 5;
movement_towards ball_movement = down_right;

extern uint8_t slider_Player1;
extern uint8_t slider_Player2;
extern sliderTask *sliderobj;
extern BallTask *ballobj;
extern ScoreTask *scoreobj;
extern StartScreen *startobj;
extern volatile uint8_t accValue_Player1;
extern volatile uint8_t accValue_Player2;
volatile uint8_t player1_score;
volatile uint8_t player2_score;

const uint8_t dataArray[][2] =
{
        {15, 13}, {14, 12}, {13, 11}, {12, 10}, {11, 9}, {10, 8}, {9, 7},
        {8, 6}, {7, 5}, {6, 4}, {5, 3}, {4, 2}, {3, 1}, {2, 0}
};

sliderTask::sliderTask(uint8_t priority): scheduler_task("SliderTask", 2048, priority)
{

}

bool sliderTask::run(void *p)
{
    slider_Player1 = dataArray[accValue_Player1][1];
    slider_Player2 = dataArray[accValue_Player2][1];

    matrix.drawLine(0, dataArray[accValue_Player1][0], 0,dataArray[accValue_Player1][1] , matrix.Color333(8, 0, 0));
    matrix.drawLine(31, dataArray[accValue_Player2][0], 31,dataArray[accValue_Player2][1] , matrix.Color333(0, 0, 8));

    vTaskDelay(100);
    matrix.fill_slider(matrix.Color333(0,0,0));
    return true;
}

ScoreTask::ScoreTask(uint8_t priority): scheduler_task("SliderTask", 2048, priority)
{


}

bool new_game = false;

bool ScoreTask::run(void *p)
{
    // Display the score
    matrix.fill(matrix.Color333(0, 0, 0));
    matrix.drawChar(6, 4, player1_score + '0',matrix.Color333(8, 8, 8), 1);
    matrix.drawChar(20, 4, player2_score + '0' ,matrix.Color333(8, 8, 8), 1);
    matrix.drawLine(14, 7, 16, 7, matrix.Color333(8, 8, 8));

    vTaskDelay(1000);
    matrix.fill(matrix.Color333(0, 0, 0));
    vTaskDelay(100);

    matrix.drawChar(6, 4, player1_score + '0',matrix.Color333(8, 8, 8), 1);
    matrix.drawChar(20, 4, player2_score + '0' ,matrix.Color333(8, 8, 8), 1);
    matrix.drawLine(14, 7, 16, 7, matrix.Color333(8, 8, 8));

    vTaskDelay(1000);
    matrix.fill(matrix.Color333(0, 0, 0));
    vTaskDelay(100);
    matrix.drawChar(6, 4, player1_score + '0',matrix.Color333(8, 8, 8), 1);
    matrix.drawChar(20, 4, player2_score + '0' ,matrix.Color333(8, 8, 8), 1);
    matrix.drawLine(14, 7, 16, 7, matrix.Color333(8, 8, 8));

    vTaskDelay(1000);
    matrix.fill(matrix.Color333(0, 0, 0));
    vTaskDelay(100);

    // Reset the score
    if(player1_score == 9 || player2_score == 9)
    {
        if(player1_score == 9)
        {
            matrix.drawChar(1, 0, 'P', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(6, 0, 'L', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(10, 0, 'A', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(15, 0, 'Y', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(20, 0, 'E', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(25, 0, 'R', matrix.Color333(8, 0, 0), 1);

            matrix.drawChar(2, 8, '1', matrix.Color333(0, 8, 0), 1);
            matrix.drawChar(10, 8, 'W', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(15, 8, 'I', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(20, 8, 'N', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(25, 8, 'S', matrix.Color333(8, 0, 0), 1);
        }
        else
        {
            matrix.drawChar(1, 0, 'P', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(6, 0, 'L', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(10, 0, 'A', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(15, 0, 'Y', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(20, 0, 'E', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(25, 0, 'R', matrix.Color333(8, 0, 0), 1);

            matrix.drawChar(2, 8, '2', matrix.Color333(0, 8, 0), 1);
            matrix.drawChar(10, 8, 'W', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(15, 8, 'I', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(20, 8, 'N', matrix.Color333(8, 0, 0), 1);
            matrix.drawChar(25, 8, 'S', matrix.Color333(8, 0, 0), 1);
        }
        vTaskDelay(5000);
        matrix.fill(matrix.Color333(0,0,0));

        player1_score = 0;
        player2_score = 0;
        new_game = true;
    }

    if(new_game)
    {
        new_game = false;
        startobj->resume();
        ballobj->reset_ball();
        this->suspend();
    }
    else
    {
        ballobj->reset_ball();
        ballobj->resume();
        sliderobj->resume();
        this->suspend();
    }

    return true;
}


BallTask::BallTask(uint8_t priority): scheduler_task("BallTask", 2048, priority)
{
    reset_ball();
}

void BallTask::reset_ball()
{
    srand(time(NULL));
    ball_movement = (movement_towards) (rand() % 4);
    ball_x1 = 15;
    ball_y1 = 7;
}

bool BallTask::run(void *p)
{
    if(ball_y1 == 0)  // ball hit the bottom, need to reflect back according to direction of ball
    {
        if(ball_movement == down_right)
        {
            if(ball_x1 == 0 && slider_Player1 != 0)        // corner ( 0, 0)
            {
                printf("Player 1 loses\n");
                player2_score++;
                scoreobj->resume();
                sliderobj->suspend();
                ballobj->suspend();
            }
            else if(ball_x1 == 0)
            {
                ball_movement = top_left;
                ball_x1 = 1;
                ball_y1 = 1;
            }
            else
            {
                ball_movement = top_right;
                ball_y1 = 1;
                ball_x1--;
            }
        }
        else if(ball_movement == down_left)
        {
            if(ball_x1 == 31 && slider_Player2 != 13)       // corner (31, 0)
            {
                printf("Player 2 loses \n");
                player1_score++;
                scoreobj->resume();
                sliderobj->suspend();
                ballobj->suspend();
            }
            else if(ball_x1 == 31)
            {
                ball_movement = top_right;
                ball_x1 = 30;
                ball_y1 = 1;
            }
            else
            {
                ball_movement = top_left;
                ball_y1 = 1;
                ball_x1++;
            }
        }
    }
    else if(ball_x1 == 0)
    {
        if(ball_movement == top_right)
        {
            if(ball_y1 == 15 && slider_Player1 != 13)    // top right corner (0,15)
            {
                printf("Player 1 loses \n");
                player2_score++;
                scoreobj->resume();
                sliderobj->suspend();
                ballobj->suspend();
                //this->suspend();
            }
            else if(ball_y1 == 15)
            {
                ball_movement = down_left;
                ball_x1 = 1;
                ball_y1 = 14;
            }
            else if(ball_y1 != 15)
            {
                if(ball_y1 == slider_Player1 || ball_y1 == slider_Player1 + 1 || ball_y1 == slider_Player1 + 2)
                {
                    ball_movement = top_left;
                    ball_x1 = 1;
                    ball_y1++;
                }
                else
                {
                    printf("Player 1 loses\n");
                    player2_score++;
                    scoreobj->resume();
                    sliderobj->suspend();
                    ballobj->suspend();
                    //this->suspend();
                }
            }
        }
        else if(ball_movement == down_right)
        {
            if(ball_y1 == slider_Player1 || ball_y1 == slider_Player1 + 1 || ball_y1 == slider_Player1 + 2)
            {
                ball_movement = down_left;
                ball_x1 = 1;
                ball_y1--;
            }
            else
            {
                printf("Player 1 loses \n");
                player2_score++;
                scoreobj->resume();
                sliderobj->suspend();
                ballobj->suspend();
                //this->suspend();
            }
        }
    }
    else if(ball_y1 == 15)
    {
        if(ball_movement == top_left)
        {
            if(ball_x1 == 31 && slider_Player2 != 13)   // top left corner (31,15)
            {
                printf("Player 2 loses \n");
                player1_score++;
                scoreobj->resume();
                sliderobj->suspend();
                ballobj->suspend();
            }
            else if(ball_x1 == 31)
            {
                ball_movement = down_right;
                ball_x1 = 30;
                ball_y1--;
            }
            else
            {
                ball_movement = down_left;
                ball_x1++;
                ball_y1 = 14;
            }
        }
        else if(ball_movement == top_right)
        {
            ball_movement = down_right;
            ball_x1--;
            ball_y1 = 14;
        }
    }
    else if(ball_x1 == 31)
    {
        if(ball_movement == top_left)
        {
            if(ball_y1 == slider_Player2 || ball_y1 == slider_Player2 + 1 || ball_y1 == slider_Player2 + 2)
            {
                ball_movement = top_right;
                ball_x1 = 30;
                ball_y1++;
            }
            else
            {
                printf("Player 2 loses \n");
                player1_score++;
                scoreobj->resume();
                sliderobj->suspend();
                ballobj->suspend();
            }
        }
        else if(ball_movement == down_left)
        {
            if(ball_y1 == slider_Player2 || ball_y1 == slider_Player2 + 1 || ball_y1 == slider_Player2 + 2)
            {
                ball_movement = down_right;
                ball_x1 = 30;
                ball_y1--;
            }
            else
            {
                printf("Player 2 loses \n");
                player1_score++;
                scoreobj->resume();
                sliderobj->suspend();
                ballobj->suspend();
            }
        }
    }
    else
    {
        if(ball_movement == top_right)
        {
            ball_x1--;
            ball_y1++;
        }
        else if(ball_movement == top_left)
        {
            ball_x1++;
            ball_y1++;
        }
        else if(ball_movement == down_right)
        {
            ball_x1--;
            ball_y1--;
        }
        else if(ball_movement == down_left)
        {
            ball_x1++;
            ball_y1--;
        }
    }
#if DEBUG
    printf("Ball x: %d y : %d \n", ball_x1, ball_y1);
#endif

    matrix.fill_ball(matrix.Color333(0, 0, 0));      // Clear screen
    matrix.drawPixel(ball_x1, ball_y1, matrix.Color333(0, 8, 0));
    vTaskDelay(75);
    return true;
}

bool player1_ready = false, player2_ready = false;

bool StartScreen::run(void *p)
{
    sliderobj->suspend();
    ballobj->suspend();
    scoreobj->suspend();

    matrix.fill_slider(matrix.Color333(0,0,0));

    matrix.drawChar(1, 0, 'P', matrix.Color333(8, 0, 0), 1);
    matrix.drawChar(5, 0, 'I', matrix.Color333(8, 0, 0), 1);
    matrix.drawChar(10, 0, 'N', matrix.Color333(8, 0, 0), 1);
    matrix.drawChar(15, 0, 'G', matrix.Color333(8, 0, 0), 1);

    matrix.drawChar(11, 9, 'P', matrix.Color333(0, 0, 8), 1);
    matrix.drawChar(16, 9, 'O', matrix.Color333(0, 0, 8), 1);
    matrix.drawChar(21, 9, 'N', matrix.Color333(0, 0, 8), 1);
    matrix.drawChar(26, 9, 'G', matrix.Color333(0, 0, 8), 1);

    matrix.drawChar(22, 0, '1',  matrix.Color333(0, 8, 0), 1);
    matrix.drawChar(1, 9, '2',  matrix.Color333(0, 8, 0), 1);
    matrix.drawRect(26, 2, 3, 3, matrix.Color333(3,3,3));
    matrix.drawRect(7, 10, 3, 3, matrix.Color333(3,3,3));

    if(accValue_Player1 == 20 && !player1_ready)
    {
        matrix.drawPixel(27, 3, matrix.Color333(0, 8, 0));  // player 1 ready
        player1_ready = true;
    }
    else if(!player1_ready)
    {
        matrix.drawPixel(27, 3, matrix.Color333(8, 0, 0));
    }

    if(accValue_Player2 == 25 && !player2_ready)
    {
        matrix.drawPixel(8, 11, matrix.Color333(0, 8, 0));  // player 2 ready
        player2_ready = true;
    }
    else if(!player2_ready)
    {
        matrix.drawPixel(8, 11, matrix.Color333(8, 0, 0));
    }

    if(player1_ready && player2_ready)
    {
        player1_ready = false;
        player2_ready = false;
        vTaskDelay(2000);
        matrix.fill(matrix.Color333(0,0,0)); // clear screen

        matrix.drawChar(15, 7, '3', matrix.Color333(8, 8, 8), 1);
        vTaskDelay(1000);

        matrix.fill(matrix.Color333(0,0,0)); // clear screen
        matrix.drawChar(15, 7, '2', matrix.Color333(8, 8, 8), 1);
        vTaskDelay(1000);

        matrix.fill(matrix.Color333(0,0,0)); // clear screen
        matrix.drawChar(15, 7, '1', matrix.Color333(8, 8, 8), 1);
        vTaskDelay(1000);
        matrix.fill(matrix.Color333(0,0,0)); // clear screen

        sliderobj->resume();
        ballobj->resume();
        this->suspend();
    }
    vTaskDelay(100);
    return true;
}
