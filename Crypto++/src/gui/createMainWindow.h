#pragma once
#include <QMainWindow>
#include <QProgressBar>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "AppContext.h"
#include "applyTheme.h"
#include "createAesPage.h"
#include "createEcdsaPage.h"
#include "createHashPage.h"
#include "createLogPanel.h"
#include "createRsaPage.h"
#include "createMagmaPage.h"
#include "createStreebogPage.h"
#include "createKuznyechikPage.h"
#include "createSidebar.h"
#include "../core/Log.h"

inline QMainWindow* createMainWindow() {
    auto* win = new QMainWindow;
    win->setAttribute(Qt::WA_DeleteOnClose);   // окно уничтожается до QApplication
    win->setWindowTitle(QStringLiteral("Crypto++ Utility"));
    win->resize(1120, 900);
    win->setMinimumSize(920, 660);

    auto* ctx = new AppContext(win);
    ctx->window = win;

    auto* root = new QWidget;
    root->setObjectName("root");
    auto* h = new QHBoxLayout(root);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);

    auto* stack = new QStackedWidget;
    stack->addWidget(createAesPage(ctx));
    stack->addWidget(createRsaPage(ctx));
    stack->addWidget(createHashPage(ctx));
    stack->addWidget(createEcdsaPage(ctx));
    stack->addWidget(createMagmaPage(ctx));
    stack->addWidget(createStreebogPage(ctx));
    stack->addWidget(createKuznyechikPage(ctx));

    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(stack);
    ctx->content = scroll;

    auto* progress = new QProgressBar;
    progress->setTextVisible(false);
    progress->setRange(0, 1);
    progress->setValue(0);
    ctx->progress = progress;

    auto* logPanel = createLogPanel(ctx);
    auto* split = new QSplitter(Qt::Vertical);
    split->setChildrenCollapsible(false);
    split->setHandleWidth(8);
    split->addWidget(scroll);
    split->addWidget(logPanel);
    split->setStretchFactor(0, 4);
    split->setStretchFactor(1, 1);
    split->setSizes({ 700, 170 });

    auto* right = new QWidget;
    auto* rv = new QVBoxLayout(right);
    rv->setContentsMargins(0, 0, 0, 14);
    rv->setSpacing(0);
    rv->addWidget(progress);
    auto* splitWrap = new QHBoxLayout;
    splitWrap->setContentsMargins(20, 0, 20, 0);
    splitWrap->addWidget(split);
    rv->addLayout(splitWrap, 1);

    h->addWidget(createSidebar(ctx, stack));
    h->addWidget(right, 1);
    win->setCentralWidget(root);

    applyTheme(ctx->darkTheme);
    Log("Приложение запущено");
    return win;
}
