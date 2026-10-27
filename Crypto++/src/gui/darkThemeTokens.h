#pragma once
#include <QHash>
#include <QString>

inline QHash<QString, QString> darkThemeTokens() {
    return {
        {"bg",          "#0e1117"}, {"sidebar",     "#12161f"}, {"surface",    "#161b25"},
        {"surface2",    "#1d2330"}, {"border",      "#2a3142"}, {"text",       "#e7eaf1"},
        {"muted",       "#8b93a7"}, {"accent",      "#7c93ff"}, {"accentHover","#97aaff"},
        {"onAccent",    "#0b0f1a"}, {"accentSoft",  "rgba(124,147,255,38)"},
        {"success",     "#3ecf8e"}, {"successSoft", "rgba(62,207,142,34)"},
        {"danger",      "#ff6b6b"}, {"dangerSoft",  "rgba(255,107,107,34)"},
    };
}
