#include "stdafx.h"
#include "content.h"

Content *g_pContent = NULL;

Content::Content(QFrame *parent)
	: QFrame(parent)
{
	setObjectName("content");
	//setMouseTracking(true);


}

Content::~Content()
{
}

void Content::InitControl()
{
	
}
