**POUR COMPILER**

gcc main.c pieces.c logic.c render.c -o tetris $(sdl2-config --cflags --libs) -lSDL2_ttf -lSDL2_image

**POUR AJOUTER DES MODIFS :**

git add .

git commit -m "commentaire"

git push

**POUR Récupérer des modifs :**

git pull

**à installer pour éxécuter :**
sudo apt install libsdl2-image-dev
