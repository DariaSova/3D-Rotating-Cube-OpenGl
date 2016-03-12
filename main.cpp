#include "Canvas.h"
#include <math.h>

void InitializeGL()
{
}

void MouseMove(double x, double y)
{
}

void MouseButton(MouseButtons mouseButton, bool press)
{
}

void KeyPress(char keychar)
{
}

void OnPaint()
{
}

void OnTimer()
{
}

int main(int, char **){
    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.1, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "Image Kit Demo");
    //Do our initialization
    InitializeGL();
    canvas.Show();
    return 0;
}

