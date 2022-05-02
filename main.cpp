#include <Wt/WApplication.h>
#include "mainwnd.h"

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env)
{
	auto app = std::make_unique<Wt::WApplication>(env);
	auto container = std::make_unique<MainWnd>();
	app->root()->addWidget(std::move(container));
	return app;
}


int main(int argc, char** argv)
{
	return WRun(argc, argv, &createApplication);
}
