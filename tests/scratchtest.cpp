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

#include "stringutil.h"
#include "SurfaceMesh.h"
#include "TetMesh.h"
#include "Vertex.h"
#include "tensor.h"
#include "PDBReader.h"

#include <libraries/casc/include/CASCFunctions.h>
#include <libraries/casc/include/SimplexSet.h>
#include <libraries/casc/include/SimplexMap.h>
#include <libraries/casc/include/SimplicialComplex.h>
#include <libraries/casc/include/decimate.h>
#include <libraries/casc/include/typetraits.h>

template <typename Complex>
struct Callback
{
    using SimplexSet = typename casc::SimplexSet<Complex>;
    using KeyType = typename Complex::KeyType;

    tetmesh::Cell operator()(Complex& F,
                             const std::array<KeyType, 4>& new_name,
                             const SimplexSet& merged){
        //std::cout << merged << " -> " << casc::to_string(new_name) << std::endl;
        return **merged.template cbegin<4>();
    }

    tetmesh::Face operator()(Complex& F,
                    const std::array<KeyType, 3>& new_name,
                    const SimplexSet& merged){
        return **merged.template cbegin<3>();
    }

    tetmesh::Edge operator()(Complex& F,
                    const std::array<KeyType, 2>& new_name,
                    const SimplexSet& merged){


       // tetmesh::TetVertex v1 = **merged.template cbegin<1>();
       // tetmesh::TetVertex v2 = **merged.template cend<1>();
/*
        auto v = v1 - v2;

        //TODO :: getter for this
        double vertexLoc = 0;
        auto pos = v1.position - v*vertexLoc;

        std::cout << merged << " -> " << casc::to_string(new_name) << std::endl;

        return tetmesh::TetVertex(pos);
        */
        return **merged.template cbegin<2>();
    }

    tetmesh::TetVertex operator()(Complex& F,
                      const std::array<KeyType, 1>& new_name,
                      const SimplexSet& merged){

        /*

        tetmesh::TetVertex v1 = *verts.cbegin();
        tetmesh::TetVertex v2 = *verts.end();

        v1 = verts.first();

        std::cout << v1 << "sdfsd" << std::endl;
        auto v = v1.position - v2.position;

        //TODO :: getter for this
        double vertexLoc = 0;
        Vector pos = v1 - v*vertexLoc;
         */
         //std::cout << merged << " -> " << casc::to_string(new_name) << std::endl;
  /*      return Vertex(pos);
        */
  return **merged.template cbegin<1>();
    }

    /*
    template <std::size_t k>
    typename Complex::template NodeDataTypes<k> operator()(Complex& F,
            const std::array<KeyType, k>& new_name,
            const SimplexSet& merged) {
        std::cout << merged << " -> " << new_name << std::endl;
        auto a = merged.template getlevel<k>();
        return 0;

    }
    */
};

int  main(int argc, char *argv[])
{
    auto mesh = readOFF(argv[1]);
    // auto mesh = readPQR_gauss(argv[1],-0.2, 2.5);
    // writeOFF("test.off", *mesh);


    auto vol = getVolume(*mesh);
    if (vol < 0) {
        flipNormals(*mesh);
    }

    auto &rootMetadata = *mesh->get_simplex_up();
    rootMetadata.ishole = false;

    std::cout << "Mesh read in" << std::endl;
    std::vector<SurfaceMesh*> vec;
    vec.push_back(mesh.get());

    auto tetmesh = makeTetMesh(vec, "q2/2O8/7AYVC");

    //edgeCollapse(*tetmesh, edge, 0.5, Callback<TetMesh>());
    decimation(*tetmesh, 10000, Callback<SurfaceMesh>());

    std::cout << "EOF" << std::endl;


    SurfaceMesh surfaceMesh;

    std::set<int> vertices;
    // Create surface mesh from tetrahedral mesh
    for (auto face : tetmesh->get_level_id<3>()) {
        if (tetmesh->get_cover(face).size() > 1) {
            continue;
        } else {
            auto name = tetmesh->get_name(face);
            auto v1 = name[0];
            auto v2 = name[1];
            auto v3 = name[2];
            surfaceMesh.insert({v1,v2,v3});

            for (auto key : name){
                vertices.insert(key);
            }
        }
    }
    for (auto vertexKey : vertices){
        auto& data = *surfaceMesh.get_simplex_up({vertexKey});
        data = Vertex((*tetmesh->get_simplex_up({vertexKey})).position);
    }

    compute_orientation(surfaceMesh);

    writeOFF("meshOut.off", surfaceMesh);


}
