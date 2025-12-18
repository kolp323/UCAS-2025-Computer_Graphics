#include <iostream>
#include <vector>
#include <meshark/geometry-mesh.h>
#include <cassert>

using namespace meshark;

int main(){
    GeometryMesh mesh;

    // 创建顶点
    // 修复方法：框架在 createVertexAttribute 中支持 glm::vec3
    // 这样只传一个参数，避开 std::forward<Args...> 的展开问题
    Vertex v_center = mesh.createVertex(glm::vec3(0.0f, 0.0f, 0.0f)); 
    Vertex v1 = mesh.createVertex(glm::vec3(1.0f, 0.0f, 0.0f));       
    Vertex v2 = mesh.createVertex(glm::vec3(0.0f, 1.0f, 0.0f));       
    Vertex v3 = mesh.createVertex(glm::vec3(-1.0f, -1.0f, 0.0f));

    // 创建半边
    // --> Outgoing edges (从中心发出的边)
    HalfEdge h_c1 = mesh.createHalfEdge(); // center -> v1
    HalfEdge h_c2 = mesh.createHalfEdge(); // center -> v2
    HalfEdge h_c3 = mesh.createHalfEdge(); // center -> v3

    // <-- Incoming edges (指向中心的边，即 twin)
    HalfEdge h_1c = mesh.createHalfEdge(); // v1 -> center
    HalfEdge h_2c = mesh.createHalfEdge(); // v2 -> center
    HalfEdge h_3c = mesh.createHalfEdge(); // v3 -> center

    // 配置半边属性 (Tip, Tail, Twin)
    // 设置 h_c1 (center -> v1)
    h_c1->tail = v_center; h_c1->tip = v1; h_c1->twin = h_1c;
    // 设置 h_1c (v1 -> center)
    h_1c->tail = v1; h_1c->tip = v_center; h_1c->twin = h_c1;

    // 设置 h_c2 (center -> v2)
    h_c2->tail = v_center; h_c2->tip = v2; h_c2->twin = h_2c;
    // 设置 h_2c (v2 -> center)
    h_2c->tail = v2; h_2c->tip = v_center; h_2c->twin = h_c2;

    // 设置 h_c3 (center -> v3)
    h_c3->tail = v_center; h_c3->tip = v3; h_c3->twin = h_3c;
    // 设置 h_3c (v3 -> center)
    h_3c->tail = v3; h_3c->tip = v_center; h_3c->twin = h_c3;

    // 链接 Next 指针以形成环路
    h_1c->next = h_c2; // 构成面 (v1, center, v2) 的一部分
    h_2c->next = h_c3; // 构成面 (v2, center, v3) 的一部分
    h_3c->next = h_c1; // 构成面 (v3, center, v1) 的一部分，形成闭环


    // 将顶点的 halfEdge 指向其中任意一个 outgoing edge
    v_center->halfEdge() = h_c1;

    std::cout << "拓扑构建完成。中心点 Index: " << mesh.index(v_center) << std::endl;
    std::cout << "预期邻居顺序 (逆时针): V" << mesh.index(v1) << " -> V" << mesh.index(v2) << " -> V" << mesh.index(v3) << std::endl;

    // 测试遍历逻辑
    std::cout << "\n=== 运行迭代器测试 (Running Iterator Test) ===" << std::endl;
    
    int count = 0;
    std::vector<int> visited_indices;

    for (HalfEdge h : v_center->outgoingHalfEdges()) {
        Vertex neighbor = h->tip;
        
        std::cout << "遍历到半边: " << mesh.index(h) 
                  << " | 连接: V" << mesh.index(h->tail) << " -> V" << mesh.index(h->tip) << std::endl;

        // 验证起点必须是 center
        if (h->tail != v_center) {
            std::cerr << "[错误] 半边的 tail 不是中心点！" << std::endl;
            return 1;
        }

        visited_indices.push_back(mesh.index(neighbor));
        count++;

        // 防止死循环
        if (count > 10) {
            std::cerr << "[错误] 检测到死循环！operator++ 的终止条件可能写错了。" << std::endl;
            break;
        }
    }

    // 验证结果
    std::cout << "\n=== 测试结果验证 ===" << std::endl;
    if (count == 3) {
        std::cout << "[成功] 找到了 3 个邻居。" << std::endl;
        // 检查是否覆盖了 1, 2, 3
        bool found1 = false, found2 = false, found3 = false;
        for(int idx : visited_indices) {
            if(idx == 1) found1 = true;
            if(idx == 2) found2 = true;
            if(idx == 3) found3 = true;
        }
        if (found1 && found2 && found3) {
            std::cout << "[成功] 所有邻居顶点 (1, 2, 3) 均已访问！" << std::endl;
        } else {
            std::cout << "[失败] 邻居访问不完整。" << std::endl;
        }
    } else {
        std::cout << "[失败] 邻居数量不对。预期 3 个，实际 " << count << " 个" <<std::endl;
        std::cout << "请检查 include/meshark/mesh-elements.h 中的 operator++ 实现。" << std::endl;
    }

    return 0;
}
