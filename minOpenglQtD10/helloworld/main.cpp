#include <QApplication>
#include <QSurfaceFormat>
#include <QStackedLayout>
#include <QPushButton>
#include <QTimer>
#include "glwidget.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	//format.setSamples(4);
	QSurfaceFormat::setDefaultFormat(format);

	//QStackedLayout* stackedLayout = new QStackedLayout();
	//stackedLayout->setStackingMode(QStackedLayout::StackAll);

	//QPushButton* testpush = new QPushButton("tettttttt");
	//testpush->setWindowFlags(Qt::FramelessWindowHint);
	//testpush->setAttribute(Qt::WA_TranslucentBackground, true);

	//stackedLayout->addWidget(testpush);
	//stackedLayout->addWidget(new GLWidget());
	//stackedLayout->setCurrentIndex(1);

	//QTimer::singleShot(200, [&]() {
	//	stackedLayout->setCurrentIndex(0);
	//});

	//QWidget w;
	//w.setLayout(stackedLayout);
	//w.show();

	GLWidget w;
	w.show();

    return app.exec();
}
