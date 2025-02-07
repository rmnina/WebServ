#!/bin/bash

echo "---- DEBUG: Début du script ----" >> debug.txt

# Afficher les informations de base sur la requête HTTP
echo "CONTENT_LENGTH: $CONTENT_LENGTH" >> debug.txt
echo "QUERY_STRING: '$QUERY_STRING'" >> debug.txt
echo "REQUEST_METHOD: '$REQUEST_METHOD'" >> debug.txt

# Lire les données POST correctement
echo "CONTENT_LENGTH again: $CONTENT_LENGTH" >> debug.txt
exec 3<&0  # Sauvegarde stdin avant de manipuler
echo "Lecture manuelle de stdin..." >> debug.txt

# Tentons une lecture propre de stdin
POST_DATA=""
if [ -n "$CONTENT_LENGTH" ] && [ "$CONTENT_LENGTH" -gt 0 ]; then
    echo "***** On entre dans le block de lecture" >> debug.txt
    $POST_DATA=$(cat)
    echo "***** POST_DATA après dd: $POST_DATA" >> debug.txt
else
    echo "***** CONTENT_LENGTH est vide ou nul !" >> debug.txt
fi

exec 0<&3  # Restaure stdin pour pouvoir utiliser d'autres commandes si nécessaire

# Vérification de ce qu'on a récupéré
echo "---- DEBUG: Lecture des données POST ----" >> debug.txt
echo "POST DATA: $POST_DATA" >> debug.txt

# Si on a des données, on les passe à PHP
if [ -n "$POST_DATA" ]; then
    echo "***** On passe POST_DATA à PHP" >> debug.txt
    RESULT=$(echo "$POST_DATA" | php /home/eltouma/code/webserv/www/cgi/astro.php 2>> debug_php.txt)
    echo "***** Résultat de PHP: $RESULT" >> debug.txt
else
    echo "***** Aucune donnée POST à traiter !" >> debug.txt
fi

# Réponse HTTP correcte
echo "Content-Type: text/plain"
echo ""
echo "$RESULT"

