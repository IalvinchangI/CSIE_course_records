from panda3d.core import CollisionTraverser, CollisionHandlerQueue, CollisionNode, CollisionRay, CollisionBox
from panda3d.core import Point3, Vec3

class InteractionManager():
    STATE_IDLE = 0          # 放開滑鼠、無互動
    STATE_HOVER = 1         # 滑鼠移到貓咪身上，但沒按住
    STATE_CLICK = 2         # 滑鼠點擊貓咪的瞬間
    STATE_DRAGGING = 3      # 按住滑鼠並在貓咪身上拖拉 (擼貓中)

    def __init__(self, base_instance, cat_model):
        """
        base_instance: ShowBase 實例，以便存取相機、滑鼠與事件綁定
        cat_model: CatModel 實例，以便加上碰撞體並監聽點擊
        """
        self.base = base_instance
        self.cat_model = cat_model

        # Collision
        self.c_traverser = CollisionTraverser()
        self.c_queue = CollisionHandlerQueue()
        
        self.picker_ray = CollisionRay()
        picker_node = CollisionNode('mouse_picker_ray')
        picker_node.addSolid(self.picker_ray)
        picker_np = self.base.cam.attachNewNode(picker_node)
        self.c_traverser.addCollider(picker_np, self.c_queue)
        self._setup_cat_collision()

        # Param
        self.current_mouse_state = self.STATE_IDLE
        self.drag_intensity = 0.0          # 當前幀的拖拉強度 (根據滑鼠移動速度)
        self.drag_delta = Vec3(0, 0, 0)    # 當前幀的拖拉方向與距離向量

        # 內部歷史追蹤變數
        self.is_dragging = False        
        self.last_mouse_pos = Vec3(0, 0, 0) 

        # 綁定滑鼠事件
        self.base.accept("mouse1", self.on_mouse_down)
        self.base.accept("mouse1-up", self.on_mouse_up)

    def _setup_cat_collision(self):
        c_node = CollisionNode("clickable_cat")
        
        c_box = CollisionBox(Point3(0, 0.4, -0.05), 0.2, 0.35, 0.8)
        c_node.addSolid(c_box)
        self.cat_collision_np = self.cat_model.cat.attachNewNode(c_node)
        
        # self.cat_collision_np.show()

    def on_mouse_down(self):
        """ 當玩家『按住』左鍵不放 """
        if self.base.mouseWatcherNode.hasMouse():
            self.is_dragging = True
            # 初始化點擊瞬間的滑鼠位置
            m_pos = self.base.mouseWatcherNode.getMouse()
            self.last_mouse_pos = Vec3(m_pos.getX(), m_pos.getY(), 0)

    def on_mouse_up(self):
        """ 當玩家『放開』左鍵 """
        self.is_dragging = False

    def drag_update_task(self, dt):
        """ 
        每幀由外部 Game Loop 主動呼叫的辨識更新函式。
        僅更新辨識狀態、強度與位移量，不做任何模型角度變動。
        """
        self.drag_intensity = 0.0
        self.drag_delta = Vec3(0, 0, 0)

        # 檢查滑鼠是否在視窗內，不在的話直接歸類為 IDLE
        if not self.base.mouseWatcherNode.hasMouse():
            self.current_mouse_state = self.STATE_IDLE
            return self.current_mouse_state

        # 1. 獲取當前滑鼠 2D 座標
        m_pos = self.base.mouseWatcherNode.getMouse()
        current_pos = Vec3(m_pos.getX(), m_pos.getY(), 0)
        
        # 2. 計算這一幀滑鼠移動了多少
        mouse_delta = current_pos - self.last_mouse_pos
        self.last_mouse_pos = current_pos 

        # 3. 發射射線，檢查滑鼠是否停留在貓咪身上
        self.picker_ray.setFromLens(self.base.camNode, m_pos.getX(), m_pos.getY())
        self.c_traverser.traverse(self.base.render)
        
        is_touching_cat = False
        if self.c_queue.getNumEntries() > 0:
            self.c_queue.sortEntries()
            if self.c_queue.getEntry(0).getIntoNodePath().getName() == "clickable_cat":
                is_touching_cat = True

        # 4. 滑鼠狀態辨識
        if is_touching_cat:
            if self.is_dragging:
                # 狀況 A：按住滑鼠、且在貓咪身上移動 -> 擼貓進行中
                self.current_mouse_state = self.STATE_DRAGGING
                
                # 計算並導出拖拉物理資料
                if mouse_delta.length() > 0.001:
                    self.drag_delta = mouse_delta
                    # 計算拖拉強度（數值越高代表滑鼠擼得越快）
                    self.drag_intensity = mouse_delta.length() * 5.0
            else:
                # 狀況 B：滑鼠只是懸停在貓咪身上，沒按按鍵
                self.current_mouse_state = self.STATE_HOVER
        else:
            # 狀況 C：滑鼠沒指著貓咪
            self.current_mouse_state = self.STATE_IDLE
        
        return self.current_mouse_state
