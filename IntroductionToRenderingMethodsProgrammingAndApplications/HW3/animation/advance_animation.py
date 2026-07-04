# 3 function
# 1. default
# 2. 棒狀標本
# 3. lego
import math
import random
from panda3d.core import Vec3

from animation.default_animation import *


def setup_general_restore_animation(model):
    """
    【通用還原初始化】從任何變形狀態（包含樂高）切回預設狀態時『只呼叫一次』。
    利用陣列管理，確保所有 9 個零件不論飛去哪，都能在 2 秒內吸回原位。
    """
    # 1. 統一管理所有需要操控的零件清單
    parts = [
        model.body, 
        model.head, model.ear_l, model.ear_r, 
        model.leg_fl, model.leg_fr, model.leg_bl, model.leg_br, 
        model.tail_1, model.tail_2
    ]
    
    # 2. 抓取觸發瞬間，所有零件當下的狀態（Pos, Hpr）
    start_states = [(p.getPos(), p.getHpr()) for p in parts]

    # 3. 初始組裝狀態 (Pos, Hpr)
    # Hpr 全部為 (0,0,0)
    targets = [
        (Vec3(0, 0, 0),               model.body.getHpr()), # body
        (Vec3(0.0, 0.56375, -0.4375),       Vec3(0, 0, 0)), # head
        (Vec3(-0.09375, 0.12375, -0.0625),  Vec3(0, 0, 0)), # ear_l
        (Vec3(0.09375, 0.12375, -0.0625),   Vec3(0, 0, 0)), # ear_r
        (Vec3(-0.075, 0.6375, -0.25),       Vec3(0, 0, 0)), # leg_fl
        (Vec3(0.075, 0.6375, -0.25),        Vec3(0, 0, 0)), # leg_fr
        (Vec3(-0.06875, 0.375, 0.4375),     Vec3(0, 0, 0)), # leg_bl
        (Vec3(0.06875, 0.375, 0.4375),      Vec3(0, 0, 0)), # leg_br
        (Vec3(0.0, 0.58554, 0.521112),      Vec3(0, 0, 0)), # tail_1
        (Vec3(0.0, -0.329409, 0.344914),    Vec3(0, 0, 0))  # tail_2
    ]

    elapsed_time = 0.0
    duration = 2.0

    def general_restore_loop(current_time: float, dt: float):
        nonlocal elapsed_time
        
        elapsed_time = min(duration, elapsed_time + dt)
        alpha = elapsed_time / duration
        
        # 4. 迴圈批次處理所有零件的線性插值
        for i, part in enumerate(parts):
            s_pos, s_hpr = start_states[i]
            t_pos, t_hpr = targets[i]
            
            part.setPos(s_pos + (t_pos - s_pos) * alpha)
            part.setHpr(s_hpr + (t_hpr - s_hpr) * alpha)

        if alpha >= 1.0:
            model.active_anim_func = setup_walking_animation(model)

        return alpha >= 1.0

    return general_restore_loop


def setup_rod_animation(model):
    """
    【處理初始化】當貓咪切換到標本模式的瞬間『只呼叫一次』。
    同樣納入全部 9 個零件的管理，確保就算從樂高切成標本，頭跟耳朵也不會跑掉。
    """
    # 1. 管理清單與抓取當前狀態
    parts = [
        model.head, model.ear_l, model.ear_r, 
        model.leg_fl, model.leg_fr, model.leg_bl, model.leg_br, 
        model.tail_1, model.tail_2
    ]
    start_states = [(p.getPos(), p.getHpr()) for p in parts]
    
    # 2. 標本狀態的終點數值
    # 頭部跟耳朵保持不動 (原始座標)，四肢與尾巴進行折平與位移
    targets = [
        (Vec3(0.0, 0.56375, -0.4375),       Vec3(0, 0, 0)),    # head (維持)
        (Vec3(-0.09375, 0.12375, -0.0625),  Vec3(0, 0, 0)),    # ear_l (維持)
        (Vec3(0.09375, 0.12375, -0.0625),   Vec3(0, 0, 0)),    # ear_r (維持)
        (Vec3(-0.075, 0.375, -0.25),        Vec3(0, 90, 0)),   # leg_fl (高度降下，往前伸)
        (Vec3(0.075, 0.375, -0.25),         Vec3(0, 90, 0)),   # leg_fr (高度降下，往前伸)
        (Vec3(-0.06875, 0.375, 0.4375),     Vec3(0, -90, 0)),  # leg_bl (往後伸)
        (Vec3(0.06875, 0.375, 0.4375),      Vec3(0, -90, 0)),  # leg_br (往後伸)
        (Vec3(0.0, 0.58554, 0.521112),      Vec3(0, -45, 0)),  # tail_1 (壓平)
        (Vec3(0.0, -0.329409, 0.344914),    Vec3(0, 45, 0))    # tail_2 (反向折直)
    ]

    elapsed_time = 0.0
    duration = 2.0

    def rod_anim_loop(time, dt):
        nonlocal elapsed_time
        
        elapsed_time = min(duration, elapsed_time + dt)
        alpha = elapsed_time / duration
        
        # 3. 批次插值
        for i, part in enumerate(parts):
            s_pos, s_hpr = start_states[i]
            t_pos, t_hpr = targets[i]
            
            part.setPos(s_pos + (t_pos - s_pos) * alpha)
            part.setHpr(s_hpr + (t_hpr - s_hpr) * alpha)
        
        return alpha >= 1.0

    return rod_anim_loop


