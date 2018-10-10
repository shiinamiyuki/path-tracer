//
// Created by xiaoc on 2018/10/9.
//

#include "voxelmanager.h"
#include "fmt/format.h"
#include "scene.h"
Hit VoxelManager::intersect(const Ray &ray) {
    return kdTree->fullSearch(ray);
    double minDist = 9999999;
    RenderObject *object = nullptr;
    Vector3 norm;
    for (auto i:voxel) {
        auto hit = i->intersect(ray);
        if (hit.hit()) {
            if (minDist > hit.distance) {
                minDist = hit.distance;
                object = i;
                norm = hit.normal;
            }
        }
    }
    return Hit{object, minDist, norm};
}

void VoxelManager::removeUnreachable() {
    for (int x = 1; x < dimension - 1; x++) {
        for (int y = 1; y < dimension - 1; y++) {
            for (int z = 1; z < dimension - 1; z++) {
                bool flag = true;
                for (int i = x - 1; i <= x + 1; i++) {
                    for (int j = y - 1; j <= y + 1; j++) {
                        for (int k = z - 1; k <= z + 1; k++) {
                            if (x == i && j == y && z == k) {
                                continue;
                            }
                            flag = flag && voxel[getIdx(i, j, k)]->occupied;
                        }
                    }
                }
                voxel[getIdx(x, y, z)]->unreachable = flag;
            }
        }
    }
    for (int x = 1; x < dimension - 1; x++) {
        for (int y = 1; y < dimension - 1; y++) {
            for (int z = 1; z < dimension - 1; z++) {
                if (voxel[getIdx(x, y, z)]->unreachable) {
              //      fmt::print("removed {0},{1},{2}", x, y, z);
                    voxel[getIdx(x, y, z)]->occupied = false;
                }
            }
        }
    }
    std::vector<Voxel *> temp;
    for (auto &i:voxel) {
        if (i->occupied)
            temp.emplace_back(i);
    }
    voxel = temp;
}

VoxelKDTree *VoxelManager::constructKDTree(const AABB &box, std::vector<Voxel *> vec, unsigned int depth) {
    if (vec.empty())
        return nullptr;
    unsigned int axis = depth % 3;
    if (vec.size() <= 2) {
        return new VoxelKDTree(vec, axis, box); // objects only on leaves
    }
    std::sort(vec.begin(), vec.end(), [=](Voxel *a, Voxel *b) {
        return a->box.max.get(axis) < b->box.max.get(axis);
    });
    for (auto i:vec) {
   //     fmt::print("{0} ", i->box.max.get(axis));
    }

    auto median = vec[vec.size() / 2];
    auto medIdx = vec.size() / 2;
    auto med = median->box.max.get(axis);
  //  fmt::print("\n m = {0}\n", med);
    auto tree = new VoxelKDTree(nullptr, axis, box);
    AABB left, right;
    auto get = [=](unsigned int _i) {
        if (_i == axis) {
            return med;
        } else {
            return box.max.get(_i);
        }
    };
    auto get2 = [=](unsigned int _i) {
        if (_i == axis) {
            return med;
        } else {
            return box.min.get(_i);
        }
    };
    for (const auto &i:vec) {
        box.assetContains(i->box);
    }
    left = AABB(box.min, Vector3{get(0), get(1), get(2)});
    right = AABB(Vector3{get2(0), get2(1), get2(2)}, box.max);
    std::vector<Voxel *> v1, v2;
    for (auto idx = 0; idx < vec.size(); idx++) {
        /*    if (idx < medIdx) {
                left.assetContains(vec[idx]->box);
                v1.emplace_back(vec[idx]);
            } else {
                right.assetContains(vec[idx]->box);
                v2.emplace_back(vec[idx]);
            }*/
        if (left.contains(vec[idx]->box)) {
            v1.emplace_back(vec[idx]);
        } else {
            right.assetContains(vec[idx]->box);
            v2.emplace_back(vec[idx]);
        }
    }
    if(v1.empty() || v2.empty()){
        tree->voxels = vec;
        return tree;
    }
    //fmt::print("left: {0}, right: {1}\n", v1.size(), v2.size());
    tree->addLeft(constructKDTree(left, v1, depth + 1));
    tree->addRight(constructKDTree(right, v2, depth + 1));
    return tree;
}

Hit VoxelKDTree::search(const Ray &ray) {
    /*
    if (!box.intersect(ray).hit())
        return Hit{nullptr, -1, {}};
    if (!left && !right) {
        auto hit = voxel->intersect(ray);
        if (hit.hit()) {
            hit.object = voxel;
            return hit;
        } else {
            return {};
        }
    }

    Hit L, R, result;
    /*
    L = left->search(ray);
    R = right->search(ray);
    if(!L.hit())return R;
    if(!R.hit())return L;
    if(L.distance<R.distance)return L;else return R;

    auto searchLeft = [&]() {
        result = left->search(ray);
        if (!result.hit()) {
            return right->search(ray);
        }
        return result;
    };
    auto searchRight = [&]() {
        result = right->search(ray);
        if (!result.hit()) {
            return left->search(ray);
        }
        return result;
    };

    if (left) {
        L = left->box.intersect(ray);
    }
    if (right) {
        R = right->box.intersect(ray);
    }
    if (!L.hit()) {
        return right->search(ray);
    }
    if (!R.hit()) {
        return left->search(ray);
    }
    //both hit
    if (left->box.contains(ray.origin)) {
        return searchLeft();
    }
    if (right->box.contains(ray.origin)) {
        return searchRight();
    }
    if (L.distance < R.distance) {
        return searchLeft();
    } else {
        return searchRight();
    }*/
}

Hit VoxelKDTree::fullSearch(const Ray &ray) {
    if (box.intersect(ray).hit() || box.contains(ray.origin)) {
        if (!left && !right) {
            auto min = 9999999.0;
            RenderObject *v = nullptr;
            Vector3 n;
            for (auto i:voxels) {
                auto hit = i->intersect(ray);
                if (hit.hit() && hit.distance < min) {
                    min = hit.distance;
                    n = hit.normal;
                    v = i;
                }
            }
            if (v)
                return Hit(v, min, n);
            else
                return {};
        }
        auto L = left ? left->fullSearch(ray) : Hit();
        auto R = right ? right->fullSearch(ray) : Hit();
        if (!L.hit())return R;
        if (!R.hit())return L;
        if (L.distance < R.distance)return L;
        else return R;
    } else
        return {};
}
void VoxelManager::addLightSourceToScene(Scene&s){
    for(auto i:voxel) {
        if(i->material.type == EMIT)
            s.addLightSource(i);
    }
}