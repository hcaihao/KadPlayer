#ifndef CONTENT_H
#define CONTENT_H

#include <QFrame>

class Content : public QFrame
{
	Q_OBJECT

public:
	Content(QFrame *parent = 0);
	~Content();

protected:
private:
	void InitControl();
public slots:

private slots:
};

extern Content *g_pContent;

#endif // CONTENT_H
