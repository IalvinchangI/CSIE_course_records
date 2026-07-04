import math
import os
from direct.showbase.ShowBase import ShowBase
from panda3d.core import AmbientLight, CardMaker, DirectionalLight, Filename, WindowProperties, Texture
from panda3d.core import Point3, Vec3

from model import CatModel
from camera import Camera
from user_control import InteractionManager


class Screen(ShowBase):
    # Render CatModel and Camera
    # Simple Light
    # Transport Event (e.g. transport animation event from InteractionManager to CatModel)
    
    def __init__(self):
        # Build Ground
        # Setup Simple Light
        # Render CatModel and Camera
        # Setup Event Transport
        # Start Game Loop
        ShowBase.__init__(self)
        # self.render.analyze()
        # self.axis = self.loader.loadModel("models/zup-axis")
        # self.axis.reparentTo(self.render)

        self.setup_ground(n=50)
        self.setup_lights()
        self.setup_model()
        self.setup_camera()
        self.interaction_mgr = InteractionManager(self, self.cat)
        
        # Start Game Loop
        self.count_down = 0
        self.taskMgr.add(self.game_loop, "UpdateGameLoop")

    def setup_ground(self, n=20):
        """使用 CardMaker 建立地板"""
        cm = CardMaker("ground_card")
        cm.setFrame(-n, n, -n, n)  # 2nx2n 的地板
        ground_node = cm.generate()

        self.ground = self.render.attachNewNode(ground_node)
        self.ground.lookAt(0, 0, -1)  # 把它放倒在 XY 平面上

        ground_tex = self.loader.loadTexture("assets/ground.png")
        if ground_tex:
            ground_tex.setMagfilter(Texture.FT_nearest)
            ground_tex.setMinfilter(Texture.FT_nearest)
        self.ground.setTexture(ground_tex)
        
    def setup_lights(self):
        """設置基礎光源"""
        # A. 建立平行光
        d_light = DirectionalLight("sun_light")
        d_light.setColor((0.3, 0.3, 0.3, 1.0))
        self.d_light_path = self.render.attachNewNode(d_light)
        self.d_light_path.setHpr(45, -45, 0)  # 從右上方往左下方照
        self.render.setLight(self.d_light_path)

        # B. 建立環境光
        a_light = AmbientLight("ambient_light")
        a_light.setColor((0.5, 0.5, 0.5, 1.0))
        self.a_light_path = self.render.attachNewNode(a_light)
        self.render.setLight(self.a_light_path)

        self.accept("space", self.toggle_directional_light)

    def toggle_directional_light(self):
        """開關燈邏輯"""
        if self.render.hasLight(self.d_light_path):
            self.render.clearLight(self.d_light_path)
        else:
            self.render.setLight(self.d_light_path)

    def setup_model(self):
        self.cat = CatModel(self.loader)
        self.cat.reparentTo(self.render)
        self.cat.setPos(0, 0, 0)

        self.accept("1", self.cat.switchTo, [CatModel.DEFAULT_STAGE, CatModel.WALK_TYPE])
        self.accept("2", self.cat.switchTo, [CatModel.ROD_STAGE, None])
        self.accept("3", self.cat.switchTo, [CatModel.LEGO_STAGE, None])

    def setup_camera(self):
        """初始化相機視角"""
        self.cam_ctrl = Camera(self)
        self.cam_ctrl.reparentTo(self.cat.root)

    def change_cursor_to_hand(self, show_hand=True):
        """切換滑鼠外觀"""
        if not hasattr(self, '_current_cursor_is_hand'):
            self._current_cursor_is_hand = None

        if self._current_cursor_is_hand == show_hand:
            return

        props = WindowProperties()
        if show_hand:
            abs_path = os.path.abspath("assets/hand_cursor.ico")
            cursor_file = Filename.fromOsSpecific(abs_path)
            
            props.setCursorFilename(cursor_file)
        else:
            props.clearCursorFilename()
            props.setCursorFilename(Filename("")) 
        self.win.requestProperties(props)
        self._current_cursor_is_hand = show_hand

    def game_loop(self, task):
        """全域遊戲迴圈"""
        current_time = task.time
        dt = globalClock.getDt()

        # 呼叫並獲取當前的互動狀態
        interaction_code = self.interaction_mgr.drag_update_task(dt)

        # 滑鼠形狀
        if interaction_code in [InteractionManager.STATE_HOVER, InteractionManager.STATE_DRAGGING]:
            self.change_cursor_to_hand(True)
        else:
            self.change_cursor_to_hand(False)

        # Animation
        if self.cat.current_stage == CatModel.DEFAULT_STAGE:
            if interaction_code == InteractionManager.STATE_CLICK:
                pass
            elif interaction_code == InteractionManager.STATE_DRAGGING:
                self.cat.switchTo(CatModel.DEFAULT_STAGE, CatModel.LIE_TYPE)
                self.count_down = 100
            elif interaction_code == InteractionManager.STATE_IDLE:
                if self.count_down > 0:
                    self.count_down -= 1
                else:
                    self.count_down = 0
                    self.cat.switchTo(CatModel.DEFAULT_STAGE, CatModel.WALK_TYPE)
        else:
            self.count_down = 0
        self.cat.animate(current_time, dt)
        
        return task.cont
