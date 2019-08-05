/**
 * This is a scratch space for testing new functions and such on the fly.
 */

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <array>
#include <memory>

#include "gamer/gamer"

// #include <casc/casc>


int  main(int argc, char *argv[])
{
    auto mesh = gamer::sphere(1);

    for(auto fID : mesh->get_level_id<3>()){
        (*fID).marker = 5;
    }

    // gamer::writeOFF("sphere.off", *mesh);
    // gamer::writeOFFm("spherem.off", *mesh);
    gamer::writeOBJ("sphere.obj", *mesh);

    auto mesh2 = gamer::readOFFm("spherem.off");

    for (auto fID : mesh2->get_level_id<3>()){
        std::cout << fID << " " << (*fID).marker << std::endl;
    }

    // auto mesh = gamer::readOFF("icosa.off");

    // int i = 0;
    // for(auto v : mesh->get_level_id<1>()){
    //     auto data = *v;
    //     std::cout << "mesh->insert({" << i++
    //         << "}, SMVertex("
    //         << data[0] << ", "
    //         << data[1] << ", "
    //         << data[2] << "));" << std::endl;
    // }

    // for(auto f : mesh->get_level_id<3>()){
    //     auto name = f.indices();
    //     std::cout << "mesh->insert({"
    //         << name[0] << ","
    //         << name[1] << ","
    //         << name[2]
    //     << "});" << std::endl;
    // }


    std::cout << "EOF" << std::endl;
}