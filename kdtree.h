//
// Created by xiaoc on 2018/10/9.
//

#ifndef PATH_TRACER_KDTREE_H
#define PATH_TRACER_KDTREE_H
#include "util.h"
#include "renderobject.h"
#include "aabb.h"
class KDTree{
    KDTree * left,*right;
    AABB box;
    RenderObject * object;
    unsigned int axis;
public:
    KDTree(RenderObject * _o, unsigned int _a, const AABB&_box):object(_o),axis(_a){
        box = _box;
        left = right = nullptr;
    }
    void addLeft(KDTree * t){
        left = t;
    }
    void addRight(KDTree * t){
        right = t;
    }
    Hit search(const Ray &);
    Hit fullSearch(const Ray&);
};


#endif //PATH_TRACER_KDTREE_H
