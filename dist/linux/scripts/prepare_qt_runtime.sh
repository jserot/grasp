#!/bin/bash
set -e

GRASP_BIN="./src/Grasp"  # Executable (qui va être patché)
QT_DIR="/home/js/Qt/6.9.1/gcc_64"
TARGET_LIB="/usr/lib/grasp/qt6" # Chemin dans le paquet .deb

echo "Préparation du runtime Qt..."

# Répertoire temporaire (image du .deb)
APPDIR=$(pwd)/qt-runtime
rm -rf "$APPDIR"
mkdir -p "$APPDIR$TARGET_LIB"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR$TARGET_LIB/plugins"

echo "Copie du binaire..."
cp "$GRASP_BIN" "$APPDIR/usr/bin/grasp"

echo "Analyse des dépendances Qt..."
QT_LIBS=$(ldd "$GRASP_BIN" | grep "Qt6" | awk '{print $3}')

for lib in $QT_LIBS; do
    echo " -> copie : $(basename $lib)"
    cp "$lib" "$APPDIR$TARGET_LIB/"
done

echo "Analyse des dépendances transitives Qt..."
for lib in $QT_LIBS; do
    EXTRA=$(ldd "$lib" | grep "Qt6" | awk '{print $3}')
    for x in $EXTRA; do
        if [ ! -f "$APPDIR$TARGET_LIB/$(basename $x)" ]; then
            echo " -> copie (transitive) : $(basename $x)"
            cp "$x" "$APPDIR$TARGET_LIB/"
        fi
    done
done

echo "Copie des plugins Qt..."
PLUGIN_DIRS=(
    platforms
    imageformats
    tls
    styles
    iconengines
    xcbglintegrations
)

for dir in ${PLUGIN_DIRS[@]}; do
    echo " -> plugins/$dir"
    mkdir -p "$APPDIR$TARGET_LIB/plugins/$dir"
    cp "$QT_DIR/plugins/$dir"/*.so "$APPDIR$TARGET_LIB/plugins/$dir/" 2>/dev/null || true
done

echo "Création de qt.conf..."
cat > "$APPDIR/usr/bin/qt.conf" << EOF
[Paths]
Prefix = ../lib/grasp/qt6
Plugins = plugins
Libraries = .
EOF

echo "Fix RUNPATH..."
patchelf --set-rpath '$ORIGIN/../lib/grasp/qt6' "$APPDIR/usr/bin/grasp"

echo ""
echo "=============================================="
echo "Qt runtime généré dans : $APPDIR"
echo "Prêt à être intégré dans le paquet .deb"
echo "=============================================="
