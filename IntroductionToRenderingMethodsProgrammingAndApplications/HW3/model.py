from panda3d.core import NodePath, Texture
from panda3d.core import Point3, Vec3
import os
import random

from animation.advance_animation import *
from animation.default_animation import *


class CatModel():
    # Scene Graph
    # Texture
    # Animation (from ./animation/*)
    # Keep Animation Type/Stage
    
    ASSETS_PATH = "assets"
    TEXTURE = "tabby.png"

    DEFAULT_STAGE = "default"
    ROD_STAGE = "rod"
    LEGO_STAGE = "lego"

    WALK_TYPE = "walking"
    LIE_TYPE = "lie"
    TURN_TYPE = "turn_head"

    def __init__(self, loader_instance):
        self.loader = loader_instance

        # 1. 初始化模型根節點（虛擬母節點）
        self._cat_root = NodePath("CatRoot")
        
        # 2. 定義各零件的相對路徑
        body_path = os.path.join(self.ASSETS_PATH, "body.obj")
        head_path = os.path.join(self.ASSETS_PATH, "head.obj")
        tail1_path = os.path.join(self.ASSETS_PATH, "tail1.obj")
        tail2_path = os.path.join(self.ASSETS_PATH, "tail2.obj")
        ear_l_path = os.path.join(self.ASSETS_PATH, "headEarL.obj")
        ear_r_path = os.path.join(self.ASSETS_PATH, "headEarR.obj")
        
        leg_fr_path = os.path.join(self.ASSETS_PATH, "frontLegR.obj")
        leg_fl_path = os.path.join(self.ASSETS_PATH, "frontLegL.obj")
        leg_br_path = os.path.join(self.ASSETS_PATH, "backLegR.obj")
        leg_bl = os.path.join(self.ASSETS_PATH, "backLegL.obj")
        
        # 3. 載入原始 3D 模型零件 (自帶 Minecraft 絕對位置數據)
        self.body = self.loader.loadModel(body_path)
        raw_head = self.loader.loadModel(head_path)
        raw_tail_1 = self.loader.loadModel(tail1_path)
        raw_tail_2 = self.loader.loadModel(tail2_path)
        raw_ear_l = self.loader.loadModel(ear_l_path)
        raw_ear_r = self.loader.loadModel(ear_r_path)
        raw_leg_fr = self.loader.loadModel(leg_fr_path)
        raw_leg_fl = self.loader.loadModel(leg_fl_path)
        raw_leg_br = self.loader.loadModel(leg_br_path)
        raw_leg_bl = self.loader.loadModel(leg_bl)

        # Scene Graph
        self.body.reparentTo(self._cat_root)

        # --- 左前腿關節 ---
        self.leg_fl = NodePath("joint_front_left")
        self.leg_fl.reparentTo(self.body)
        self.leg_fl.setPos(-0.075, 0.6375, -0.25)
        raw_leg_fl.reparentTo(self.leg_fl)
        raw_leg_fl.setPos(0.075, -0.6375, 0.25)

        # --- 右前腿關節 ---
        self.leg_fr = NodePath("joint_front_right")
        self.leg_fr.reparentTo(self.body)
        self.leg_fr.setPos(0.075, 0.6375, -0.25)
        raw_leg_fr.reparentTo(self.leg_fr)
        raw_leg_fr.setPos(-0.075, -0.6375, 0.25)

        # --- 左後腿關節 ---
        self.leg_bl = NodePath("joint_back_left")
        self.leg_bl.reparentTo(self.body)
        self.leg_bl.setPos(-0.075, 0.375, 0.4375)
        raw_leg_bl.reparentTo(self.leg_bl)
        raw_leg_bl.setPos(0.075, -0.375, -0.4375)

        # --- 右後腿關節 ---
        self.leg_br = NodePath("joint_back_right")
        self.leg_br.reparentTo(self.body)
        self.leg_br.setPos(0.075, 0.375, 0.4375)
        raw_leg_br.reparentTo(self.leg_br)
        raw_leg_br.setPos(-0.075, -0.375, -0.4375)

        # --- 頭部關節 ---
        self.head = NodePath("joint_head")
        self.head.reparentTo(self.body)
        self.head.setPos(0.0, 0.56375, -0.4375)
        raw_head.reparentTo(self.head)
        raw_head.setPos(0.0, -0.56375, 0.4375)

        # 左耳
        self.ear_l = NodePath("joint_ear_l")
        self.ear_l.reparentTo(self.head)
        self.ear_l.setPos(-0.09375, 0.12375, -0.0625)
        raw_ear_l.reparentTo(self.ear_l)
        raw_ear_l.setPos(0.09375, -0.6875, 0.5)

        # 右耳
        self.ear_r = NodePath("joint_ear_r")
        self.ear_r.reparentTo(self.head)
        self.ear_r.setPos(0.09375, 0.12375, -0.0625)
        raw_ear_r.reparentTo(self.ear_r)
        raw_ear_r.setPos(-0.09375, -0.6875, 0.5)

        # --- 尾巴第一節修正 (把空關節拉回身體後端) ---
        self.tail_1 = NodePath("joint_tail_1")
        self.tail_1.reparentTo(self.body)
        self.tail_1.setPos(0.0, 0.58554, 0.521112)
        raw_tail_1.reparentTo(self.tail_1)
        raw_tail_1.setPos(0.0, -0.58554, -0.521112) 

        # --- 尾巴第二節 (必須掛在第一節關節下，達成 3 層階層) ---
        self.tail_2 = NodePath("joint_tail_2")
        self.tail_2.reparentTo(self.tail_1) 
        self.tail_2.setPos(0.0, -0.329409, 0.344914)
        raw_tail_2.reparentTo(self.tail_2)
        raw_tail_2.setPos(0.0, -0.256131, -0.866026)

        # Texture
        texture_path = os.path.join(self.ASSETS_PATH, self.TEXTURE)
        if os.path.exists(texture_path):
            cat_texture = self.loader.loadTexture(texture_path)
            if cat_texture:
                cat_texture.setMagfilter(Texture.FT_nearest)
                cat_texture.setMinfilter(Texture.FT_nearest)
            self._cat_root.setTexture(cat_texture, 1)
        else:
            print(f"警告：未在 {texture_path} 找到貼圖檔案。")

        # 修正方向偏轉
        self.body.setP(90)

        # 初始化參數
        self.current_stage = self.DEFAULT_STAGE
        self.current_type = self.WALK_TYPE
        self.active_anim_func = setup_walking_animation(self)

    def reparentTo(self, parent):
        self._cat_root.reparentTo(parent)
    
    def setPos(self, x, y, z):
        self._cat_root.setPos(x, y, z)
    
    @property
    def root(self):
        return self._cat_root
    
    @property
    def cat(self):
        return self.body
    
    def update_movement(self, velocity: Vec3, rotation_speed: float, dt: float):
        """
        velocity: 局部座標的前進速度，例如 Vec3(0, 5.0, 0) 代表每秒向前走 5 單位
        rotation_speed: 每秒轉動的角度（Heading）
        dt: globalClock.getDt()，確保動畫在不同幀率下速度一致
        """
        current_hpr = self._cat_root.getHpr()
        new_h = current_hpr.getX() + (rotation_speed * dt)
        self._cat_root.setH(new_h)
        
        world_movement = self._cat_root.getQuat().getForward() * velocity.getY() * dt
        current_pos = self._cat_root.getPos()
        self._cat_root.setPos(current_pos + world_movement)

    def switchTo(self, stage: str, type_: str):
        if self.current_stage == stage and self.current_type == type_:
            return
        if self.current_stage != stage:
            if stage == self.ROD_STAGE:
                self.active_anim_func = setup_rod_animation(self)
            elif stage == self.LEGO_STAGE:
                self.active_anim_func = setup_lego_animation(self)
            elif stage == self.DEFAULT_STAGE:
                self.active_anim_func = setup_general_restore_animation(self)
        elif stage == self.DEFAULT_STAGE:
            if type_ == self.WALK_TYPE:
                self.active_anim_func = setup_general_restore_animation(self)
            elif type_ == self.LIE_TYPE:
                self.active_anim_func = setup_lie_animation(self)
        
        self.current_stage = stage
        self.current_type = type_

    def animate(self, time, dt):
        if self.active_anim_func is not None:
            self.active_anim_func(time, dt)
