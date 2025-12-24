#include <iostream>
#include "OutPut/Camera.hpp"
#include "LoadingFiles/Mesh.hpp"
#include "OutPut/Light.hpp"
#include "OutPut/Device.hpp"
#include "LoadingFiles/LoadObj.hpp"
#include <memory>
#include <thread>
#include <regex>
using namespace Render3D;

/**
 * @file Render3D.cpp
 * @brief Point d'entrée principal de l'application de rasterisation 3D.
 *
 * Ce fichier contient la fonction main qui initialise la scène, charge les objets,
 * configure la caméra, les lumières et lance le rendu.
 */

/**
 * @brief Fonction principale du programme.
 * @param argc Nombre d'arguments de la ligne de commande
 * @param argv Tableau des arguments de la ligne de commande
 * @return Code de sortie du programme
 */
int main(int argc, char *argv[])
{
    try
    {
        std::shared_ptr<Camera> camera = std::make_shared<Camera>();

        Lights l;

        // Light Setting.
        // Use exclusively in direct light.
        vec3 Light_Direction = {};
        Light_Direction.x = 1.0f;
        Light_Direction.y = 1.0f;
        Light_Direction.z = -1.0f;
        vec3 Light_Direction2 = {};
        Light_Direction2.x = -1.0f;
        Light_Direction2.y = -1.0f;
        Light_Direction2.z = -1.0f;
        // Suitable for other types of light.
        vec3 Light_Position = {};
        Light_Position.x = 0.0f;
        Light_Position.y = 0.0f;
        Light_Position.z = 1.0f;
        vec3 Light_Position2 = {};
        Light_Position2.x = 0.0f;
        Light_Position2.y = 3.0f;
        Light_Position2.z = 0.0f;
        vec3 red = {1.0f, 0.0f, 0.0f};
        vec3 green = {0.0f, 1.0f, 0.0f};
        vec3 blue = {0.0f, 0.0f, 1.0f};
        vec3 white = {1.0f, 1.0f, 1.0f};
        vec3 yellow = {1.0f, 1.0f, 0.0f};
        vec3 apricot = {0.9843f, 0.8078f, 0.6941f};

        camera->set_position(0.0f, 0.0f, 8.0f);

        if (argc < 2 || string(argv[1]) == "-h")
        {
            cout << "Usage: Render3D <file.obj> [-c=x,y,z] [-l=type]\n"
                 << "  -c: Camera position (x,y,z coordinates) (optional)\n"
                 << "  -l: Light type: sun, pointlight, spot (optional)\n"
                 << "Example:\n"
                 << "  Render3D model.obj -c=0.0,0.0,5.0 -l=sun\n";
            return 0;
        }
        if (argc >= 3)
        {
            if (string(argv[2]).substr(0, 3) == "-c=")
            {
                string texte = string(argv[2]);
                regex motif(R"(-?\d+(?:\.\d+)?)");
                vector<float> valeurs;

                try
                {
                    for (sregex_iterator it(texte.begin(), texte.end(), motif), end; it != end; ++it)
                    {
                        valeurs.push_back(stof(it->str()));
                    }
                    camera->set_position(valeurs[0], valeurs[1], valeurs[2]);
                }
                catch (const exception &e)
                {
                    cerr << "Error converting values: " << e.what() << endl;
                    return 1;
                }

                if (valeurs.size() != 3)
                {
                    cerr << "Error: -c must contain exactly 3 float values. Example: -c=0.0,0.5,1.2\n";
                    return 1;
                }
            }
            else if (string(argv[2]).substr(0, 3) == "-l=")
            {
                if (string(argv[2]).substr(3, 3) == "sun")
                {
                    l.setLight("sun", LightType::DirectionLight, {}, white, Light_Direction);
                }
                if (string(argv[2]).substr(3, 10) == "pointlight")
                {
                    l.setLight("pointlight", LightType::PointLight, Light_Position2, blue);
                }
                if (string(argv[2]).substr(3, 4) == "spot")
                {
                    vec3 v = {0.0f, 0.0f, -1.0f};
                    l.setLight("spot", LightType::SpotLight, Light_Position, white, v);
                }
            }
        }
        if (argc == 4)
        {
            if (string(argv[3]).substr(0, 3) == "-l=")
            {
                if (string(argv[3]).substr(3, 3) == "sun")
                {
                    l.setLight("sun", LightType::DirectionLight, {}, white, Light_Direction2);
                }
                if (string(argv[3]).substr(3, 10) == "pointlight")
                {
                    l.setLight("pointlight", LightType::PointLight, Light_Position2, white);
                }
                if (string(argv[3]).substr(3, 4) == "spot")
                {
                    vec3 v = {0.0f, 0.0f, -1.0f};
                    l.setLight("spot", LightType::SpotLight, Light_Position, white, v);
                }
            }
            else if (string(argv[3]).substr(0, 3) == "-c=")
            {
                string texte = string(argv[3]);
                regex motif(R"(-?\d+(?:\.\d+)?)");
                vector<float> valeurs;

                try
                {
                    for (sregex_iterator it(texte.begin(), texte.end(), motif), end; it != end; ++it)
                    {
                        valeurs.push_back(stof(it->str()));
                    }
                    camera->set_position(valeurs[0], valeurs[1], valeurs[2]);
                }
                catch (const exception &e)
                {
                    cerr << "Error converting values: " << e.what() << endl;
                    return 1;
                }

                if (valeurs.size() != 3)
                {
                    cerr << "Error: -c must contain exactly 3 float values. Example: -c=0.0,0.5,1.2\n";
                    return 1;
                }
            }
        }

        if (!std::filesystem::exists(string(argv[1])))
        {
            throw std::runtime_error("Error: The specified .obj file does not exist.");
        }

        Mesh m = Mesh();

        // Creating Device.
        std::unique_ptr<Device> d = std::make_unique<Device>(1000, 563);

        // Loading Objects.
        std::unique_ptr<LoadObj> obj = std::make_unique<LoadObj>(string(argv[1]));
        obj->get_Mesh(m);
        obj.reset();

        camera->set_target(0.0f, 0.0f, 0.0f);

        // Application of Transformation on Objects.
        // m.set_rotation(0,0.0f, glm::radians(45.0f), 0.0f);
        //  m.set_position(0,0.0f,0.0f,3.0f);

        for (MeshData me : m.get_meshData())
        {
            cout << "Name of Mesh: " << me.nameMesh << '\n'
                 << endl;
        }

        int const nbThreads = std::thread::hardware_concurrency();
        cout << "threads available:" << nbThreads << endl;

        /*Rendering*/
        d->RenderScene(camera, m, l);
    }
    catch (exception &e)
    {
        std::cerr << "The application has encountered an issue: " << e.what() << std::endl;
    }

    return 0;
}
