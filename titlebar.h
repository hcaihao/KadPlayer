#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

class TitleBar : public QFrame
{
	Q_OBJECT

public:
	friend class MainWindow;
	friend class SkinWindow;
	friend class MessageWindow;
	friend class MiniPortalWindow;
	friend class SettingWindow;
	friend class PlayWindow;
	friend class CloudWindow;
	friend class FeedbackWindow;

	TitleBar(QFrame * parent = 0);
	~TitleBar();

protected:
	void mouseDoubleClickEvent(QMouseEvent *event);
private:
	void InitControl();
	void AddContextMenu();

	QMenu *m_pTitleMenu;
	QLabel *m_pTitleLabel;

	QToolButton *m_pSkinButton;
	QToolButton *m_pMiniPortalButton;
	QToolButton *m_pCloudPlayButton;
	QToolButton *m_pMenuButton;
	QToolButton *m_pMinButton;
	QToolButton *m_pMaxButton;
	QToolButton *m_pCloseButton;

	QRect m_windowRect;
	bool m_bMaxWin;
	bool m_bTopWin;
public slots:
	void showTop();
	void showSkin();
	void showFeedback();
	void showMiniPortal();
	void showCloud();
	void showMenu();
	void showMin();
	void showMaxRestore();
signals:
	void showSkinWindow();
	void showFeedbackWindow(int type, QString content);
	void showMiniPortalWindow();
	void showSettingWindow();
	void showCloudWindow();
	void showUpdate();
	void showHelp();
	void showAbout();
};

extern TitleBar *g_pTitleBar;

#endif // TITLEBAR_H