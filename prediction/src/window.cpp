#include "window.h"
const char *ACTION_LABEL2[5] = {"Sitting", "Stretching", "Walking", "Jumping", "Running"};
struct InferenceWindow initInferenceWindow()
{
    struct InferenceWindow InferenceWindow;

    memset(InferenceWindow.velocityInput, 0, sizeof(InferenceWindow.velocityInput));
    memset(InferenceWindow.coordinateX, 0, sizeof(InferenceWindow.coordinateX));
    memset(InferenceWindow.coordinateY, 0, sizeof(InferenceWindow.coordinateY));
    InferenceWindow.idx = 0;
    InferenceWindow.isFull = false;
    return InferenceWindow;
}

void addInferenceWindow(struct InferenceWindow *InferenceWindow, HLK_LD2450 frame, uint8_t i)
{
    if (InferenceWindow->isFull == false)
    {
        int16_t velocity = frame.getVelocity(i);
        int16_t x = frame.getCoordinateX(i);
        int16_t y = frame.getCoordinateY(i);
        Serial.printf("velocity: %d,", velocity);
        Serial.printf("x: %d,", x);
        Serial.printf("y: %d\n", y);
        InferenceWindow->velocityInput[i][InferenceWindow->idx] = velocity;
        InferenceWindow->coordinateX[i][InferenceWindow->idx] = x;
        InferenceWindow->coordinateY[i][InferenceWindow->idx] = y;
        InferenceWindow->idx++;
        if (InferenceWindow->idx >= WIN_SIZE)
        {
            InferenceWindow->idx = 0;
            InferenceWindow->isFull = true;
        }
    }
}

bool isInferenceWindowFull(struct InferenceWindow *InferenceWindow)
{
    return InferenceWindow->isFull;
}
float *getInferenceWindow(struct InferenceWindow *InferenceWindow, uint8_t i)
{
    if (InferenceWindow->isFull)
    {
        InferenceWindow->isFull = false;
        return InferenceWindow->velocityInput[i];
    }
    return NULL;
}
int resultAnalyse(float *result, int16_t *coordinateX, int16_t *coordinateY)
{
    int idx = 0;
    for (int i = 1; i < 5; i++)
    {
        if (result[i] > result[idx])
        {
            idx = i;
        }
        Serial.printf("%f,", result[i]);
    }
    Serial.printf("could be: %s",ACTION_LABEL2[idx]);

    if (result[idx] < 0.5)
    {
        Serial.printf("too low confidence\n");
        return -1;
    }
    return checkCoordinate(coordinateX, coordinateY,idx) ? idx : -1;
}
bool checkCoordinate(int16_t *coordinateX, int16_t *coordinateY,int idx)
{
    int16_t maxThreshold = 155;
    int16_t threshold = 15;
    for (int i = 1; i < WIN_SIZE; ++i)
    {
        int16_t deltaX = std::abs(coordinateX[i] - coordinateX[i - 1]);
        int16_t deltaY = std::abs(coordinateY[i] - coordinateY[i - 1]);

        if (deltaX > maxThreshold || deltaY > maxThreshold)
            continue;
        
        int16_t change = std::max(deltaX, deltaY);
        // run or walk
        if (change < threshold && (idx == 2 || idx == 4))
            return false;

    }
    return true;
}
void addVelocity(struct InferenceWindow *InferenceWindow, int16_t velocity, uint8_t i)
{
    if (InferenceWindow->isFull == false)
    {
        InferenceWindow->velocityInput[i][InferenceWindow->idx] = velocity;
        InferenceWindow->idx++;
        if (InferenceWindow->idx >= WIN_SIZE)
        {
            InferenceWindow->idx = 0;
            InferenceWindow->isFull = true;
        }
    }
}
void addCoordinate(struct InferenceWindow *InferenceWindow, int16_t x, int16_t y, uint8_t i)
{
    if (InferenceWindow->isFull == false)
    {
        InferenceWindow->coordinateX[i][InferenceWindow->idx] = x;
        InferenceWindow->coordinateY[i][InferenceWindow->idx] = y;
    }
}
