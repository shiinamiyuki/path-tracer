//
// Created by xiaoc on 2018/10/9.
//

#include "kdtree.h"
#include "fmt/format.h"

Hit KDTree::search(const Ray &ray) {
    auto expected = fullSearch(ray);
    if(expected.hit()){
        fmt::print("{0}",expected.distance);
    }
    auto hit = box.intersect(ray);
    if (hit.distance < eps) {
        return hit;
    }
    auto pt = ray.getIntersectionPoint(hit.distance);
    if (!left && !right) { // leaf
        if(object)
            return object->intersect(ray);
        else
            return Hit{nullptr, -1, {}};
    }
    if(left){
        hit = left->box.intersect(ray);
        if(hit.hit()){
            return left->search(ray);
        }
    }
    if(right){
        hit = right->box.intersect(ray);
        if(hit.hit()){
            return right->search(ray);
        }
    }
/*
    if (pt.get(axis) < box.max.get(axis)) {
        if (left)
            return left->search(ray);
        else {
            return Hit{nullptr, -1, {}};
        }
    } else {
        if (right)
            return right->search(ray);
        else {
            return Hit{nullptr, -1, {}};
        }
    }*/
    return Hit{nullptr, -1, {}};
}

Hit KDTree::fullSearch(const Ray &ray) {
    if(!left && !right){
        return object->intersect(ray);
    }else{
        if(left){
            auto hit = left->fullSearch(ray);
            if(hit.hit())
                return hit;
            if(right){
                hit = right->fullSearch(ray);
                if(hit.hit())
                    return hit;
            }
        }
    }
    return Hit{nullptr,-1,{}};
}
