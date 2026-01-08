#ifndef KD_TREE_H
#define KD_TREE_H
#include "./element.h"
#include <vector>
#include <algorithm>

#define MAX_KD_TREE_DEPTH 20

// 轴向包围盒
class AABB
{
public:
    Vec3f min; // 盒子最小顶点的坐标 (x_min, y_min, z_min)
    Vec3f max; // 盒子最大顶点的坐标 (x_max, y_max, z_max)
    AABB() : min(INFINITY), max(-INFINITY) {}
    AABB(const Vec3f& min, const Vec3f& max) : min(min), max(max) {}

    // 扩展盒子以包含另一个盒子
    void expand(const AABB& other) {
        min.x = std::min(min.x, other.min.x);
        min.y = std::min(min.y, other.min.y);
        min.z = std::min(min.z, other.min.z);
        max.x = std::max(max.x, other.max.x);
        max.y = std::max(max.y, other.max.y);
        max.z = std::max(max.z, other.max.z);
    }

    // Slab法相交检测
    bool intersect(const Vec3f& rayorig, const Vec3f& raydir, float& t_enter, float& t_exit) const {
        float tmin = -INFINITY, tmax = INFINITY;

        // 分别检查 X, Y, Z 三个slab
        for (int i = 0; i < 3; ++i) {
            // 获取当前轴的分量 (0=x, 1=y, 2=z)
            float origin_val = (i == 0) ? rayorig.x : (i == 1 ? rayorig.y : rayorig.z);
            float dir_val    = (i == 0) ? raydir.x : (i == 1 ? raydir.y : raydir.z);
            float min_val    = (i == 0) ? min.x : (i == 1 ? min.y : min.z);
            float max_val    = (i == 0) ? max.x : (i == 1 ? max.y : max.z);

            if (std::abs(dir_val) < 1e-8) {
                // 光线在该轴平行，如果起点不在范围内，则必不相交
                if (origin_val < min_val || origin_val > max_val) return false;
            } else {
                float t1 = (min_val - origin_val) / dir_val;
                float t2 = (max_val - origin_val) / dir_val;
                if (t1 > t2) std::swap(t1, t2);
                
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                
                if (tmin > tmax) return false; // 不相交
            }
        }
        
        t_enter = tmin;
        t_exit = tmax;
        return tmax > 0; // 如果 tmax < 0，盒子在光线背后
    }
};
// 获取球体的 AABB
inline AABB get_Sphere_AABB(const Sphere &s){
    return AABB(s.center - Vec3f(s.radius), s.center + Vec3f(s.radius));
}


struct KDNode {
    AABB bbox;
    KDNode *left = nullptr;
    KDNode *right = nullptr;
    std::vector<const Sphere*> objects;
    bool isLeaf = false;

    ~KDNode() {
        delete left;
        delete right;
    }
};


static KDNode* build_kd_tree(std::vector<const Sphere*>& objs, int depth) {
    KDNode* node = new KDNode();
    // 计算当前节点所有物体的整体包围盒
    for (const auto* s : objs) {
        node->bbox.expand(get_Sphere_AABB(*s));
    }

    // 终止条件：如果物体很少，或超过最大深度，直接作为叶子节点
    if (objs.size() <= 2 || depth > MAX_KD_TREE_DEPTH) {
        node->isLeaf = true;
        node->objects = objs;
        return node;
    }

    // 选择分割轴 (按深度循环选择 X, Y, Z)
    int axis = depth % 3;

    // 按球体中心在所选轴上的位置排序
    std::sort(objs.begin(), objs.end(), [axis](const Sphere* a, const Sphere* b) {
        if (axis == 0) return a->center.x < b->center.x;
        if (axis == 1) return a->center.y < b->center.y;
        return a->center.z < b->center.z;
    });

    // 取中位数进行切分
    size_t mid = objs.size() / 2;
    std::vector<const Sphere*> left_objs(objs.begin(), objs.begin() + mid);
    std::vector<const Sphere*> right_objs(objs.begin() + mid, objs.end());

    // 递归创建子节点
    node->left = build_kd_tree(left_objs, depth + 1);
    node->right = build_kd_tree(right_objs, depth + 1);

    return node;
}


static const Sphere* intersect_kd_tree(KDNode* node, const Vec3f& rayorig, const Vec3f& raydir, float& tnear) {
    if (!node) return nullptr;

    float t_enter, t_exit;
    // 如果光线没射中当前节点的 AABB，或者 AABB 在当前已找到的最短距离之外，直接跳过
    if (!node->bbox.intersect(rayorig, raydir, t_enter, t_exit) || t_enter > tnear) {
        return nullptr;
    }

    // 如果是叶子节点，遍历其中的球体
    if (node->isLeaf) {
        const Sphere* hitObj = nullptr;
        for (const auto* s : node->objects) {
            float t0 = INFINITY, t1 = INFINITY;
            if (s->intersect(rayorig, raydir, t0, t1)) {
                if (t0 < 0) t0 = t1;
                if (t0 < tnear) {
                    tnear = t0;
                    hitObj = s;
                }
            }
        }
        return hitObj;
    }

    // 如果是内部节点，递归遍历子节点
    const Sphere* hitLeft = intersect_kd_tree(node->left, rayorig, raydir, tnear);
    const Sphere* hitRight = intersect_kd_tree(node->right, rayorig, raydir, tnear);

    if (hitRight) return hitRight; // 右子树最后更新了 tnear，说明找到了更近的物品，返回它
    return hitLeft;
}

#endif