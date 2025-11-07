#!/bin/bash
# ======================================================
# Génère la ligne "Depends:" pour un paquet Debian
# à partir des bibliothèques utilisées par un exécutable.
# ======================================================

if [ -z "$1" ]; then
    echo "Usage: $0 chemin/vers/executable"
    exit 1
fi

BIN="$1"

if [ ! -f "$BIN" ]; then
    echo "❌ Erreur : fichier non trouvé : $BIN"
    exit 1
fi

echo "🔍 Analyse des dépendances de $BIN..."
echo

# 1. Liste des bibliothèques dynamiques
LIBS=$(ldd "$BIN" | grep "=> /" | awk '{print $3}' | sort -u)

# 2. Trouver les paquets Debian correspondants
DEBS=""
for lib in $LIBS; do
    PKG=$(dpkg -S "$lib" 2>/dev/null | cut -d: -f1 | head -n1)
    if [ -n "$PKG" ]; then
        DEBS+="$PKG, "
    fi
done

# 3. Supprimer la dernière virgule et espace
DEBS=$(echo "$DEBS" | sed 's/, $//')

# 4. Générer la ligne finale
echo "✅ Ligne à insérer dans DEBIAN/control :"
echo
echo "Depends: $DEBS"
echo
echo "💡 Vérifie manuellement si certaines dépendances Qt doivent être remplacées par un paquet plus générique (ex: qt6-base-dev, qt6-base-bin, etc.)"
