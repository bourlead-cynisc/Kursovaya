#pragma once
#include <QApplication>
#include "darkThemeTokens.h"
#include "lightThemeTokens.h"
#include "buildStyleSheet.h"
#include "buildPalette.h"

inline void applyTheme(bool dark) {
    const auto tokens = dark ? darkThemeTokens() : lightThemeTokens();
    qApp->setPalette(buildPalette(tokens));
    qApp->setStyleSheet(buildStyleSheet(tokens));
}
