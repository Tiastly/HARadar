#include "window.h"
// #include "Debug.h"
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
        if (velocity > 1000 || velocity < -1000)
            return;
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

int resultAnalyse(float *result, int16_t *coordinateX, int16_t *coordinateY, bool game)
{
    Serial.print("output:\n");
    int idx = 0;
    for (int i = 1; i < 5; i++)
    {
        if (result[i] > result[idx])
        {
            idx = i;
        }
        Serial.printf("%f,", result[i]);
    }
    Serial.printf("could be: %s", ACTION_LABEL[idx]);
    if (result[idx] < 0.5)
    {
        Serial.println("too low confidence");
        return -1;
    }
    return game?idx:(checkCoordinate(coordinateX, coordinateY, idx) ? idx : -1);
}
bool checkCoordinate(int16_t *coordinateX, int16_t *coordinateY, int idx)
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
float *getInferenceVelocity(struct InferenceWindow *InferenceWindow, uint8_t i)
{
    return InferenceWindow->velocityInput[i];
}
void addVelocities(struct InferenceWindow *InferenceWindow, int16_t velocity, uint8_t i)
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
void addCoordinates(struct InferenceWindow *InferenceWindow, int16_t x, int16_t y, uint8_t i)
{
    if (InferenceWindow->isFull == false)
    {
        InferenceWindow->coordinateX[i][InferenceWindow->idx] = x;
        InferenceWindow->coordinateY[i][InferenceWindow->idx] = y;
    }
}

float *getVelocities(struct InferenceWindow *InferenceWindow, uint8_t i)
{
    return InferenceWindow->velocityInput[i];
}
int16_t *getCoordinateXs(struct InferenceWindow *InferenceWindow, uint8_t i)
{
    return InferenceWindow->coordinateX[i];
}
int16_t *getCoordinateYs(struct InferenceWindow *InferenceWindow, uint8_t i)
{
    return InferenceWindow->coordinateY[i];
}