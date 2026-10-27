#pragma once
#include <QHash>
#include <QString>

inline QHash<QString, QString> lightThemeTokens() {
    return {
        {"bg",          "#f3f5fa"}, {"sidebar",     "#ffffff"}, {"surface",    "#ffffff"},
        {"surface2",    "#f1f3f9"}, {"border",      "#dfe3ee"}, {"text",       "#1b2030"},
        {"muted",       "#6a7389"}, {"accent",      "#4f6df5"}, {"accentHover","#3f5be0"},
        {"onAccent",    "#ffffff"}, {"accentSoft",  "rgba(79,109,245,32)"},
        {"success",     "#14946a"}, {"successSoft", "rgba(20,148,106,30)"},
        {"danger",      "#d93a3f"}, {"dangerSoft",  "rgba(217,58,63,28)"},
    };
}