def setup_lego_animation(model):
    """
    【樂高模式 - 完美防穿模版】
    將所有零件強制以絕對 90 度對齊，並且限制 Y 軸（高度軸）絕對大於 0，防止積木陷進地板。
    """
    # 1. 統一管理所有需要拆解的 9 個零件清單
    parts = [
        model.head, model.ear_l, model.ear_r, 
        model.leg_fl, model.leg_fr, model.leg_bl, model.leg_br, 
        model.tail_1, model.tail_2
    ]
    
    # 2. 抓取觸發瞬間，所有零件當下的狀態（起點）
    start_states = [(p.getPos(), p.getHpr()) for p in parts]

    # =======================================================
    # 3. 樂高積木隨機大洗牌演算法
    # =======================================================
    def get_lego_hpr():
        # 確保旋轉一定是正正的直角卡榫，沒有任何歪斜
        angles = [0.0, 90.0, -90.0, 180.0]
        return Vec3(random.choice(angles), random.choice(angles), random.choice(angles))

    def get_grid_pos(scale_steps=3, step_size=0.2, is_level3=False):
        """
        強迫位置對齊網格點，並特判 Y 軸確保大於 0.0（不陷地）。
        """
        steps = [i * step_size for i in range(-scale_steps, scale_steps + 1)]
        
        x = random.choice(steps)
        y = random.choice(steps)
        z = random.choice(steps)
        
        # 【核心特判：高度防穿模】
        # 在你的模型結構中，Y 是控制零件上下高度的軸。
        # 如果隨機出來的局部 Y 座標小於 0，強制拔高到 0.05 以上，確保一定露出地面！
        if y < 0.0:
            y = random.choice([0.05, 0.2, 0.4])
            
        return Vec3(x, y, z)

    # 4. 特判與指定每個積木零件的最終混亂終點 (Target)
    targets = [
        (get_grid_pos(3, 0.2),  get_lego_hpr()),  # head (Level 2)
        (get_grid_pos(2, 0.15, True), get_lego_hpr()),  # ear_l (Level 3 - 掛在頭上，範圍縮小)
        (get_grid_pos(2, 0.15, True), get_lego_hpr()),  # ear_r (Level 3 - 掛在頭上，範圍縮小)
        (get_grid_pos(3, 0.2),  get_lego_hpr()),  # leg_fl (Level 2)
        (get_grid_pos(3, 0.2),  get_lego_hpr()),  # leg_fr (Level 2)
        (get_grid_pos(3, 0.2),  get_lego_hpr()),  # leg_bl (Level 2)
        (get_grid_pos(3, 0.2),  get_lego_hpr()),  # leg_br (Level 2)
        
        # 尾巴保持絕對垂直/水平的積木方向，且高度同樣受控大於 0
        (get_grid_pos(3, 0.2),  Vec3(random.choice([0, 90, -90, 180]), 0, 0)), # tail_1
        (get_grid_pos(2, 0.15, True), Vec3(random.choice([0, 90, -90, 180]), 0, 0))  # tail_2
    ]

    # =======================================================
    # 5. 兩階段拆卸時間軸 (0~2秒拆 Lvl2, 2~4秒拆 Lvl3)
    # =======================================================
    elapsed_time = 0.0
    duration_lvl2 = 2.0  
    duration_lvl3 = 2.0  

    def lego_anim_loop(time, dt):
        nonlocal elapsed_time
        elapsed_time += dt
        
        alpha_2 = min(1.0, elapsed_time / duration_lvl2)
        alpha_3 = 0.0
        if elapsed_time > duration_lvl2:
            alpha_3 = min(1.0, (elapsed_time - duration_lvl2) / duration_lvl3)

        # 6. 精準批次插值
        for i, part in enumerate(parts):
            s_pos, s_hpr = start_states[i]
            t_pos, t_hpr = targets[i]
            
            if i in [1, 2, 8]: # Level 3 零件 (耳朵、尾2)
                part.setPos(s_pos + (t_pos - s_pos) * alpha_3)
                part.setHpr(s_hpr + (t_hpr - s_hpr) * alpha_3)
            else:              # Level 2 零件 (頭、四肢、尾1)
                part.setPos(s_pos + (t_pos - s_pos) * alpha_2)
                part.setHpr(s_hpr + (t_hpr - s_hpr) * alpha_2)
                
        # 樂高狀態下身體完全靜止
        model.update_movement(Vec3(0, 0, 0), 0.0, dt)

        return elapsed_time >= (duration_lvl2 + duration_lvl3)

    return lego_anim_loop
