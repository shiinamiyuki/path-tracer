//
// Created by xiaoc on 2018/10/9.
//

#ifndef PATH_TRACER_VOXELMANAGER_H
#define PATH_TRACER_VOXELMANAGER_H

#include "util.h"
#include "cube.h"

class Voxel : public Cube {
public:
    bool occupied;
    bool unreachable;

    Voxel() {
        unreachable = false;
        occupied = false;
    }

    Voxel(const Vector3 &c, const Material &m, double s) : Cube(c, m, s) {
        unreachable = false;
        occupied = true;
        material = m;
    }
};

struct VoxelKDTree {
    unsigned int axis;
    std::vector<Voxel *> voxels;
    AABB box;
    VoxelKDTree *left, *right;

    void addLeft(VoxelKDTree *t) { left = t; }

    void addRight(VoxelKDTree *t) { right = t; }

    VoxelKDTree(const std::vector<Voxel *>& _voxel, unsigned int _a, const AABB &aabb){
        axis = _a;
        voxels = _voxel;
        box = aabb;
        left = right = nullptr;
    }
    VoxelKDTree(Voxel *_voxel,  unsigned int _a,const AABB &aabb) {
        axis = _a;
        if(_voxel)
        voxels.emplace_back(_voxel);
        box = aabb;
        left = right = nullptr;
    }
    Hit search(const Ray & ray);
    Hit fullSearch(const Ray &ray);
    int countLeaves(){
        auto ans = 0;
        if(left)
            ans +=left->countLeaves();
        if(right)
            ans += right->countLeaves();
        return ans + (int)voxels.size();
    }
    void check(){
        if(!left &&!right){
            for(auto i:voxels)
                ptAssert(box.contains(i->box));
        }else{
            if(left)
            left->check();
            if(right)
            right->check();
        }

    }
};
class Scene;
class VoxelManager : public RenderObject {
    double voxelSize;
    int dimension;
    std::vector<Voxel *> voxel;
    VoxelKDTree *kdTree;
public:
    VoxelManager(const Vector3 &c, double size, int dim) {
        center = c;
        voxelSize = size;
        dimension = dim;
        for (int i = 0; i < dimension * dimension * dimension; i++) {
            voxel.emplace_back(new Voxel());
        }
    }

    inline int getIdx(int x, int y, int z) const {
        return (x * dimension + y) * dimension + z;
    }

    void addVoxel(int x, int y, int z, const Material &m) {
        if (x >= dimension || y >= dimension || z >= dimension || x < 0 || y < 0 || z < 0)
            return;
        voxel[getIdx(x, y, z)] = new Voxel(center + Vector3(x, y, z) * voxelSize, m, voxelSize);
    }

    void prepare() override {
        removeUnreachable();
        kdTree = constructKDTree(AABB(center - Vector3{1,1,1}*dimension*voxelSize,center + Vector3{1,1,1}*dimension*voxelSize),voxel,0);
        ptAssert(kdTree->countLeaves() == voxel.size());
        kdTree->check();
    }

    VoxelKDTree *constructKDTree(const AABB& box, std::vector<Voxel *> vec, unsigned int depth);

    void removeUnreachable();

    Hit intersect(const Ray &ray) override;
    void addLightSourceToScene(Scene&s);
};


#endif //PATH_TRACER_VOXELMANAGER_H
