# 3 function
# 1. walking: move + 直著甩尾巴
# 2. 趴下: 趴著甩尾巴
import math
import random
from panda3d.core import Vec3


def setup_walking_animation(model):
    """
    【處理初始化】當貓咪切換到預設散步模式時『只呼叫一次』。
    負責初始化散步專用的隨機變數與計時器，並回傳每幀動畫的實作 function。
    """
    # 閉包內部的狀態變數，取代原本寫在類別內的 hasattr 屬性
    direction_timer = 0.0
    chosen_rotation_speed = 0.0

    # 動畫基本參數設定
    walk_speed = 12.0       # 走路四肢擺動頻率
    walk_amplitude = 30.0   # 四肢擺動最大角度
    tail_speed = 4.0        # 尾巴搖晃頻率
    tail_amplitude = 15.0   # 尾巴搖晃最大角度

    def walking_anim_loop(current_time: float, dt: float):
        """
        【處理動畫】這是真正每幀會被呼叫的實作 function。
        """
        nonlocal direction_timer, chosen_rotation_speed

        # 1. AI 隨機轉向決策倒計時
        direction_timer -= dt

        # 當計時器歸零，才「重新骰一次」接下來的行為
        if direction_timer <= 0:
            rand = random.random()
            if rand > 0.65:    
                chosen_rotation_speed = 60.0   # 左轉
                direction_timer = random.uniform(0.5, 1.2)
            elif rand > 0.3:  
                chosen_rotation_speed = -60.0  # 右轉
                direction_timer = random.uniform(0.5, 1.2)
            else:              
                chosen_rotation_speed = 0.0     # 專心直走
                direction_timer = random.uniform(1.5, 3.0)

        # 2. 驅動模型位移與旋轉 (Root Motion)
        velocity = Vec3(0, 2.0, 0)
        model.update_movement(velocity, chosen_rotation_speed, dt)

        # walking animation
        # 1. 四肢擺動邏輯：Minecraft 四肢是直的，走路時靠著前後交替的相位差產生對稱感
        # 右前腿和左後腿一組，左前腿和右後腿一組（相差 180 度）
        phase = current_time * walk_speed
        
        angle_group_A = math.sin(phase) * walk_amplitude
        angle_group_B = math.sin(phase + math.pi) * walk_amplitude
        
        model.leg_fr.setP(angle_group_A)
        model.leg_bl.setP(angle_group_A)
        
        model.leg_fl.setP(angle_group_B)
        model.leg_br.setP(angle_group_B)

        # 2. 尾巴波浪擺動邏輯 (核心給分點：2層階層動態連動)
        # 讓 tail_2 的擺動時間稍微落後 tail_1 一點點，做出波浪感
        tail_phase_1 = current_time * tail_speed
        tail_phase_2 = tail_phase_1 - 0.7
        
        model.tail_1.setH(math.sin(tail_phase_1) * tail_amplitude)
        model.tail_2.setR(math.sin(tail_phase_2) * tail_amplitude)

        return True

    return walking_anim_loop

def setup_lie_animation(model):
    """
    【側躺賴皮模式 - 完美貼地版】
    除了身體側翻 -90 度、頭部反向補償 90 度之外，
    特別在 TARGET_BODY_POS 中將 Z 軸下壓，讓貓咪完美躺在草地上，不再懸空！
    """
    # 1. 統一管理所有 9 個零件
    parts = [
        model.head, model.ear_l, model.ear_r, 
        model.leg_fl, model.leg_fr, model.leg_bl, model.leg_br, 
        model.tail_1, model.tail_2
    ]
    
    # 2. 捕捉觸發瞬間所有零件的狀態，以及「身體(body)」當下的狀態
    start_states = [(p.getPos(), p.getHpr()) for p in parts]
    start_body_hpr = model.body.getHpr()
    start_body_pos = model.body.getPos()

    # =======================================================
    # 3. 調整終點目標 (讓身體側翻的同時往地面沉降)
    # =======================================================
    # 角度目標：維持側翻 -90 度
    TARGET_BODY_HPR = Vec3(start_body_hpr[0], start_body_hpr[1], start_body_hpr[2])
    TARGET_BODY_POS = Vec3(0, 0, -0.22)

    # 子零件完美局部目標
    targets = [
        (Vec3(0.0, 0.5, -0.4375),       Vec3(0, 0, 20)),     # head
        (Vec3(-0.09375, 0.12375, -0.0625),  Vec3(0, 0, 0)),     # ear_l
        (Vec3(0.09375, 0.12375, -0.0625),   Vec3(0, 0, 0)),     # ear_r
        
        # 四肢癱平：微調角度與位置，讓腿看起來是平鋪在地面上
        (Vec3(-0.062, 0.32, -0.05),       Vec3(0, 90.0, 0)),    # leg_fl
        (Vec3(0.062, 0.32, -0.05),        Vec3(0, 90.0, 0)),    # leg_fr
        (Vec3(-0.085, 0.375, 0.5),     Vec3(0, 60.0, 0)),       # leg_bl
        (Vec3(0.085, 0.375, 0.5),      Vec3(0, 60.0, 0)),       # leg_br
        
        # 尾巴貼地
        (Vec3(0.0, 0.58554, 0.521112),      Vec3(0, -15.0, 0)), # tail_1
        (Vec3(0.0, -0.329409, 0.344914),    Vec3(0, 30.0, 0))   # tail_2
    ]

    elapsed_time = 0.0
    duration = 2.0
    
    # 躺好後的待機呼吸參數
    breath_speed = 3.0      
    breath_amplitude = 0.01 
    tail_idle_speed = 2.0   

    def lie_anim_loop(current_time: float, dt: float):
        nonlocal elapsed_time
        
        elapsed_time = min(duration, elapsed_time + dt)
        alpha = elapsed_time / duration  # 0.0 -> 1.0

        # --- 身體狀態插值 (旋轉的同時往下沉) ---
        model.body.setHpr(start_body_hpr + (TARGET_BODY_HPR - start_body_hpr) * alpha)
        model.body.setPos(start_body_pos + (TARGET_BODY_POS - start_body_pos) * alpha)

        # --- 所有子零件插值 ---
        for i, part in enumerate(parts):
            s_pos, s_hpr = start_states[i]
            t_pos, t_hpr = targets[i]
            
            part.setPos(s_pos + (t_pos - s_pos) * alpha)
            part.setHpr(s_hpr + (t_hpr - s_hpr) * alpha)

        model.update_movement(Vec3(0, 0, 0), 0.0, dt)

        # --- 躺好後的待機動態 ---
        if alpha >= 1.0:
            # 呼吸起伏：在已經下沉的基礎上（-0.22）進行波形震盪
            breath_offset = TARGET_BODY_POS.getZ() + math.sin(current_time * breath_speed) * breath_amplitude
            model.body.setZ(breath_offset)
            
            # 尾巴無聊拍打
            model.tail_1.setH(math.sin(current_time * tail_idle_speed) * 10.0)
            model.tail_2.setR(math.sin(current_time * tail_idle_speed) * 50.0)

        return alpha >= 1.0

    return lie_anim_loop
