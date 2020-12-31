//
// Created by ryu19 on 2020/12/30.
//

#ifndef RAYTRACING_HITTABLE_LIST_H
#define RAYTRACING_HITTABLE_LIST_H

#include "hittable.h"
#include <memory>
#include <vector>

using namespace std;

class hittable_list: public hittable{
public:
    hittable_list(){}
    hittable_list(shared_ptr<hittable> object){add(object);}
    void clear(){objects.clear();}
    void add(shared_ptr<hittable> object){objects.push_back(object);}
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
private:
    vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    hit_record tmp_rec;
    bool hit_anything = false;
    //現状で一番近い接触点
    auto closest_so_far = t_max;
    for(const auto& object: objects){
        //t_maxを現状で一番近い接点にすることで、それより近い点のみ検出している
        if(object->hit(r,t_min,closest_so_far,tmp_rec)){
            hit_anything = true;
            //接触点を更新
            closest_so_far = tmp_rec.t;
            rec = tmp_rec;
        }
    }
    return hit_anything;
}

#endif //RAYTRACING_HITTABLE_LIST_H
