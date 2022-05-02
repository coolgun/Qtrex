#ifndef MAINWND_H
#define MAINWND_H
#include <Wt/WContainerWidget.h>

class MainWnd : public Wt::WContainerWidget
{
    void setupUi();
public:
    MainWnd();

private:
    class QGameBoard* board;

};

#endif // MAINWND_H
