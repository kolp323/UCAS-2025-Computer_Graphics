#!/bin/bash

if [ -z "$1" ]; then
    echo "错误: 请提供简化率参数 (例如: 0.5)"
    echo "用法: ./run.sh <ratio>"
    exit 1
fi

RATIO=$1
OUTPUT_DIR="./output/$RATIO"
ASSETS_DIR="./assets"
EXECUTABLE="./build/meshark/simplify"

# =========================
# 运行程序
# =========================
if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir -p "$OUTPUT_DIR"
fi

echo "=== 开始批量简化 (简化率: $RATIO) ==="

found_files=false
failed_tasks=()   # 用于记录失败的任务

for input_path in "$ASSETS_DIR"/*.obj; do
    if [ ! -e "$input_path" ]; then
        echo "警告: 在 $ASSETS_DIR 下没有找到 .obj 文件"
        break
    fi
    found_files=true

    filename=$(basename "$input_path")
    output_path="$OUTPUT_DIR/$filename"

    echo "正在处理: $filename ..."

    if [ -e "$output_path" ]; then
        echo "  -> 已存在，跳过"
        continue
    fi

    # 执行核心命令
    "$EXECUTABLE" "$input_path" "$output_path" "$RATIO"
    ret=$?

    if [ $ret -eq 0 ]; then
        echo "  -> 完成"
    else
        echo "  -> [失败] 处理 $filename 时出错 (exit code=$ret)"
        failed_tasks+=("$filename")
    fi
done

# =========================
# 结果汇总
# =========================
if [ "$found_files" = true ]; then
    echo
    echo "=== 任务汇总 ==="
    if [ ${#failed_tasks[@]} -eq 0 ]; then
        echo "✅ 所有任务成功完成!"
    else
        echo "❌ 以下任务处理失败 (${#failed_tasks[@]} 个):"
        for f in "${failed_tasks[@]}"; do
            echo "  - $f"
        done
    fi
    echo "输出目录: $OUTPUT_DIR"
else
    echo "❌ 未处理任何文件。"
fi
