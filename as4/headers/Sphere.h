#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

using namespace std;
class Material;

class HitResult {
public:
    HitResult() { m_isHit = false; };
    bool m_isHit;
    Vector3D m_hitPos;
    Vector3D m_hitNormal;
    shared_ptr<Material> m_hitMaterial;
    float m_t;
};


class Sphere {
    
public:
    Vector3D m_center;
    float m_radius;
    shared_ptr<Material> m_pMaterial;

    Sphere() {}

    Sphere(Vector3D center, float r, shared_ptr<Material> m) {
        m_center = center;
        m_radius = r;
        m_pMaterial = m;
    }

    HitResult hit(Ray& r, float min_t, float max_t);
};


// Test if ray hits this sphere within range min_t and max_t
HitResult Sphere::hit(Ray& ray, float min_t, float max_t) {
    HitResult hit_result;
    
    // o - c
    Vector3D oc = ray.origin() - m_center;
    // d . (o - c)
    float half_b = dot(ray.direction(), oc);
    // d . d (a) = 1 since d is normalized
    // (o - c) . (o - c) - r^2
    float c = oc.length_squared() - m_radius * m_radius;
    // (b / 2)^2 - ac
    float discriminant = half_b * half_b - c;

    // Check if an intersection exists
    if (discriminant >= 0) {

        // If t1 is within range
        float t1 = (-half_b - sqrt(discriminant));
        if (min_t <= t1 && t1 <= max_t) {
            hit_result.m_isHit = true;
            hit_result.m_t = t1;
        }
        // If t1 is out of range but t2 is within range
        else {
            float t2 = (-half_b + sqrt(discriminant));
            if (min_t <= t2 && t2 <= max_t) {
                hit_result.m_isHit = true;
                hit_result.m_t = t2;
            }
        }
    }

    // When a hit exists, compute other attributes
    if (hit_result.m_isHit) {
        hit_result.m_hitPos = ray.at(hit_result.m_t);
        hit_result.m_hitNormal = (hit_result.m_hitPos - m_center) / m_radius;
        hit_result.m_hitMaterial = m_pMaterial;
    }
    
    return hit_result;
}

#endif
