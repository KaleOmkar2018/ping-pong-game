/*
 * display_node.hpp
 *
 *  Created on: May 9, 2018
 *      Author: Akil Khan
 */

#ifndef DISPLAY_NODE_HPP_
#define DISPLAY_NODE_HPP_


typedef enum
{
    top_left,
    top_right,
    down_left,
    down_right
}movement_towards;

class BallTask: public scheduler_task
{
public :
    BallTask(uint8_t priority);
    void reset_ball();
    bool run(void *p);
};

class ScoreTask: public scheduler_task
{
public :
    ScoreTask(uint8_t priority);
    bool run(void *p);
};

class sliderTask: public scheduler_task
{
public :
    sliderTask(uint8_t priority);
    bool run(void *p);
};

class StartScreen: public scheduler_task
{
public :
    StartScreen(uint8_t priority): scheduler_task("StartScreen", 2048, priority){

    }
    bool run(void *p);
};



#endif /* DISPLAY_NODE_HPP_ */
