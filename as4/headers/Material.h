#ifndef MATERIAL_H
#define MATERIAL_H

#include<cassert>

class HitResult;

class ReflectResult {
public:
    Ray m_ray;
    Vector3D m_color;
};

class Material {
public:
    Vector3D m_color;
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) = 0;
};


class Diffuse : public Material {
public:
    Diffuse(const Vector3D& color) {
        m_color = color;
    };
    
    // Generate one scattered ray
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override {
        ReflectResult res;
        // Check if the hit indeed exists
        assert(hit.m_isHit == true);

        // Pick a direction randomly
        // By picking a point in the unit cube, each coordinate has the range of [-1, 1]
        // If the direction is not in the desired hemisphere (dir . hit.normal < 0)
        // Or the direction is invalid (i.e (0, 0, 0))
        // Pick another one, until the condition is satisfied
        Vector3D dir;
        do {
            dir = Vector3D::random(-1, 1);
        } while ((dot(dir, hit.m_hitNormal) < 0) || (dir.x() == 0 && dir.y() == 0 && dir.z() == 0));

        // Normalize the direction
        dir = normalize(dir);
        res.m_ray = Ray(hit.m_hitPos, dir);
        
        res.m_color = m_color;
        return res;
    }
};


class Specular : public Material {
public:
    Specular(const Vector3D& color) {
        m_color = color;
    }
    
    // Generate one mirrored ray
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override {
        ReflectResult res;
        // Check if the hit indeed exists
        assert(hit.m_isHit == true);

        // Mirrored direction
        Vector3D dir = normalize(ray.direction() - 2 * dot(ray.direction(), hit.m_hitNormal) * hit.m_hitNormal);
        res.m_ray = Ray(hit.m_hitPos, dir);
        
        res.m_color = m_color;
        return res;
    }
};
#endif
