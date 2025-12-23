# Projet Rasterization 3D

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue)
![Standard](https://img.shields.io/badge/C%2B%2B-17-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-gray)
![GLM](https://img.shields.io/badge/GLM-0.9.9.8-lightgrey)
![libjpeg](https://img.shields.io/badge/libjpeg-9f-orange)

Ce projet présente un <span style="color: #3EB489"><strong>logiciel de rendu 3D</strong></span> simple utilisant
le standard **OpenGL**, afin de comprendre ce qu’est la <span style="color: #3EB489"><strong>rasterization</strong></span>
et son fonctionnement, ainsi que les techniques qui l’accompagnent.

## Fonctionnalités implémentées

* Lecture de fichiers `.obj` provenant de [Blender 3D](https://www.blender.org/)  
* Gestion et lecture des fichiers textures : albedo, normal map, parallax map, etc.
* Création des matrices de **vue**, **projection** et **monde**  
* Implémentation du **Z-Buffer**  
* Implémentation de la lumière selon le modèle **Phong**
* Implémentation du **Space-Screen Reflection** de la manière la plus simple

## Techniques utilisées

* Barycentre  
* Modèle d’éclairage de Phong  
* Normal mapping  
* Parallax mapping  
* Screen-space reflection

## Technologies et bibliothèques utilisées

* **C++** : langage principal du projet  
* **GLM** (*OpenGL Mathematics*) : bibliothèque mathématique pour les vecteurs et matrices  
* **libjpeg** : utilisée pour le chargement et la gestion des textures au format JPEG  

## Illustrations

![Rendu du parallax](/Exemples/wall/wall.jpg)

* rendu d'un mur en brique avec du parallax.

Plus d'images d’exemple sont disponibles dans le répertoire /Exemples.

## Liens utilisés pour ce projet personnel

* <https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel.html>  
* <https://learnopengl.com>
